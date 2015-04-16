#include <iostream>
#include <sstream>
using namespace std;


string op_value[] = {"OR_OP",
		     "AND_OP", 
		     "EQ_OP",
		     "NE_OP",
		     "LT",
		     "GT",
		     "LE_OP",
		     "GE_OP",
		     "Plus",
		     "Minus",
		     "Mult",
		     "Assign_exp",
		     "Div"
};

string unop_value[] = {"Minus",
		       "Not",
		       "PP"

};

string fallInstr(string op){
  if (op == "LT")
    return "jge";
  else if (op == "GT")
    return "jle";
  else if (op == "GE_OP")
    return "jl";
  else if (op == "LE_OP")
    return "jg";
  else if (op == "EQ_OP")
    return "jne";
  else if (op == "NE_OP")
    return "je";
  else return "j";
}


string notFallInstr(string op){
  if (op == "LT")
    return "jl";
  else if (op == "GT")
    return "jg";
  else if (op == "GE_OP")
    return "jge";
  else if (op == "LE_OP")
    return "jle";
  else if (op == "EQ_OP")
    return "je";
  else if (op == "NE_OP")
    return "jne";
  else return "j";
}


/* Function to convert to string from basic types */
template<class T>
string toString(T arg){
  stringstream ss;
  ss << arg;
  string s = ss.str();
  return s;
}

//global array of code
vector<Code*> codeStack;

/* Function to print the entire code stack */

void printCodeStack(){
  int l = codeStack.size();
  for(int i = 0; i < l; i++){
    codeStack[i]->print();
  }
}

/* reset the code Stack to empty */

void resetCodeStack(){
  int l = codeStack.size();
  for (int i = 0; i < l; i++){
    delete codeStack[i];
  }
  codeStack.clear();
}

int nextInstr(){
  return codeStack.size();
}

Code* getInstr(int index){
  return codeStack[index];
}

template<class T>
int isLexp(T* obj){
  if (dynamic_cast<const Identifier*>(obj) || dynamic_cast<const Identifier*>(obj)) return 1;
  else return 0;
}

/* To check if src is of type dst */

template<class S, class R>
int isType(S* src, R *dst){
  if (dynamic_cast<const R*>(src)) return 1;
  else return 0;
}


////////////////////////////////////////////////////
/* the stack to maintain current in use registers */
////////////////////////////////////////////////////

vector<pair<Register*, int> > usedReg;

void pushUsedReg(Register* reg, int d){
  usedReg.push_back(pair<Register*, int>(reg, d));
}

void popUsedReg(){
  if (usedReg.size() > 0)
    usedReg.pop_back();
}

void saveUsedReg(){
  int l = usedReg.size();
  for(int i = 0; i < l; i++){
    if (usedReg[i].second == 1){
      codeStack.push_back(new Instr("pushi", (usedReg[i].first)->getName()));
    }
    else {
      codeStack.push_back(new Instr("pushf", (usedReg[i].first)->getName()));
    }
  }
}

void loadUsedReg(){
  int l = usedReg.size();
  for(int i = l - 1; i >= 0; i--){
    if (usedReg[i].second == 1){
      codeStack.push_back(new Instr("loadi", "ind(esp)", (usedReg[i].first)->getName()));
      codeStack.push_back(new Instr("popi", "1"));
    }
    else {
      codeStack.push_back(new Instr("loadf", "ind(esp)", (usedReg[i].first)->getName()));
      codeStack.push_back(new Instr("popf", "1"));
    }
  }
}

////////////////////////////////
/* code for   support classes */
////////////////////////////////    

Type::Type() :
  tag(Ok) {
}
Type::Type(Kind kindval) :
  tag(kindval) {
}

Type::Type(Kind kindval, Basetype btype) :
  tag(kindval), basetype(btype) {
  if (btype == Int) dim = 4;
  else if (btype == Float) dim = 4;
}

Type::Type(Kind kindval, Type* ptd) :
  tag(kindval), pointed(ptd) {
}

Type::~Type()
{
  switch (tag) {
  case Pointer: delete pointed;
    break;
  }
}

void Type::printType(){ 
  switch (tag) {
  case Base: switch (basetype) {
       case Int: std::cout << "Int" << std::endl;
       break;
       case Float: std::cout << "Float" << std::endl;
    };
    break;
  case Error:std::cout << "Error" << std::endl;
    break;
  case Ok:std::cout << "Ok" << std::endl;
    break;
  case Pointer:std::cout << "Pointer to\n ";
    pointed->printType();
  }
}

int Type::calcSize(){
  if (tag == Base){
    if (basetype == Int) return 4;
    else if (basetype == Float) return 4;
    else if (basetype == Void) return 0;
    else return 0;
  }
  else if (tag == Pointer){
    return dim * pointed->calcSize();
  }
  else return 0;
}

// int Type::calcFactor(){
//   if (tag == Base) return 1;
//   else if (tag == Pointer){
//     return pointed->calcSize();
//   }
//   else return 0;
// }

Type::Basetype Type::getBasetype(){
  if (tag == Base){
    return basetype;
  }
  else if (tag == Pointer){
    return pointed->getBasetype();
  }
  else return Void; 
}

bool equal(Type *t1, Type *t2) {
  if (t1->tag == Type::Base && t2->tag == Type::Base)
    return (t1->basetype == t2->basetype);
  else if (t1->tag == Type::Pointer && t2->tag == Type::Pointer)
    return equal(t1->pointed, t2->pointed);
  else return false;
}

bool compatible(Type *t1, Type *t2) {
  if ((t1->basetype == Type::Int || t1->basetype == Type::Float)
      && (t2->basetype == Type::Int || t2->basetype == Type::Float))
    return 1;
  else
    return equal(t1, t2);
}

Type* formType(Type* t1, Type *t2, int lineNo){
  if (t1->tag == Type::Error || t2->tag == Type::Error){
    return new Type(Type::Error);
  }
  else if (!compatible(t1, t2)){
    cout<<"Incompatible Types at lineNo : "<<lineNo<<endl; exit(0);
    return new Type(Type::Error);
  }
  else if (equal(t1, t2)){
    return t1;
  }
  else if (t1->basetype == Type::Float){
    return t1;
  }
  else if (t2->basetype == Type::Float){
    return t2;
  }
  else return t1;
}


Type::Basetype relBaseType(BasicVarType inp){
  switch(inp){
  case BasicVarType::INT:
    return Type::Int;
    break;
  case BasicVarType::VOID:
    return Type::Void;
    break;
  case BasicVarType::FLOAT:
    return Type::Float;
    break;
  }
}

Type* getVarType(GlType *typ){
  if (typ->type == VarType::BASIC){
    Type *t = new Type(Type::Base, relBaseType(((BasicType*)typ)->typeName));
    return t;
  }
  else {
    Type* t = new Type(Type::Pointer, getVarType(((ArrayType*)typ)->typeName));
    t->dim = ((ArrayType*)typ)->dim;
    return t;
  }
}



/* Deifnitions for register class */

Register::Register(string _name){
  name = _name;
}

/* getter method to obtain the register name */
string Register::getName(){
  return name;
}


/* Definition for the Code Class and its subclass */

/* Helper Class and Global to generate new label */
int labelCount = 0;

void resetLabelCount(){
  labelCount = 0;
}

string getNextLabel(){
  stringstream ss;
  ss<<labelCount;
  labelCount++;
  string s = ss.str();
  return "l"+s;
}


GotoInstr :: GotoInstr(string _func){
  func = _func;
  hasLabel = 0;
  argCount = 1;
  isGoto = 1;
}

GotoInstr :: GotoInstr(string _func, string _arg1){
  func = _func;
  arg1 = _arg1;
  hasLabel = 0;
  argCount = 1;
  isGoto = 1;
}

void GotoInstr :: backpatch(Code* code){
  code->setLabel();
  arg1 = code->getLabel();
}

void GotoInstr :: setLabel(){
  if (!hasLabel){
    // set the label
    label = getNextLabel();
    hasLabel = 1;
  }
}

string GotoInstr :: getLabel(){
  if (hasLabel)
    return label;
  else return "";
}

void GotoInstr :: print(){
  if (hasLabel) cout<<label<<":";
  cout<<"\t"<<func<<"("<<arg1<<");"<<endl;
}

