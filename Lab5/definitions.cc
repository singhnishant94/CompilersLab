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
  cout<<"\t"<<func<<"("<<arg1<<")"<<endl;
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
  if (argCount == 1)
    cout<<"\t"<<func<<"("<<arg1<<")"<<endl;  
  else   cout<<"\t"<<func<<"("<<arg1<<","<<arg2<<")"<<endl;  
}

/* Definitions for the AST's */

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
  fall = 0;
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
}

void Return :: print(){
  cout<<"(Return ";
  node1->print();
  cout<<")";
}

If :: If(ExpAst* node1, StmtAst* node2, StmtAst* node3){
  this->node1 = node1;
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
  
  
  string opr = op_value[op];
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
  
}

UnOp :: UnOp(UnOpType _op){
  op = _op;
  
}


void UnOp :: print(){
  cout<<"("<<unop_value[op]<<" "; 
  node1->print();
  cout<<")";
}

void UnOp::setExp(ExpAst* node1){
  this->node1 = node1;
  // check for lexp here for PP
}

Funcall :: Funcall() {
  rType = 0;
  
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


void Identifier::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  if (astnode_type->basetype == Type::Int){
    /* variable is of type Int */
    Register* top = regStack.top();
    string regName = top->getName();
    int offset = rec->offset;
    //    cout<<"loadi(ind(ebp, "<<offset<<"), "<<regName<<")"<<endl;
    cout<<"loadi("<<val<<", "<<regName<<")"<<endl;
  }
  else if (astnode_type->basetype == Type::Float){
    /* variable is of type Float */
    Register* top = regStack.top();
    string regName = top->getName();
    int offset = rec->offset;
    //    cout<<"loadf(ind(ebp, "<<offset<<"), "<<regName<<")"<<endl;
    cout<<"loadf("<<val<<", "<<regName<<")"<<endl;
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
  for (int i = 0; i < l; i++){
    vec[i]->genCode(regStack);
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
      cout<<"store"<<type<<"("<<val<<", ind(ebp, "<<offset<<"))"<<endl;
    }
    else {
      /* RHS is exp, calculated in Reg */
      node2->genCode(regStack);
      Register *reg = regStack.top();
      string regName = reg->getName();
      VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
      int offset = rec->offset;
      cout<<"store"<<type<<"("<<regName<<", ind(ebp, "<<offset<<"))"<<endl;
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
      cout<<"store"<<type<<"("<<val<<", ind("<<regName<<"))"<<endl;
    }
    else {
      /* RHS is exp, calculated in Reg */
      node2->genCode(regStack);
      Register *reg1 = regStack.top();
      string regName1 = reg1->getName();
      
      if (regCount == 2){
	/* Need to store the result calculated in this case */
	cout<<"push"<<type<<"("<<regName1<<")"<<endl;
	swapTopReg(regStack);             // SWAP
	((Index*)node1)->genCodeLExp(regStack);
	swapTopReg(regStack);             // SWAP
	reg1 = regStack.top();
	regName1 = reg1->getName();
	regStack.pop();                 // POP
	cout<<"load"<<type<<"(ind(esp), "<<regName1<<")"<<endl;
	cout<<"pop"<<type<<"(1)"<<endl;

	Register* reg2 = regStack.top();
	string regName2 = reg2->getName();
	
	// reg2 has the addr, reg1 has the value
	cout<<"store"<<type<<"("<<regName1<<", ind("<<regName2<<"))"<<endl;
	regStack.push(reg1);              // PUSH
      }
      else {
	/* Store not needed as sufficent registers */
	regStack.pop();                 // POP
	((Index*)node1)->genCodeLExp(regStack);
	Register* reg2 = regStack.top();
	string regName2 = reg2->getName();
	
	// reg2 has the addr, reg1 has the value
	cout<<"store"<<type<<"("<<regName1<<", ind("<<regName2<<"))"<<endl;
	regStack.push(reg1);            // PUSH
      }
    }
  }
}