Instr :: Instr(){
  isGoto = 0;
  func = "";
  argCount = 0;
  hasLabel = 0;
}

Instr :: Instr(string _func){
  isGoto = 0;
  func = _func;
  argCount = 0;
  hasLabel = 0;
}

Instr :: Instr(string _func, string _arg1){
  isGoto = 0;
  func = _func;
  arg1 = _arg1;
  argCount = 1;
  hasLabel = 0;
}

Instr :: Instr(string _func, string _arg1, string _arg2){
  func = _func;
  arg1 = _arg1;
  arg2 = _arg2;
  argCount = 2;
  hasLabel = 0;
  isGoto = 0;
}

void Instr :: setLabel(){
  if (!hasLabel){
    // if has no label
    label = getNextLabel();
    hasLabel = 1;
  }
}

string Instr :: getLabel(){
  if (hasLabel) return label;
  else return "";
}

void Instr :: print(){
  if (hasLabel) cout<<label<<":";
  if (argCount == 0)
    cout<<"\t"<<func<<";"<<endl;
  else if (argCount == 1)
    cout<<"\t"<<func<<"("<<arg1<<");"<<endl;  
  else   cout<<"\t"<<func<<"("<<arg1<<","<<arg2<<");"<<endl;  
}

void Instr :: backpatch(Code* code){}

HCode :: HCode(string _func){
  func = _func;
}

void HCode :: print(){
  cout<<func<<endl;
}

void HCode :: backpatch(Code* temp){;}
string HCode :: getLabel(){
   return "";
}

void HCode :: setLabel(){
  if (!hasLabel){
    // set the label
    label = getNextLabel();
    hasLabel = 1;
  }
}

LCode :: LCode(){
  hasLabel = 0;
  label = "";
}

void LCode :: setLabel(){
  if (!hasLabel){
    // if has no label
    label = getNextLabel();
    hasLabel = 1;
  }
}

string LCode :: getLabel(){
  if (hasLabel) return label;
  else return "";
}

void LCode :: print(){
  if (hasLabel) cout<<label<<":"<<endl;
}

void LCode :: backpatch(Code* temp){;}

void CList::add(Code* code){
  arr.push_back(code);
}

void CList::backpatch(Code* code){
  for (int i = 0; i < arr.size(); i++){
    arr[i]->backpatch(code);
  }
}

void CList::merge(CList* clist){
  for (int i = 0; i < (clist->arr).size(); i++){
    arr.push_back(clist->arr[i]);
  }
}

/* Definitions for the AST's */


ExpAst* getConstType(Type* t){
  if (t->basetype == Type::Int){
    IntConst* temp = new IntConst(0);
    temp->setType(t);
    return temp;
  }
  else if (t->basetype == Type::Float){
    FloatConst* temp = new FloatConst(0);
    temp->setType(t);
    return temp;
  }
  else return 0;
}


int abstract_astnode :: getrType(){
  return rType;
}

StmtAst :: StmtAst(){
  nextList = new CList();
}

Type* StmtAst::getType(){
  return astnode_type;
}

void StmtAst::setType(Type* t){
  astnode_type = t;
}

ExpAst::ExpAst(){
  fall = 1;
  arith = 1;
  nextList = new CList();
  trueList = new CList();
  falseList = new CList();
}

Type* ExpAst::getType(){
  return astnode_type;
}

void ExpAst::setType(Type* t){
  astnode_type = t;
}

void ExpAst::setArith(){
  arith = 0;
}

void ExpAst::setLogical(){
  logical = 1;
}


FuncDef :: FuncDef(StmtAst* _node1, string _name){
  node1 = _node1;
  isMain = 0;
  name = _name;
}

void FuncDef :: setMain(){
  isMain = 1;
}

void FuncDef :: print(){
  node1->print();
}


BlockAst :: BlockAst(){
}

void BlockAst :: print(){
  cout<<"(Block [";
  for (int i = 0; i < vec.size(); i++) {
    vec[i]->print();
    if (i != vec.size() - 1){
      cout<<endl;
    }
  }
  cout<<"])";

}

void BlockAst :: add(StmtAst* stmtAst){
  vec.push_back(stmtAst);
}


Ass :: Ass(ExpAst* node1, ExpAst* node2) {
  this->node1 = node1;
  this->node2 = node2;
  
  if (node1 != 0 && node2 != 0){
    Type* t1 = node1->getType();
    Type* t2 = node2->getType();
    if (t1->basetype == Type::Int && t2->basetype == Type::Float){
      this->node2 = new ToInt(node2);
    }
    else if (t1->basetype == Type::Float && t2->basetype == Type::Int){
      this->node2 = new ToFloat(node2);
    }
  }
}

void Ass :: print(){
  if (node1 != 0 && node2 != 0){
    cout<<"(Ass ";
    node1->print();
    cout<<" ";
    node2->print();
    cout<<")";
  }
  else {
    cout<<"(Empty)";
  }
}


While :: While(ExpAst* node1, StmtAst* node2) {
  this->node1 = node1;
  if (!node1->logical){
    this->node1 = new Op(this->node1, getConstType(node1->getType()), OpType::NE_OP);
  }
  (this->node1)->setArith();
  this->node2 = node2;
}

void While :: print(){
  cout<<"(While ";
  node1->print();cout<<endl;
  node2->print();
  cout<<")";
}

For :: For(ExpAst* node1, ExpAst* node2, ExpAst* node3, StmtAst* node4) {
  this->node1 = node1;
  this->node2 = node2;
  if (!node2->logical){
    this->node2 = new Op(this->node2, getConstType(node2->getType()), OpType::NE_OP);
  }
  (this->node2)->setArith();
  this->node3 = node3;
  this->node4 = node4;
}

void For :: print(){
  cout<<"(For ";
  node1->print();cout<<endl;
  node2->print();cout<<endl;
  node3->print();cout<<endl;
  node4->print();
  cout<<")";
}

Return :: Return(ExpAst* node1){
  this->node1 = node1;
  retOffset = 4;
  
}

void Return :: setRetOffset(int val){
  retOffset = val;
}

void Return :: print(){
  cout<<"(Return ";
  node1->print();
  cout<<")";
}

If :: If(ExpAst* node1, StmtAst* node2, StmtAst* node3){
  this->node1 = node1;
  if (!node1->logical){
    this->node1 = new Op(this->node1, getConstType(node1->getType()), OpType::NE_OP);
  }
  (this->node1)->setArith();
  this->node2 = node2;
  this->node3 = node3;
}

void If :: print(){
  cout<<"(If ";
  node1->print();cout<<endl;
  node2->print();cout<<endl;
  node3->print();
  cout<<")";
}


Op :: Op(ExpAst* _node1, ExpAst* _node2, OpType _op) {
  node1 = _node1;
  node2 = _node2;
  op = _op;
  rType = 0;
  logical = 0;
  
  string opr = op_value[op];
  
  if (opr == "EQ_OP" || opr == "NE_OP" || opr == "LT" || opr == "GT" || opr == "LE_OP" || opr == "GE_OP") logical = 1;
  
  if (opr == "Assign_exp"){
    Type* t1 = node1->getType();
    Type* t2 = node2->getType();
    if (t1->basetype == Type::Int && t2->basetype == Type::Float){
      node2 = new ToInt(node2);
    }
    else if (t1->basetype == Type::Float && t2->basetype == Type::Int){
      node2 = new ToFloat(node2);
    }
  }
  else if (!(opr == "OR_OP" || opr == "AND_OP")){
    Type* t1 = node1->getType();
    Type* t2 = node2->getType();
    if (!(t1->basetype == Type::Int && t2->basetype == Type::Int)){
      // Int types to be converted to float
      if (t1->basetype == Type::Int){
	node1 = new ToFloat(node1);
      }
      if (t2->basetype == Type::Int){
	node2 = new ToFloat(node2);
      }
    }
  }
  
  if (opr == "AND_OP" || opr == "OR_OP"){
    logical = 1; 
    if (!node1->logical){
      node1 = new Op(node1, getConstType(node1->getType()), OpType::NE_OP);
    }
    if (!node2->logical){
      node2 = new Op(node2, getConstType(node2->getType()), OpType::NE_OP);
    }
  }
  
}