void While::genCode(stack<Register*> &regStack){}
void For::genCode(stack<Register*> &regStack){}
void Return::genCode(stack<Register*> &regStack){}

void If::genCode(stack<Register*> &regStack){
  node1->fall = 1;               // expression.fall = 1
  node1->genCode(regStack);      
  //int node2Start = nextInstr();
  node2->genCode(regStack);
  //Code* code = new Code(1, "jl", "");
  //codeStack.push_back(code);
  //nextList->add(code);
  //int node3Start = nextInstr();
  node3->genCode(regStack);
  //(node1->trueList)->backPatch(getInstr(node2Start));
  //(node1->falseList)->backPatch(getInstr(node3Start));
  //nextList->merge(node2->nextList);
  //nextList->merge(node3->nextList);
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
  else{//TODO
  }
}

void UnOp::genCode(stack<Register*> &regStack){
  if (op == UnOpType::NOT){
    // TODO
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
      cout<<"load"<<type<<"("<<regName<<", "<<val<<")"<<endl;
    }
  }
  else {
    node1->genCode(regStack);
    Register* reg = regStack.top();
    string regName = reg->getName();
    if (op == UnOpType::UMINUS){
      cout<<"mul"<<type<<"(-1, "<<regName<<")"<<endl;
    }
    else if (op == UnOpType::PP){
      cout<<"add"<<type<<"(1, "<<regName<<")"<<endl;
    }
  }
}

void Funcall::genCode(stack<Register*> &regStack){}

void FloatConst::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  Register *top = regStack.top();
  string regName = top->getName();
  float val = getValue();
  cout<<"loadf("<<val<<", "<<regName<<")"<<endl;
}

void IntConst::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  Register *top = regStack.top();
  string regName = top->getName();
  int val = getValue();
  cout<<"loadi("<<val<<", "<<regName<<")"<<endl;
  
}

void StringConst::genCode(stack<Register*> &regStack){}


/* This special function is to generate the code 
   needed for Index when this is used as lexpression */

void Index::genCodeLExp(stack<Register*> &regStack){
  int regCount = regStack.size();
  genCodeInternal(regStack);
  Register* reg = regStack.top();
  string regName = reg->getName();
  cout<<"addi("<<regName<<", ebs)"<<endl;
}


void Index::genCode(stack<Register*> &regStack){
  int regCount = regStack.size();
  genCodeInternal(regStack);
  Register* reg = regStack.top();
  string regName = reg->getName();
  cout<<"addi("<<regName<<", ebs)"<<endl;
  if (astnode_type->getBasetype() == Type::Int){
    cout<<"loadi(ind("<<regName<<"), "<<regName<<")"<<endl;
  }
  else if (astnode_type->getBasetype() == Type::Int){
    cout<<"loadf(ind("<<regName<<"), "<<regName<<")"<<endl;
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
      cout<<"muli("<<factor<<", "<<regName2<<")"<<endl;
      swapTopReg(regStack);               // SWAP, top restored

      // obtaining the dimension in regName
      VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
      int offset = rec->offset;
      Register *reg = regStack.top();
      string regName = reg->getName();
      cout<<"loadi("<<offset<<", "<<regName<<")"<<endl;
      
      // adding the offset to the dimension in regName
      cout<<"addi("<<regName<<", "<<regName2<<")"<<endl;
    }
    else {
      /* In this case we evaulate the LHS first */
      ((Index*)node1)->genCodeInternal(regStack);
      Register* reg1 = regStack.top();
      regStack.pop();
      node2->genCode(regStack);
      Register* reg2 = regStack.top();

      string regName1 = reg1->getName();
      string regName2 = reg2->getName();
      
      int factor = astnode_type->calcSize();
      cout<<"muli("<<factor<<", "<<regName2<<")"<<endl;
      cout<<"addi("<<regName1<<", "<<regName2<<")"<<endl;
      regStack.push(reg1);  // restore the stack
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
      cout<<"muli("<<factor<<", "<<regName2<<")"<<endl;
      swapTopReg(regStack);               // SWAP, top restored

      // obtaining the dimension in regName
      VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
      int offset = rec->offset;
      Register *reg = regStack.top();
      string regName = reg->getName();
      cout<<"loadi("<<offset<<", "<<regName<<")"<<endl;
      
      // adding the offset to the dimension in regName
      cout<<"addi("<<regName<<", "<<regName2<<")"<<endl;
    }
    else {
      /* In this case we evaulate the LHS first */
      ((Index*)node1)->genCodeInternal(regStack);
      Register* reg1 = regStack.top();
      // Saving the register value
      string regName1 = reg1->getName();
      cout<<"pushi("<<regName1<<")"<<endl;   // push the reg Value
      swapTopReg(regStack);   // SWAP
      
      node2->genCode(regStack);
      Register* reg2 = regStack.top();
      string regName2 = reg2->getName();
      swapTopReg(regStack);
      reg1 = regStack.top();
      regStack.pop();               // POP
      regName1 = reg1->getName();
      cout<<"loadi(ind(esp), "<<regName1<<")"<<endl;   // loading pushed value from stack
      cout<<"popi(1)"<<endl;    // restore esp stack

      int factor = astnode_type->calcSize();
      cout<<"muli("<<factor<<", "<<regName2<<")"<<endl;
      cout<<"addi("<<regName1<<", "<<regName2<<")"<<endl;
      regStack.push(reg1);  // restore the stack

    }
  }
  else {
    cout<<"Too low registers"<<endl;
  }
}

void FuncallStmt::genCode(stack<Register*> &regStack){}

void ToFloat::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  node1->genCode(regStack);   // constant loads itself on top Reg
  Register* top = regStack.top();
  string regName = top->getName();
  cout<<"intTofloat("<<regName<<")"<<endl;
}