void Op :: print(){
  string opr = op_value[op];

  if (!(opr == "OR_OP" || opr == "AND_OP" || opr == "Assign_exp")){
    Type *t = node1->getType();
    if (t->basetype == Type::Int) opr += "_Int";
    else if (t->basetype == Type::Float) opr += "_Float";
  }

  cout<<"("<<opr<<" ";
  node1->print();
  cout<<" ";
  node2->print();
  cout<<")";
}

UnOp :: UnOp(ExpAst* node1, UnOpType _op) {
  this->node1 = node1;
  op = _op;
  rType = 0;
  if (op == UnOpType::NOT){
    logical = 1;
  }
  else logical = 0;
}

UnOp :: UnOp(UnOpType _op){
  op = _op;
  logical = 0;
  rType = 0;
  if (op == UnOpType::NOT) logical = 1;
}


void UnOp :: print(){
  cout<<"("<<unop_value[op]<<" "; 
  node1->print();
  cout<<")";
}

void UnOp::setExp(ExpAst* node1){
  this->node1 = node1;
  // check for lexp here for PP
  if (op == UnOpType::NOT){
    logical = 1;
    if (!node1->logical){
      this->node1 = new Op(this->node1, getConstType(node1->getType()), OpType::NE_OP);
    }
  }
}

Funcall :: Funcall() {
  rType = 0;
  logical = 0;
}

void Funcall :: print(){
  cout<<"("; 
  funName->print();
  cout<<" ";
  int l = vec.size();
  for (int i = 0; i < l; i++){
    vec[i]->print();
    cout<<" ";
  }
  cout<<")";
}

void Funcall :: setName(Identifier* _funName){
  funName = _funName;
}

void Funcall :: addExp(ExpAst* exp){
  vec.push_back(exp);
}


FloatConst :: FloatConst(float _val) {
  val = _val;
  rType = 2;
  logical = 0;
}

void FloatConst :: print(){
  cout<<"(FloatConst "<<val<<")";
}

float FloatConst :: getValue(){
  return val;
}

IntConst :: IntConst(int _val) {
  val = _val;
  rType = 2;
  logical = 0;
}

int IntConst :: getValue(){
  return val;
}

void IntConst :: print(){
  cout<<"(IntConst "<<val<<")";
}

StringConst :: StringConst(string _val) {
  val = _val;
  rType = 2;
  logical = 0;
}

string StringConst :: getValue(){
  return val;
}

void StringConst :: print(){
  cout<<"(StringConst "<<val<<")";
}

ArrayRef :: ArrayRef() {
}


Identifier :: Identifier(string _val) {
  val = _val;
  rType = 1;
  rec = 0;
  logical = 0;
}

string Identifier::getIdentifierName(){
  return val;
}

void Identifier::setRecord(GlRecord* _rec){
  rec = _rec;
}

void Identifier :: print(){
  cout<<"(Id \""<<val<<"\")";
}

void Identifier :: printFold(){
  this->print();
}

GlRecord* Identifier :: getRecord(){
  return rec;
}

Index :: Index(ArrayRef* node1, ExpAst* node2) {
  this->node1 = node1;
  this->node2 = node2;
  rType = 0;
  logical = 0;
}

void Index :: print(){
  cout<<"(ArrayRef ";
  this->printFold();
  cout<<")";
}

void Index :: printFold(){
  node1->printFold();
  cout<<" ";
  node2->print();
}


FuncallStmt::FuncallStmt(Funcall* node1){
  this->node1 = node1;
  setType(node1->getType());
}

void FuncallStmt::print(){
  node1->print();
}


ToFloat::ToFloat(ExpAst* node1){
  this->node1 = node1;
  Type *t = node1->getType();
  if (t->tag != Type::Error){
    setType(new Type(Type::Base, Type::Float));
  }
  else {
    setType(t);
  }
  
  rType = 0;
  logical = 0;
}

void ToFloat::print(){
  cout<<"(TO_FLOAT ";
  node1->print();
  cout<<")";
}


ToInt::ToInt(ExpAst* node1){
  this->node1 = node1;
  Type *t = node1->getType();
  if (t->tag != Type::Error){
    setType(new Type(Type::Base, Type::Int));
  }
  else {
    setType(t);
  }

  rType = 0;
  logical = 0;
}

void ToInt::print(){
  cout<<"(TO_INT ";
  node1->print();
  cout<<")";
}

//////////////////////////////////////////////////
/* global functions to help register management */
//////////////////////////////////////////////////


/* swaps the top two registers */
void swapTopReg(stack<Register*> &regStack){
  int l = regStack.size();
  if (l >= 2){
    Register* f = regStack.top();
    regStack.pop();
    Register* s = regStack.top();
    regStack.pop();
    regStack.push(f);
    regStack.push(s);
  }
}



////////////////////////////////////////////////////////////
/* the genCode functions for all the ast are defined here */
////////////////////////////////////////////////////////////


void FuncDef :: genCode(stack<Register*> &regStack){
  cout<<"Error function called "<<endl;
}

void FuncDef :: genCode(stack<Register*> &regStack, SymTab* symTab){
  // generate the code for the body
  codeStack.push_back(new HCode("void " + name + "()"));
  codeStack.push_back(new HCode("{"));
  
  if(!isMain){
    codeStack.push_back(new Instr("pushi", "ebp")); // set dynamic link
    codeStack.push_back(new Instr("move", "esp", "ebp")); // setting dyn link
  }
  
  map<int, GlType*> mp = symTab->getOrderedRecords();
  map<int, GlType*>::iterator itr = mp.begin();
  vector<pair<int, int> > trr;
  trr.push_back(pair<int, int>(0, 0));
  // 1 - int, 2 - float
  for(;itr != mp.end(); itr++){
    GlType* temp = itr->second;
    if (temp->type == VarType::BASIC){
      string fun = "push";
      if (((BasicType*)temp)->typeName == BasicVarType::INT){
	fun += "i";
	if (trr[trr.size() - 1].first == 1) trr[trr.size() - 1].second++;
	else trr.push_back(pair<int, int>(1, 1));
      }
      else if (((BasicType*)temp)->typeName == BasicVarType::FLOAT){
	fun += "f";
	if (trr[trr.size() - 1].first == 2) trr[trr.size() - 1].second++;
	else trr.push_back(pair<int, int>(2, 1));
      }
      codeStack.push_back(new Instr(fun, "0"));
    }
    else {
      BasicVarType t = ((ArrayType*)temp)->getBasicVarType();
      string fun = "push";
      int l = ((ArrayType*)temp)->calcDim(); 
      if (t == BasicVarType::INT){
	fun += "i";
	if (trr[trr.size() - 1].first == 1) trr[trr.size() - 1].second += l;
	else trr.push_back(pair<int, int>(1, l));
      }
      else if (t == BasicVarType::FLOAT){
	fun += "f";
	if (trr[trr.size() - 1].first == 2) trr[trr.size() - 1].second += l;
	else trr.push_back(pair<int, int>(2, l));
      }
      
      for(int i = 0; i < l; i++){
	codeStack.push_back(new Instr(fun, "0"));
      }
    }
  }
  
  node1->genCode(regStack);
  int nodeStart = nextInstr();
  
  int l1 = trr.size();
  for(int i = l1 - 1; i > 0; i--){
    if (trr[i].first == 1){
      codeStack.push_back(new Instr("popi", toString(trr[i].second)));
    }
    else {
      codeStack.push_back(new Instr("popf", toString(trr[i].second)));
    }
  }
  
  
  if (!isMain){
    codeStack.push_back(new Instr("loadi", "ind(esp)", "ebp"));  // storing dymanic link
    codeStack.push_back(new Instr("popi", "1"));  // pop stack
  }
  codeStack.push_back(new Instr("return"));
  (node1->nextList)->backpatch(getInstr(nodeStart));
  codeStack.push_back(new HCode("}"));
}


void Identifier::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  if (astnode_type->basetype == Type::Int){
    /* variable is of type Int */
    Register* top = regStack.top();
    string regName = top->getName();
    int offset = rec->offset;
    codeStack.push_back(new Instr("loadi", "ind(ebp, " + toString(offset) + ")", regName));
  }
  else if (astnode_type->basetype == Type::Float){
    /* variable is of type Float */
    Register* top = regStack.top();
    string regName = top->getName();
    int offset = rec->offset;
    codeStack.push_back(new Instr("loadf", "ind(ebp, " + toString(offset) + ")", regName));  
  }
  else if (astnode_type->basetype == Type::String){
    /* variable is of type string */
    // TODO
  }
  else {
    /* unknown case */
    cout<<"Not supported identifier type"<<endl;
  }
}

void BlockAst::genCode(stack<Register*> &regStack){
  /* call the gencode of the list of statements */
  int l = vec.size();
  if (l > 0){
    vec[0]->genCode(regStack);
  
    for (int i = 1; i < l; i++){
      int nodeStart = nextInstr();
      vec[i]->genCode(regStack);
      (vec[i - 1]->nextList)->backpatch(getInstr(nodeStart));
    }
    
    nextList = vec[l - 1]->nextList;
  }
}

void Ass::genCode(stack<Register*> &regStack){
  if (node1 != 0 && node2 != 0){
    Type* t = node1->getType();
    if (t->tag == Type::Base){
      if (t->basetype == Type::Int){
	int d1; IntConst d2(1);
	genCodeTemplate(d1, d2, regStack, "i");
      }
      else if (t->basetype == Type::Float){
	float d1; FloatConst d2(1.0);
	genCodeTemplate(d1, d2, regStack, "f");
      }
      else {
	// TODO, string, void
      }
    }
    else {
      // TODO pointer
    }
  }
}

template<class T, class R>
void Ass::genCodeTemplate(T d1, R d2, stack<Register*> &regStack, string type){
  int regCount = regStack.size();
  int lr = node1->getrType();
  int rr = node2->getrType();
  if (lr == 1){
    /* The case when identifier on LHS */
    if (rr == 2){
      /* RHS is a constant */
      T val = ((R*)node2)->getValue();
      VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
      int offset = rec->offset;
      codeStack.push_back(new Instr("store" + type, toString(val), "ind(ebp, " + toString(offset) + ")"));
      //cout<<"store"<<type<<"("<<val<<", ind(ebp, "<<offset<<"))"<<endl;
    }
    else {
      /* RHS is exp, calculated in Reg */
      node2->genCode(regStack);
      Register *reg = regStack.top();
      string regName = reg->getName();
      VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
      int offset = rec->offset;
      codeStack.push_back(new Instr("store" + type, regName, "ind(ebp, " + toString(offset) + ")"));
      //cout<<"store"<<type<<"("<<regName<<", ind(ebp, "<<offset<<"))"<<endl;
    }
  }
  else {
    /* case when array on LHS */

    if (rr == 2){
      /* RHS is a constant */
      T val = ((R*)node2)->getValue();
      ((Index*)node1)->genCodeLExp(regStack);
      Register* reg = regStack.top(); // reg contains the address which would be dereferenced
      string regName = reg->getName();
      codeStack.push_back(new Instr("store" + type, toString(val), "ind(" + regName + ")"));
      //cout<<"store"<<type<<"("<<val<<", ind("<<regName<<"))"<<endl;
    }
    else {
      /* RHS is exp, calculated in Reg */
      node2->genCode(regStack);
      Register *reg1 = regStack.top();
      string regName1 = reg1->getName();
      
      if (regCount == 2){
	/* Need to store the result calculated in this case */
	codeStack.push_back(new Instr("push" + type, regName1));
	//cout<<"push"<<type<<"("<<regName1<<")"<<endl;
	swapTopReg(regStack);             // SWAP
	((Index*)node1)->genCodeLExp(regStack);
	swapTopReg(regStack);             // SWAP
	reg1 = regStack.top();
	regName1 = reg1->getName();
	regStack.pop();                 // POP
	codeStack.push_back(new Instr("load" + type, "ind(esp) ", regName1));
	//cout<<"load"<<type<<"(ind(esp), "<<regName1<<")"<<endl;
	codeStack.push_back(new Instr("pop" + type, "1"));
	//cout<<"pop"<<type<<"(1)"<<endl;

	Register* reg2 = regStack.top();
	string regName2 = reg2->getName();
	
	// reg2 has the addr, reg1 has the value
	codeStack.push_back(new Instr("store" + type, regName1, "ind(" + regName2 + ")"));
	//cout<<"store"<<type<<"("<<regName1<<", ind("<<regName2<<"))"<<endl;
	regStack.push(reg1);              // PUSH
      }
      else {
	/* Store not needed as sufficent registers */
	regStack.pop();                 // POP
	
	if (type == "i") pushUsedReg(reg1, 1);   // saving the register with type of data
	else pushUsedReg(reg1, 2);

	((Index*)node1)->genCodeLExp(regStack);
	Register* reg2 = regStack.top();
	string regName2 = reg2->getName();
	
	// reg2 has the addr, reg1 has the value
	codeStack.push_back(new Instr("store" + type, regName1, "ind(" + regName2 + ")"));
	//cout<<"store"<<type<<"("<<regName1<<", ind("<<regName2<<"))"<<endl;
	regStack.push(reg1);            // PUSH
	popUsedReg();  // poping the saved register
      }
    }
  }
}

void While::genCode(stack<Register*> &regStack){
  node1->fall = 1;
  int node1Start = nextInstr();
  node1->genCode(regStack);
  int node2Start = nextInstr();
  node2->genCode(regStack);
  (node1->trueList)->backpatch(getInstr(node2Start));
  (node2->nextList)->backpatch(getInstr(node1Start));
  nextList = node1->falseList;
  GotoInstr* code = new GotoInstr("j");
  codeStack.push_back(code);
  code->backpatch(getInstr(node1Start));
}

void For::genCode(stack<Register*> &regStack){
  node2->fall = 1;
  node1->genCode(regStack);
  int node2Start = nextInstr();
  node2->genCode(regStack);
  int node4Start = nextInstr();
  node4->genCode(regStack);
  int node3Start = nextInstr();
  node3->genCode(regStack);
  GotoInstr* code2 = new GotoInstr("j");
  codeStack.push_back(code2);
  (node2->trueList)->backpatch(getInstr(node4Start));
  (node4->nextList)->backpatch(getInstr(node3Start));
  code2->backpatch(getInstr(node2Start));
  nextList = node2->falseList;
}

void Return::genCode(stack<Register*> &regStack){
  // put the return value from exp onto top stack
  if (astnode_type->basetype == Type::Int){
    int d1; IntConst d2(1); 
    genCode(d1, d2, regStack, "i");
  }
  else if (astnode_type->basetype == Type::Float){
    float d1; FloatConst d2(1); 
    genCode(d1, d2, regStack, "f");
  }
  else {
    // TODO, other ret types
  }
}

template<class T, class R>
void Return::genCode(T d1, R d2, stack<Register*> & regStack, string type){
  if (isType(node1, &d2)){
    // constant case
    T val = ((R*)node1)->getValue();
    codeStack.push_back(new Instr("store" + type, toString(val), "ind(ebp, "  + toString(retOffset) + ")"));
  }
  else {
    node1->genCode(regStack);
    Register* reg = regStack.top();
    string regName = reg->getName();
    codeStack.push_back(new Instr("store" + type, regName, "ind(ebp, "  + toString(retOffset) + ")"));
  }
}

void If::genCode(stack<Register*> &regStack){
  node1->fall = 1;               // expression.fall = 1
  node1->genCode(regStack);      
  int node2Start = nextInstr();
  node2->genCode(regStack);
  GotoInstr* code = new GotoInstr("j");
  codeStack.push_back(code);
  nextList->add(code);
  int node3Start = nextInstr();
  node3->genCode(regStack);
  (node1->trueList)->backpatch(getInstr(node2Start));
  (node1->falseList)->backpatch(getInstr(node3Start));
  nextList->merge(node2->nextList);
  nextList->merge(node3->nextList);
}