void ToInt::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  node1->genCode(regStack);   // constant loads itself on top Reg
  Register* top = regStack.top();
  string regName = top->getName();
  cout<<"floatToint("<<regName<<")"<<endl;
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
    if (lr > 1 || rr > 1){

      // Case when one of subexpression is constant
      if (lr > 1 && rr > 1){
	// both subtree are constants
	T lval = ((Rtype*)node1)->getValue(), rval = ((Rtype*)node2)->getValue();	  
	Register* top = regStack.top();
	string regName = top->getName();
      
	if (opr == "Plus"){
	  cout<<"move("<<rval<<","<<regName<<")"<<endl;
	  cout<<"add"<<type<<"("<<lval<<","<<regName<<")"<<endl;
	}
	else if (opr == "Minus"){
	  rval = -1;                                               // RHS = -RHS
	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
	  cout<<"add"<<type<<"("<<rval<<","<<regName<<")"<<endl;   // LHS + -RHS
	}
	else if (opr == "Mult"){
	  cout<<"move("<<rval<<","<<regName<<")"<<endl;
	  cout<<"mul"<<type<<"("<<lval<<","<<regName<<")"<<endl;
	}
	else if (opr == "Div"){
	  cout<<"move("<<rval<<","<<regName<<")"<<endl;
	  cout<<"div"<<type<<"("<<lval<<","<<regName<<")"<<endl;   // LHS div RHS
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  cout<<"move("<<rval<<","<<regName<<")"<<endl;
	  cout<<"cmp"<<type<<"("<<lval<<","<<regName<<")"<<endl;
	  if (fall){
	    cout<<fallInstr(opr)<<"(label)"<<endl; 
	  }
	  else{
	    cout<<notFallInstr(opr)<<"(label)"<<endl;
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
	  cout<<"add"<<type<<"("<<lval<<","<<regName<<")"<<endl;
	}
	else if (opr == "Minus"){
	  cout<<"mul"<<type<<"(-1, "<<regName<<")"<<endl;             // RHS = -RHS
	  cout<<"add"<<type<<"("<<lval<<","<<regName<<")"<<endl;      // LHS + -RHS
	}
	else if (opr == "Mult"){
	  cout<<"mul"<<type<<"("<<lval<<","<<regName<<")"<<endl;
	}
	else if (opr == "Div"){
	  cout<<"div"<<type<<"("<<lval<<","<<regName<<")"<<endl;
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
     	  cout<<"cmp"<<type<<"("<<lval<<","<<regName<<")"<<endl;
	  if (fall){
	    cout<<fallInstr(opr)<<"(label)"<<endl; 
	  }
	  else{
	    cout<<notFallInstr(opr)<<"(label)"<<endl;
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
	  cout<<"add"<<type<<"("<<rval<<","<<regName<<")"<<endl;
	}
	else if (opr == "Minus"){
	  rval = -rval;                                               // RHS = -RHS
	  cout<<"add"<<type<<"("<<rval<<","<<regName<<")"<<endl;      // LHS + -RHS
	}
	else if (opr == "Mult"){
	  cout<<"mul"<<type<<"("<<rval<<","<<regName<<")"<<endl;
	}
	else if (opr == "Div"){
	  /* Need to load RHS into register */
	  regStack.pop();
	  Register* top2 = regStack.top();
	  string regName2 = top2->getName();
	  cout<<"move("<<rval<<", "<<regName2<<")"<<endl;              // Load RHS into reg
	  cout<<"div"<<type<<"("<<regName<<", "<<regName2<<")"<<endl;  // LHS div RHS
	  regStack.push(top);                                          // restore the pop
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  /* Need to load RHS into register */
	  regStack.pop();
	  Register* top2 = regStack.top();
	  string regName2 = top2->getName();
	  cout<<"move("<<rval<<", "<<regName2<<")"<<endl;              // Load RHS into reg
	  cout<<"cmp"<<type<<"("<<regName<<","<<regName2<<")"<<endl;
	  if (fall){
	    cout<<fallInstr(opr)<<"(label)"<<endl; 
	  }
	  else{
	    cout<<notFallInstr(opr)<<"(label)"<<endl;
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
	node1->genCode(regStack);

	Register* top1 = regStack.top();
	string regName1 = top1->getName();
	string regName2 = top2->getName();

	if (opr == "Plus"){
	  cout<<"add"<<type<<"("<<regName1<<", "<<regName2<<")"<<endl;
	}
	else if (opr == "Minus"){
	  cout<<"mul"<<type<<"(-1, "<<regName2<<")"<<endl;                // RHS = -RHS
	  cout<<"add"<<type<<"("<<regName1<<", "<<regName2<<")"<<endl;    // LHS + -RHS
	}
	else if (opr == "Mult"){
	  cout<<"mul"<<type<<"("<<regName1<<", "<<regName2<<")"<<endl;
	}
	else if (opr == "Div"){
	  cout<<"div"<<type<<"("<<regName1<<", "<<regName2<<")"<<endl;
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  cout<<"cmp"<<type<<"("<<regName1<<","<<regName2<<")"<<endl;
	  if (fall){
	    cout<<fallInstr(opr)<<"(label)"<<endl; 
	  }
	  else{
	    cout<<notFallInstr(opr)<<"(label)"<<endl;
	  }
	}
	else {
	  cout<<"Operation not supported"<<endl;
	}

	regStack.push(top2);
	swapTopReg(regStack); // top1 contanis the evaulated expression
      }
      else {

	// only 2 reg left, need for a store in this case
	node2->genCode(regStack);

	Register* top1 = regStack.top();
	string regName1 = top1->getName();
	cout<<"push"<<type<<"("<<regName1<<")"<<endl;   // store the value
	node1->genCode(regStack);
	  
	swapTopReg(regStack);
	Register* top2 = regStack.top();
	string regName2 = top2->getName();
	// To load the esp into regName2
	cout<<"load"<<type<<"(ind(esp), "<<regName2<<")"<<endl;
	cout<<"pop"<<type<<"(1)"<<endl;
	regStack.pop();
	top1 = regStack.top();
	regName1 = top1->getName();

	if (opr == "Plus"){
	  cout<<"add"<<type<<"("<<regName1<<", "<<regName2<<")"<<endl;
	}
	else if (opr == "Minus"){
	  cout<<"mul"<<type<<"(-1, "<<regName2<<")"<<endl;
	  cout<<"add"<<type<<"("<<regName1<<", "<<regName2<<")"<<endl;
	}
	else if (opr == "Mult"){
	  cout<<"mul"<<type<<"("<<regName1<<", "<<regName2<<")"<<endl;
	}
	else if (opr == "Div"){
	  cout<<"div"<<type<<"("<<regName1<<", "<<regName2<<")"<<endl;
	}
	else if (opr == "LT" || opr == "GT" || opr =="GE_OP" || opr == "LE_OP" || opr == "EQ_OP" || opr == "NE_OP"){
	  cout<<"cmp"<<type<<"("<<regName1<<","<<regName2<<")"<<endl;
	  if (fall){
	    cout<<fallInstr(opr)<<"(label)"<<endl; 
	  }
	  else{
	    cout<<notFallInstr(opr)<<"(label)"<<endl;
	  }
	}
	else {
	  cout<<"Operation not supported"<<endl;
	}

	regStack.push(top2);   // store the reg back
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
	cout<<"load"<<type<<"("<<val<<", "<<regName<<")"<<endl;   // top reg contains the value
	cout<<"store"<<type<<"("<<val<<", ind(ebp, "<<offset<<"))"<<endl;
      }
      else {
	/* RHS is exp, calculated in Reg */
	node2->genCode(regStack);
	Register *reg = regStack.top();   // top reg contains the value
	string regName = reg->getName();
	VarRecord* rec = (VarRecord*)((Identifier*)node1)->getRecord();
	int offset = rec->offset;
	cout<<"store"<<type<<"("<<regName<<", ind(ebp, "<<offset<<"))"<<endl;
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
	cout<<"store"<<type<<"("<<val<<", ind("<<regName<<"))"<<endl;
	cout<<"load"<<type<<"("<<val<<", "<<regName<<")"<<endl;
      }
      else {
	/* RHS is exp, calculated in Reg */
	node2->genCode(regStack);
	Register *reg1 = regStack.top();
	string regName1 = reg1->getName();
      
	if (countReg == 2){
	  /* Need to store the result calculated in this case */
	  cout<<"push"<<type<<"("<<regName1<<")"<<endl;
	  swapTopReg(regStack);             // SWAP
	  ((Index*)node1)->genCodeLExp(regStack);
	  swapTopReg(regStack);             // SWAP
	  reg1 = regStack.top();
	  regName1 = reg1->getName();
	  regStack.pop();                 // POP
	  cout<<"load"<<type<<"(ind(esp), "<<regName1<<")"<<endl;
	  cout<<"pop"<<type<<"(1)"<<endl;

	  Register* reg2 = regStack.top();
	  string regName2 = reg2->getName();
	
	  // reg2 has the addr, reg1 has the value
	  cout<<"store"<<type<<"("<<regName1<<", ind("<<regName2<<"))"<<endl;
	  regStack.push(reg1);              // PUSH
	}
	else {
	  /* Store not needed as sufficent registers */
	  regStack.pop();                 // POP
	  ((Index*)node1)->genCodeLExp(regStack);
	  Register* reg2 = regStack.top();
	  string regName2 = reg2->getName();
	
	  // reg2 has the addr, reg1 has the value
	  cout<<"store"<<type<<"("<<regName1<<", ind("<<regName2<<"))"<<endl;
	  regStack.push(reg1);            // PUSH
	}
      }
    }

  }
  else {
    
  }
}