void Op::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  string opr = op_value[op];
  
  if (opr == "Plus" && astnode_type->basetype == Type::String){
    // TODO for strings
  }
  else if (opr == "Plus" || opr == "Minus" || opr == "Mult" || opr == "Div" || opr == "Assign_exp" || opr == "LT" || opr == "GT" || opr == "LE_OP" || opr == "GE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
    Type* t = node1->getType();
    if (t->basetype == Type::Int){
      int d1;
      IntConst d2(1);
      string type = "i";
      genCodeTemplate(d1, d2, type, regStack, opr);
    }
    else if (t->basetype == Type::Float){
      float d1;
      FloatConst d2(1.0);
      string type = "f";
      genCodeTemplate(d1, d2, type, regStack, opr);
    }
    else {
      // TODO
      cout<<"Type not supported"<<endl;
    }
  }  
  else if (opr == "OR_OP"){
    if(arith){
      node1->fall = 1;
      node1->genCode(regStack);
      Register* reg = regStack.top();
      string regName = reg->getName();
      codeStack.push_back(new Instr("cmpi", "1", regName));
      GotoInstr* gt1 = new GotoInstr("je");
      codeStack.push_back(gt1);
      node2->fall = 1;
      node2->genCode(regStack);
      int nodeStart = nextInstr();
      codeStack.push_back(new LCode());
      gt1->backpatch(getInstr(nodeStart));
    }
    else{
      node1->setArith();
      node2->setArith();
      node1->fall = false;
      node2->fall = fall;
      node1->genCode(regStack);
      int node2Start = nextInstr();
      node2->genCode(regStack);
      (node1->falseList)->backpatch(getInstr(node2Start));
      trueList->merge(node1->trueList);
      trueList->merge(node2->trueList);
      falseList = node2->falseList;
    }
  }
  else if (opr == "AND_OP"){
    if (arith){
      node1->fall = 1;
      node1->genCode(regStack);
      Register* reg = regStack.top();
      string regName = reg->getName();
      codeStack.push_back(new Instr("cmpi", "0", regName));
      GotoInstr* gt1 = new GotoInstr("je");
      codeStack.push_back(gt1);
      node2->fall = 1;
      node2->genCode(regStack);
      int nodeStart = nextInstr();
      codeStack.push_back(new LCode());
      gt1->backpatch(getInstr(nodeStart));
    }
    else{
      node1->setArith();
      node2->setArith();
      node1->fall = true;
      node2->fall = fall;
      node1->genCode(regStack);
      int node2Start = nextInstr();
      node2->genCode(regStack);
      (node1->trueList)->backpatch(getInstr(node2Start));
      falseList->merge(node1->falseList);
      falseList->merge(node2->falseList);
      trueList = node2->trueList;
    }
  }
  else{//TODO
  }
}

void UnOp::genCode(stack<Register*> &regStack){
  if (op == UnOpType::NOT){
    if (arith){
      node1->fall = 0;
      node1->genCode(regStack);
    }
    else{
      node1->setArith();
      node1->fall = !fall;
      node1->genCode(regStack);
      trueList = node1->falseList;
      falseList = node1->trueList;
    }
  }
  else if (op == UnOpType::UMINUS || op == UnOpType::PP){
    if (astnode_type->basetype == Type::Int){
      int d1; IntConst d2(1); string type = "i";
      genCode(d1, d2, regStack, type);
    }
    else if (astnode_type->basetype == Type::Float){
      float d1; FloatConst d2(1); string type = "f";
      genCode(d1, d2, regStack, type);
    }
    else {
      cout<<"Unkown Type :: ";
      astnode_type->printType();
    }
  }
  else {
    cout<<"Error Operator"<<endl;
  }
}

/* Template genCode supporting the code generation 
   for Int, Float class Types , operators = ++, - */

template<class T, class R>
void UnOp::genCode(T d1, R d2, stack<Register*> &regStack, string type){
  if (isType(node1, &d2)){   // checks for constants , R is IntConst / Float Const
    if (op == UnOpType::UMINUS){
      T val = ((R*)node1)->getValue();
      val = -val;
      Register* reg = regStack.top();
      string regName = reg->getName();
      codeStack.push_back(new Instr("move", toString(val), regName));
    }
  }
  else {
    if (op == UnOpType::UMINUS){
      node1->genCode(regStack);
      Register* reg = regStack.top();
      string regName = reg->getName();
      
      codeStack.push_back(new Instr("mul" + type, "-1", regName));
    }
    else if (op == UnOpType::PP){
      
      int regCount = regStack.size();
      int lr = node1->getrType();
      if (lr == 1){
	// the val + 1 is calculated in exp
	node1->genCode(regStack);
	Register *reg = regStack.top();
	string regName = reg->getName();
	codeStack.push_back(new Instr("add" + type, "1", regName));
	VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
	int offset = rec->offset;
	codeStack.push_back(new Instr("store" + type, regName, "ind(ebp, " + toString(offset) + ")"));
      }
      else {
	/* case when array on LHS */
	node1->genCode(regStack);
	Register *reg1 = regStack.top();
	string regName1 = reg1->getName();
	codeStack.push_back(new Instr("add" + type, "1", regName1));      
	if (regCount == 2){
	  /* Need to store the result calculated in this case */
	  codeStack.push_back(new Instr("push" + type, regName1));
	  //cout<<"push"<<type<<"("<<regName1<<")"<<endl;
	  swapTopReg(regStack);             // SWAP
	  ((Index*)node1)->genCodeLExp(regStack);
	  swapTopReg(regStack);             // SWAP
	  reg1 = regStack.top();
	  regName1 = reg1->getName();
	  regStack.pop();                 // POP
	  codeStack.push_back(new Instr("load" + type, "ind(esp) ", regName1));
	  codeStack.push_back(new Instr("pop" + type, "1"));
	  
	  Register* reg2 = regStack.top();
	  string regName2 = reg2->getName();
	  
	  // reg2 has the addr, reg1 has the value
	  codeStack.push_back(new Instr("store" + type, regName1, "ind(" + regName2 + ")"));
	  regStack.push(reg1);              // PUSH
	}
	else {
	  /* Store not needed as sufficent registers */
	  regStack.pop();                 // POP
	  
	  if (type == "i") pushUsedReg(reg1, 1);   // saving the register with type of data
	  else pushUsedReg(reg1, 2);
	  
	  ((Index*)node1)->genCodeLExp(regStack);
	  Register* reg2 = regStack.top();
	  string regName2 = reg2->getName();
	  
	  // reg2 has the addr, reg1 has the value
	  codeStack.push_back(new Instr("store" + type, regName1, "ind(" + regName2 + ")"));
	  regStack.push(reg1);            // PUSH
	  popUsedReg();  // poping the saved register
	}
      }
    }
    
  }
  
}

void Funcall::genCode(stack<Register*> &regStack){
  saveUsedReg();
  string func = funName->getIdentifierName();
  if (func == "printf"){
    /* special function to take care of */
    codeStack.push_back(new Instr("pushi", "0"));  // return value
    int l = vec.size(); 
    for (int i = 0; i < l; i++){
      Type *t = vec[i]->getType();
      if (t->tag == Type::Base){
	if (t->basetype == Type :: Int){
	  int d1; IntConst d2(1);
	  genCode(vec[i], d1, d2, regStack, "int");
	}
	else if (t->basetype == Type :: Float){
	  float d1; FloatConst d2(1);
	  genCode(vec[i], d1, d2, regStack, "float");
	}
	else if (t->basetype == Type :: String){
	  string d1; StringConst d2("1");
	  genCode(vec[i], d1, d2, regStack, "string");
	}
	else {
	  // TODO voids
	}
      }
      else {
	// TODO, handling pointers
      }
    }
    
    // loading the return value and restoring the stack
    Register* reg = regStack.top();
    string regName = reg->getName();
    codeStack.push_back(new Instr("loadi", "ind(esp)", regName));
    codeStack.push_back(new Instr("popi", "1"));
  }
  else {
    /* User defined functions */
    // Making space for return value
    if (astnode_type->basetype == Type::Int){
      codeStack.push_back(new Instr("pushi", "0"));
    }
    else if (astnode_type->basetype == Type::Float){
      codeStack.push_back(new Instr("pushf", "0"));
    }
    else {
      // TODO for Void
    }
    
    // pusing of parameters on the esp stack
    int l = vec.size();
    // Pushing from right to left
    int fcount = 0, icount = 0;

    /* stores the pushed back types
       in order so as to used pop*(int) */
    vector<pair<int, int> > trr;   
    trr.push_back(pair<int, int>(0, 0));

    for (int i = l - 1; i >= 0; i--){ 
      Type* t = vec[i]->getType();
      int r = vec[i]->getrType();
      if (t->basetype == Type::Int){
	icount++;

	// storing count for the type of exp in sequence
	if (trr[trr.size() - 1].first == 1) trr[trr.size() - 1].second++; 
	else trr.push_back(pair<int, int>(1, 1));

	if (r == 2){
	  int val = ((IntConst*)vec[i])->getValue();
	  codeStack.push_back(new Instr("pushi", toString(val)));
	}
	else {
	  // code for finding the expression value in top reg
	  vec[i]->genCode(regStack); 
	  Register* reg = regStack.top();
	  string regName = reg->getName();
	  codeStack.push_back(new Instr("pushi", regName));
	}
      }
      else if (t->basetype == Type::Float){
	fcount++;

	// storing count for the type of exp in sequence
	if (trr[trr.size() - 1].first == 2) trr[trr.size() - 1].second++; 
	else trr.push_back(pair<int, int>(2, 1));
	
	if (r == 2){
	  float val = ((FloatConst*)vec[i])->getValue();
	  codeStack.push_back(new Instr("pushf", toString(val)));
	}
	else {
	  // code for finding the expression value in top reg
	  vec[i]->genCode(regStack); 
	  Register* reg = regStack.top();
	  string regName = reg->getName();
	  codeStack.push_back(new Instr("pushf", regName));
	}
      }
      else {
	// TODO for Void 
      }
    }
    
    // call to the function
    codeStack.push_back(new Instr(func, ""));
    
    // // pop out parameters from the stack
    // if (icount)
    //   codeStack.push_back(new Instr("popi", toString(icount)));
    // if (fcount)
    //   codeStack.push_back(new Instr("popf", toString(fcount)));

    int l1 = trr.size();
    for(int i = l1 - 1; i > 0; i--){
      if (trr[i].first == 1) codeStack.push_back(new Instr("popi", toString(trr[i].second)));
      else codeStack.push_back(new Instr("popf", toString(trr[i].second)));
    }
    
    Register* reg = regStack.top();
    string regName = reg->getName();

    /* Get the return value in top reg and clean up return space */
    if (astnode_type->basetype == Type::Int){
      codeStack.push_back(new Instr("loadi", "ind(esp)", regName));
      codeStack.push_back(new Instr("popi", "1"));
    }
    else if (astnode_type->basetype == Type::Float){
      codeStack.push_back(new Instr("loadf", "ind(esp)", regName));
      codeStack.push_back(new Instr("popf", "1"));
    }
    else {
      // TODO for Void
    }
    
  }
  loadUsedReg();
}

template<class T, class R>
void Funcall::genCode(ExpAst* obj, T d1, R d2, stack<Register*>& regStack, string type){
  if (isType(obj, &d2)){
    T val = ((R*)obj)->getValue();
    codeStack.push_back(new Instr("print_" + type, toString(val)));
  }
  else {
    obj->genCode(regStack);
    Register* reg = regStack.top();
    string regName = reg->getName();
    codeStack.push_back(new Instr("print_" + type, regName));
  }
}

void FloatConst::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  Register *top = regStack.top();
  string regName = top->getName();
  float val = getValue();
  codeStack.push_back(new Instr("move", toString(val*1.0), regName));
  //cout<<"loadf("<<val<<", "<<regName<<")"<<endl;
}

void IntConst::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  Register *top = regStack.top();
  string regName = top->getName();
  int val = getValue();
  codeStack.push_back(new Instr("move", toString(val), regName));
  //cout<<"loadi("<<val<<", "<<regName<<")"<<endl;
  
}

void StringConst::genCode(stack<Register*> &regStack){
  // TODO
}


/* This special function is to generate the code 
   needed for Index when this is used as lexpression */

void Index::genCodeLExp(stack<Register*> &regStack){
  int regCount = regStack.size();
  genCodeInternal(regStack);
  Register* reg = regStack.top();
  string regName = reg->getName();
  codeStack.push_back(new Instr("addi", "ebp", regName));
}


void Index::genCode(stack<Register*> &regStack){
  int regCount = regStack.size();
  genCodeInternal(regStack);
  Register* reg = regStack.top();
  string regName = reg->getName();
  codeStack.push_back(new Instr("addi", "ebp", regName));
  if (astnode_type->getBasetype() == Type::Int){
    codeStack.push_back(new Instr("loadi", "ind(" + regName + ")", regName));
  }
  else if (astnode_type->getBasetype() == Type::Int){
    codeStack.push_back(new Instr("loadf", "ind(" + regName + ")", regName));
  }
  else {
    // TODO, unknown
  }
}


/* Special Internal function,
   helps to generate array codes in gcc convention */

void Index::genCodeInternal(stack<Register*> &regStack){
  int regCount = regStack.size();
  int rr = node2->getrType();
  int lr = node1->getrType();
  
  if (regCount > 2 || (regCount == 2 && rr > 0)){
    /* First we calculate the left node and then right
       in case left is not an identifier
       following the gcc convention ,
       The regCount = 2 case handled in this as right tree
       is either an idenifier / constant, so needs 1 register */

    if (lr == 1){
      /* The left node is an Identifier 
	 so we evaluate the right expression first */
      swapTopReg(regStack);               // SWAP
      node2->genCode(regStack);   // right exp 
      int factor = astnode_type->calcSize();
      Register* reg2 = regStack.top();
      string regName2 = reg2->getName();
      codeStack.push_back(new Instr("muli", toString(-factor), regName2));  // negating the factor
      swapTopReg(regStack);               // SWAP, top restored

      // obtaining the dimension in regName
      VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
      int offset = rec->offset;
      Register *reg = regStack.top();
      string regName = reg->getName();
      codeStack.push_back(new Instr("move", toString(offset), regName));
      
      // adding the offset to the dimension in regName
      codeStack.push_back(new Instr("addi", regName2, regName));
    }
    else {
      /* In this case we evaulate the LHS first */
      ((Index*)node1)->genCodeInternal(regStack);
      Register* reg1 = regStack.top();
      regStack.pop();

      pushUsedReg(reg1, 1);   // saving the register with type of data

      node2->genCode(regStack);
      Register* reg2 = regStack.top();

      string regName1 = reg1->getName();
      string regName2 = reg2->getName();
      
      int factor = astnode_type->calcSize();
      codeStack.push_back(new Instr("muli", toString(-factor), regName2)); // negating the factor
      codeStack.push_back(new Instr("addi", regName2, regName1));
      regStack.push(reg1);  // restore the stack
      popUsedReg();   // poping the saved register
    }
  }
  else if (regCount == 2 && rr == 0){
    /* We have only 2 registers remaining,
       RHS needs > 1 register*/
    
    if (lr == 1){
      /* The left node is an Identifier 
	 so we evaluate the right expression first */
      swapTopReg(regStack);               // SWAP
      node2->genCode(regStack);   // right exp 
      int factor = astnode_type->calcSize();
      Register* reg2 = regStack.top();
      string regName2 = reg2->getName();
      codeStack.push_back(new Instr("muli", toString(-factor), regName2));  // negating factor
      swapTopReg(regStack);               // SWAP, top restored

      // obtaining the dimension in regName
      VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
      int offset = rec->offset;
      Register *reg = regStack.top();
      string regName = reg->getName();
      codeStack.push_back(new Instr("move", toString(offset), regName));
      
      // adding the offset to the dimension in regName
      codeStack.push_back(new Instr("addi", regName2, regName));
    }
    else {
      /* In this case we evaulate the LHS first */
      ((Index*)node1)->genCodeInternal(regStack);
      Register* reg1 = regStack.top();
      // Saving the register value
      string regName1 = reg1->getName();
      codeStack.push_back(new Instr("pushi", regName1));
      swapTopReg(regStack);   // SWAP
      
      node2->genCode(regStack);
      Register* reg2 = regStack.top();
      string regName2 = reg2->getName();
      swapTopReg(regStack);
      reg1 = regStack.top();
      regStack.pop();               // POP
      regName1 = reg1->getName();
      codeStack.push_back(new Instr("loadi", "ind(esp)", regName1));
      codeStack.push_back(new Instr("popi", "1"));

      int factor = astnode_type->calcSize();
      codeStack.push_back(new Instr("muli", toString(-factor), regName2)); // negating factor
      codeStack.push_back(new Instr("addi", regName2, regName1));
      regStack.push(reg1);  // restore the stack
      
    }
  }
  else {
    cout<<"Too low registers"<<endl;
  }
}

void FuncallStmt::genCode(stack<Register*> &regStack){
  node1->genCode(regStack);
}

void ToFloat::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  node1->genCode(regStack);   // constant loads itself on top Reg
  Register* top = regStack.top();
  string regName = top->getName();
  codeStack.push_back(new Instr("intToFloat", regName));
  //  cout<<"intTofloat("<<regName<<")"<<endl;
}

void ToInt::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  node1->genCode(regStack);   // constant loads itself on top Reg
  Register* top = regStack.top();
  string regName = top->getName();
  codeStack.push_back(new Instr("floatToInt", regName));
  //  cout<<"floatToint("<<regName<<")"<<endl;
}


/* Helper function to create the goto instr related stuff */

void genCodeHelper(CList* list, int fall, int arith, string regName, string opr){
  if (fall){
    if (arith){ 
      codeStack.push_back(new Instr("move", "0", regName));
      GotoInstr *code = new GotoInstr(fallInstr(opr));
      codeStack.push_back(code);
      codeStack.push_back(new Instr("move", "1", regName));
      list->add(code);
      int nodeStart = nextInstr();
      codeStack.push_back(new LCode());
      list->backpatch(getInstr(nodeStart));
    }
    else {
      GotoInstr *code = new GotoInstr(fallInstr(opr));
      codeStack.push_back(code);
      list->add(code);
    }
  }
  else{
    if (arith){ 
      codeStack.push_back(new Instr("move", "1", regName));
      GotoInstr *code = new GotoInstr(fallInstr(opr));
      codeStack.push_back(code);
      codeStack.push_back(new Instr("move", "0", regName));
      list->add(code);
      int nodeStart = nextInstr();
      codeStack.push_back(new LCode());
      list->backpatch(getInstr(nodeStart));
    }
    else{
      GotoInstr *code = new GotoInstr(notFallInstr(opr));
      codeStack.push_back(code);
      list->add(code);
    }
  }
}

/* This is a template class based function 
   supporting correctness for Int, Float
   for operators - Plus, Minus, Div, mult 
   T -> int / float
   Rtype -> IntConst / FloatConst  */
   
template<class T, class Rtype>
void Op::genCodeTemplate(T d1, Rtype d2, string type, stack<Register*> &regStack, string opr){
  int countReg = regStack.size();
  int lr = node1->getrType();
  int rr = node2->getrType();
  if (opr == "Plus" || opr == "Minus" || opr == "Div" || opr == "Mult" || opr == "LT" || opr == "GT" || opr == "LE_OP" || opr == "GE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
    // The children need to be arithmetic operations for these operators
    if (lr > 1 || rr > 1){

      // Case when one of subexpression is constant
      if (lr > 1 && rr > 1){
	// both subtree are constants
	T lval = ((Rtype*)node1)->getValue(), rval = ((Rtype*)node2)->getValue();	  
	Register* top = regStack.top();
	string regName = top->getName();
      
	if (opr == "Plus"){
	  codeStack.push_back(new Instr("move", toString(rval), regName));
	  codeStack.push_back(new Instr("add" + type, toString(lval), regName));
	}
	else if (opr == "Minus"){
	  rval = -1;                                               // RHS = -RHS
	  codeStack.push_back(new Instr("move", toString(lval), regName));
	  codeStack.push_back(new Instr("add" + type, toString(rval), regName));
	}
	else if (opr == "Mult"){
	  codeStack.push_back(new Instr("move", toString(rval), regName));
	  codeStack.push_back(new Instr("mul" + type, toString(lval), regName));
	}
	else if (opr == "Div"){
	  codeStack.push_back(new Instr(toString(rval), regName));
	  codeStack.push_back(new Instr("div" + type, toString(lval), regName));
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  codeStack.push_back(new Instr("move", toString(rval), regName));
	  codeStack.push_back(new Instr("cmp" + type, toString(lval), regName));
	  if (fall){
	    genCodeHelper(falseList, fall, arith, regName, opr);
	    // GotoInstr *code = new GotoInstr(fallInstr(opr));
	    // codeStack.push_back(code);
	    // falseList->add(code);
	  }
	  else{
	    genCodeHelper(trueList, fall, arith, regName, opr);
	    // GotoInstr *code = new GotoInstr(notFallInstr(opr));
	    // codeStack.push_back(code);
	    // trueList->add(code);
	  }
	}
	else {
	  cout<<"Not suppored for this operator"<<endl;
	}

      }
      else if (lr > 1){
	// left is constant

	node2->genCode(regStack);  // gencode for right
	T lval = ((Rtype*)node1)->getValue();
	Register* top = regStack.top();
	string regName = top->getName();

	if (opr == "Plus"){
	  codeStack.push_back(new Instr("add" + type, toString(lval), regName));
	}
	else if (opr == "Minus"){
	  codeStack.push_back(new Instr("mul" + type, "-1", regName));
	  codeStack.push_back(new Instr("add" + type, toString(lval), regName));
	}
	else if (opr == "Mult"){
	  codeStack.push_back(new Instr("mul" + type, toString(lval), regName));
	}
	else if (opr == "Div"){
	  codeStack.push_back(new Instr("div" + type, toString(lval), regName));
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  codeStack.push_back(new Instr("cmp" + type, toString(lval), regName));
	  //	  cout<<"cmp"<<type<<"("<<lval<<","<<regName<<")"<<endl;
	  if (fall){
	    genCodeHelper(falseList, fall, arith, regName, opr);
	    // GotoInstr *code = new GotoInstr(fallInstr(opr));
	    // codeStack.push_back(code);
	    // falseList->add(code);
	  }
	  else{
	    genCodeHelper(trueList, fall, arith, regName, opr);
	    // GotoInstr *code = new GotoInstr(notFallInstr(opr));
	    // codeStack.push_back(code);
	    // trueList->add(code);
	  }
	}
	else {
	  cout<<"operation not supported"<<endl;
	}

      }
      else {
	// right is constant
	node1->genCode(regStack);  // gencode for left
	T rval = ((Rtype*)node2)->getValue();
	Register* top = regStack.top();
	string regName = top->getName();

	if (opr == "Plus"){
	  codeStack.push_back(new Instr("add" + type, toString(rval), regName));
	}
	else if (opr == "Minus"){
	  rval = -rval;                                               // RHS = -RHS
	  codeStack.push_back(new Instr("add" + type, toString(rval), regName));
	}
	else if (opr == "Mult"){
	  codeStack.push_back(new Instr("mul" + type, toString(rval), regName));
	}
	else if (opr == "Div"){
	  /* Need to load RHS into register */
	  regStack.pop();
	  Register* top2 = regStack.top();
	  string regName2 = top2->getName();
	  codeStack.push_back(new Instr("move", toString(rval), regName2));
	  codeStack.push_back(new Instr("div" + type, regName, regName2));
	  regStack.push(top);                                          // restore the pop
	  swapTopReg(regStack);
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  /* Need to load RHS into register */
	  regStack.pop();
	  Register* top2 = regStack.top();
	  string regName2 = top2->getName();
	  codeStack.push_back(new Instr("move", toString(rval), regName2));
	  //	  cout<<"move("<<rval<<", "<<regName2<<")"<<endl;              // Load RHS into reg
	  codeStack.push_back(new Instr("cmp" + type, regName, regName2));
	  //	  cout<<"cmp"<<type<<"("<<regName<<","<<regName2<<")"<<endl;
	  if (fall){
	    genCodeHelper(falseList, fall, arith, regName, opr);
	    // GotoInstr *code = new GotoInstr(fallInstr(opr));
	    // codeStack.push_back(code);
	    // falseList->add(code);
	  }
	  else{
	    genCodeHelper(trueList, fall, arith, regName, opr);
	    // GotoInstr *code = new GotoInstr(notFallInstr(opr));
	    // codeStack.push_back(code);
	    // trueList->add(code);
	  }
	  regStack.push(top);                                          // restore the pop
	}
	else {
	  cout<<"operation not supported"<<endl;
	}

      }
    }
    else {

      /* When none of subtree is constant */
      if (countReg > 2){

	// Storeless add possible
	swapTopReg(regStack);
	node2->genCode(regStack);  // Evaluating the right subtree

	Register* top2 = regStack.top();
	regStack.pop();
	
	if (type == "i") pushUsedReg(top2, 1);
	else pushUsedReg(top2, 2);
	
	node1->genCode(regStack);

	Register* top1 = regStack.top();
	string regName1 = top1->getName();
	string regName2 = top2->getName();

	if (opr == "Plus"){
	  codeStack.push_back(new Instr("add" + type, regName2, regName1));
	}
	else if (opr == "Minus"){
	  codeStack.push_back(new Instr("mul" + type, "-1", regName2));
	  codeStack.push_back(new Instr("add" + type, regName2, regName1));
	}
	else if (opr == "Mult"){
	  codeStack.push_back(new Instr("mul" + type, regName2, regName1));
	}
	else if (opr == "Div"){
	  codeStack.push_back(new Instr("div" + type, regName1, regName2));	  
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  codeStack.push_back(new Instr("cmp" + type, regName1, regName2));
	  if (fall){
	    genCodeHelper(falseList, fall, arith, regName1, opr);
	    // GotoInstr *code = new GotoInstr(fallInstr(opr));
	    // codeStack.push_back(code);
	    // falseList->add(code);
	  }
	  else{
	    genCodeHelper(trueList, fall, arith, regName1, opr);
	    // GotoInstr *code = new GotoInstr(notFallInstr(opr));
	    // codeStack.push_back(code);
	    // trueList->add(code);
	  }
	}
	else {
	  cout<<"Operation not supported"<<endl;
	}

	regStack.push(top2);
	popUsedReg();
	if (opr != "Div")
	  swapTopReg(regStack); // top1 contanis the evaulated expression
      }
      else {

	// only 2 reg left, need for a store in this case
	node2->genCode(regStack);

	Register* top1 = regStack.top();
	string regName1 = top1->getName();
	codeStack.push_back(new Instr("push" + type, regName1));

	node1->genCode(regStack);	  

	swapTopReg(regStack);

	Register* top2 = regStack.top();
	string regName2 = top2->getName();
	codeStack.push_back(new Instr("load" + type, "ind(esp)", regName2));
	codeStack.push_back(new Instr("pop" + type, "1"));

	regStack.pop();

	top1 = regStack.top();
	regName1 = top1->getName();

	if (opr == "Plus"){
	  codeStack.push_back(new Instr("add" + type, regName2, regName1));
	}
	else if (opr == "Minus"){
	  codeStack.push_back(new Instr("mul" + type, "-1", regName2));
	  codeStack.push_back(new Instr("add" + type, regName2, regName1));
	}
	else if (opr == "Mult"){
	  codeStack.push_back(new Instr("mul" + type, regName2, regName1));
	}
	else if (opr == "Div"){
	  codeStack.push_back(new Instr("div" + type, regName1, regName2));
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  codeStack.push_back(new Instr("cmp" + type, regName1, regName2));
	  //	  cout<<"cmp"<<type<<"("<<regName1<<","<<regName2<<")"<<endl;
	  if (fall){
	    genCodeHelper(falseList, fall, arith, regName1, opr);
	    // GotoInstr *code = new GotoInstr(fallInstr(opr));
	    // codeStack.push_back(code);
	    // falseList->add(code);
	  }
	  else{
	    genCodeHelper(trueList, fall, arith, regName1, opr);
	    // GotoInstr *code = new GotoInstr(notFallInstr(opr));
	    // codeStack.push_back(code);
	    // trueList->add(code);
	  }
	}
	else {
	  cout<<"Operation not supported"<<endl;
	}

	regStack.push(top2);   // store the reg back
	if (opr != "Div")
	  swapTopReg(regStack);   // restore regName1 to top
      }
    }
  }
  /*----------------------------------------------Assign_Exp---------------------------------------------------*/
  else if (opr == "Assign_exp"){

    if (lr == 1){
      /* The case when identifier on LHS */
      if (rr == 2){
	/* RHS is a constant */
	T val = ((Rtype*)node2)->getValue();
	VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
	int offset = rec->offset;
	Register* reg = regStack.top();
	string regName = reg->getName();
	codeStack.push_back(new Instr("move", toString(val), regName));
	//	cout<<"move"<<"("<<val<<", "<<regName<<")"<<endl;   // top reg contains the value
	codeStack.push_back(new Instr("store" + type, toString(val), "ind(ebp, " + toString(offset) + ")"));
	//	cout<<"store"<<type<<"("<<val<<", ind(ebp, "<<offset<<"))"<<endl;
      }
      else {
	/* RHS is exp, calculated in Reg */
	node2->genCode(regStack);
	Register *reg = regStack.top();   // top reg contains the value
	string regName = reg->getName();
	VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
	int offset = rec->offset;
	codeStack.push_back(new Instr("store" + type, regName, "ind(ebp, " + toString(offset) + ")"));
	//	cout<<"store"<<type<<"("<<regName<<", ind(ebp, "<<offset<<"))"<<endl;
      }
    }
    else {
      /* case when array on LHS */

      if (rr == 2){
	/* RHS is a constant */
	T val = ((Rtype*)node2)->getValue();
	((Index*)node1)->genCodeLExp(regStack);
	Register* reg = regStack.top(); // reg contains the address which would be dereferenced
	string regName = reg->getName();
	codeStack.push_back(new Instr("store" + type, toString(val), "ind(" + regName + ")"));
	//	cout<<"store"<<type<<"("<<val<<", ind("<<regName<<"))"<<endl;
	codeStack.push_back(new Instr("move", toString(val), regName));
	//	cout<<"move("<<val<<", "<<regName<<")"<<endl;
      }
      else {
	/* RHS is exp, calculated in Reg */
	node2->genCode(regStack);
	Register *reg1 = regStack.top();
	string regName1 = reg1->getName();
      
	if (countReg == 2){
	  /* Need to store the result calculated in this case */
	  codeStack.push_back(new Instr("push" + type, regName1));
	  //	  cout<<"push"<<type<<"("<<regName1<<")"<<endl;
	  swapTopReg(regStack);             // SWAP
	  ((Index*)node1)->genCodeLExp(regStack);
	  swapTopReg(regStack);             // SWAP
	  reg1 = regStack.top();
	  regName1 = reg1->getName();
	  regStack.pop();                 // POP
	  codeStack.push_back(new Instr("load" + type, "ind(esp)", regName1));
	  //	  cout<<"load"<<type<<"(ind(esp), "<<regName1<<")"<<endl;
	  codeStack.push_back(new Instr("pop", "1"));
	  //	  cout<<"pop"<<type<<"(1)"<<endl;

	  Register* reg2 = regStack.top();
	  string regName2 = reg2->getName();
	
	  // reg2 has the addr, reg1 has the value
	  codeStack.push_back(new Instr("store" + type, regName1, "ind(" + regName2 + ")"));
	  //	  cout<<"store"<<type<<"("<<regName1<<", ind("<<regName2<<"))"<<endl;
	  regStack.push(reg1);              // PUSH
	}
	else {
	  /* Store not needed as sufficent registers */
	  regStack.pop();                 // POP

	  if (type == "i") pushUsedReg(reg1, 1);
	  else pushUsedReg(reg1, 2);

	  ((Index*)node1)->genCodeLExp(regStack);
	  Register* reg2 = regStack.top();
	  string regName2 = reg2->getName();
	
	  // reg2 has the addr, reg1 has the value
	  codeStack.push_back(new Instr("store" + type, regName2, "ind(" + regName2 + ")"));
	  //	  cout<<"store"<<type<<"("<<regName1<<", ind("<<regName2<<"))"<<endl;
	  regStack.push(reg1);            // PUSH
	  popUsedReg();
	}
      }
    }

  }
  else {
    
  }
}
