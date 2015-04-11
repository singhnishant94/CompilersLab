#include <iostream>
//#include "headers.h"
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


/* Definitions for the AST's */

int abstract_astnode :: getrType(){
  return rType;
}

Type* StmtAst::getType(){
  return astnode_type;
}

void StmtAst::setType(Type* t){
  astnode_type = t;
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

// void Ass :: print(){
    
//   if (node1 != 0 && node2 != 0){
//     Type* t1 = node1->getType();
//     Type* t2 = node2->getType();
//     if (t1->basetype == Type::Int && t2->basetype == Type::Float){
//       cout<<"(Ass ";
//       node1->print();
//       cout<<" ";
//       cout<<"(TO_INT ";
//       node2->print();
//       cout<<" )";
//     }
//     else if (t1->basetype == Type::Float && t2->basetype == Type::Int){
//       cout<<"(Ass ";
//       node1->print();
//       cout<<" ";
//       cout<<"(TO_FLOAT ";
//       node2->print();
//       cout<<" )";
//     }
//     else{
//       cout<<"(Ass ";
//       node1->print();
//       cout<<" ";
//       node2->print();
//       cout<<")";
//     }
//   }
//   else{
//     cout<<"(Empty)";
//   }
// }

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

// void Op :: print(){
//   string opr = op_value[op];
//   if (opr == "OR_OP" || opr == "AND_OP"){
//     cout<<"("<<opr<<" ";
//     node1->print();
//     cout<<" ";
//     node2->print();
//     cout<<")";
//   }
//   else if (opr == "Assign_exp"){
//     Type* t1 = node1->getType();
//     Type* t2 = node2->getType();
//     if (t1->basetype == Type::Int && t2->basetype == Type::Float){
// 	cout<<"("<<opr<<" ";
// 	node1->print();
// 	cout<<" ";
// 	cout<<"(TO_INT ";
// 	node2->print();
// 	cout<<" )";
//     }
//     else if (t1->basetype == Type::Float && t2->basetype == Type::Int){
//         cout<<"("<<opr<<" ";
// 	node1->print();
// 	cout<<" ";
// 	cout<<"(TO_FLOAT ";
// 	node2->print();
// 	cout<<" )";
//     }
//     else{
//         cout<<"("<<opr<<" ";
// 	node1->print();
// 	cout<<" ";
// 	node2->print();
// 	cout<<")";
//     }
//   }
//   else{
//     Type* t1 = node1->getType();
//     Type* t2 = node2->getType();
//     if (t1->basetype == Type::Int && t2->basetype == Type::Int){
//       cout<<"("<<opr<<"_Int ";
//       node1->print();
//       cout<<" ";
//       node2->print();
//       cout<<")";
//     }
//     else {
//       cout<<"("<<opr<<"_FLOAT ";
//       if (t1->basetype == Type::Int){
// 	cout<<"(TO_FLOAT ";
// 	node1->print();
// 	cout<<")";
//       }
//       else {
// 	node1->print();
//       }
//       cout<<" ";
      
//       if (t2->basetype == Type::Int){
// 	cout<<"(TO_FLOAT ";
// 	node2->print();
// 	cout<<")";
//       }
//       else{
// 	node2->print();
//       }
//       cout<<")";
//     }
//   }
// }


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
void If::genCode(stack<Register*> &regStack){}

void Op::genCode(stack<Register*> &regStack){
  int countReg = regStack.size();
  string opr = op_value[op];
  
  if (opr == "Plus" && astnode_type->basetype == Type::String){
    // TODO for strings
  }
  else if (opr == "Plus" || opr == "Minus" || opr == "Mult" || opr == "Div"){
    if (astnode_type->basetype == Type::Int){
      int d1;
      IntConst d2(1);
      string type = "i";
      genCodeTemplate(d1, d2, type, regStack, opr);
    }
    else if (astnode_type->basetype == Type::Float){
      float d1;
      FloatConst d2(1.0);
      string type = "f";
      genCodeTemplate(d1, d2, type, regStack, opr);
    }
    else {
      cout<<"Type not supported"<<endl;
    }
  }
  else {
    // TODO for other cases
  }
}

void UnOp::genCode(stack<Register*> &regStack){}
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
      else {
	cout<<"operation not supported"<<endl;
      }

    }
    else {
      // right is constant
      node1->genCode(regStack);  // gencode for right
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
      else {
	cout<<"Operation not supported"<<endl;
      }

      regStack.push(top2);   // store the reg back
      swapTopReg(regStack);   // restore regName1 to top
    }
  }
}


// void Op::genCode(stack<Register*> &regStack){
//   if (astnode_type->basetype == Type::Int){
//     int dummy;
//     string type = "i";
//     IntConst dummy2(1);
//     genCodeTemplate(dummy, type, regStack, dummy2);
//   }
//   else {
//     float dummy;
//     string type = "f";
//     FloatConst dummy2(1.0);
//     genCodeTemplate(dummy, type, regStack, dummy2);
//   }
  
//   return;
//   int countReg = regStack.size();
//   string opr = op_value[op];
//   /*  --------------------------------------PLUS---------------------------------------------------------- */

//   if (opr == "Plus"){    
//     /* addition is handled here */
    
//     if (astnode_type->basetype == Type::Int){
//       /* handling the int plus here */
//       int lr = node1->getrType();
//       int rr = node2->getrType();
//       if (lr > 1 || rr > 1){

// 	// Case when one of subexpression is constant
// 	if (lr > 1 && rr > 1){
// 	  // both are constants
// 	  int lval = ((IntConst*)node1)->getValue(), rval = ((IntConst*)node2)->getValue();
	  
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
// 	  cout<<"addi("<<rval<<","<<regName<<")"<<endl;
// 	}
// 	else if (lr > 1){
// 	  // left is constant
// 	  node2->genCode(regStack);  // gencode for right
// 	  int lval = ((IntConst*)node1)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addi("<<lval<<","<<regName<<")"<<endl;
// 	}
// 	else {
// 	  // right is constant
// 	  node1->genCode(regStack);  // gencode for right
// 	  int rval = ((IntConst*)node2)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addi("<<rval<<","<<regName<<")"<<endl;
// 	}
//       }
//       // else if (lr > 0 || rr > 0){
//       // 	// one of the exp is an identifier
//       // 	if (lr > 0){
//       // 	  // left exp is an identifier
//       // 	  node2->genCode(regStack); // gen code for right, top reg has value
//       // 	  swapTopReg(regStack);
//       // 	  node1->genCode(regStack);  // gen loadi code from the LHS
//       // 	  swapTopReg(regStack);
	  
//       // 	  // adding the top two registers
//       // 	  Register* top1 = regStack.top();  
//       // 	  string regName1 = top1->getName();
//       // 	  regStack.pop();
	  
//       // 	  Register* top2 = regStack.top(); 
//       // 	  string regName2 = top2->getName();
	  
//       // 	  regStack.push(top1);
//       // 	  cout<<"add("<<regName1<<","<<regName2<<")"<<endl;

//       // 	}
//       // 	else {
//       // 	  // right expression is an identifier
//       // 	  node1->genCode(regStack); // gen code for left,top reg has value
//       // 	  swapTopReg(regStack);
//       // 	  node2->genCode(regStack);  // gen loadi code from the RHS
//       // 	  swapTopReg(regStack);
	  
//       // 	  // adding the top two registers
//       // 	  Register* top1 = regStack.top();  
//       // 	  string regName1 = top1->getName();
//       // 	  regStack.pop();
	  
//       // 	  Register* top2 = regStack.top(); 
//       // 	  string regName2 = top2->getName();
	  
//       // 	  regStack.push(top1);
//       // 	  cout<<"add("<<regName1<<","<<regName2<<")"<<endl;

//       // 	}
//       // }
//       else {

// 	/* When none of subtree is constant */
// 	if (countReg > 2){

// 	  // Storeless add possible
// 	  swapTopReg(regStack);
// 	  node2->genCode(regStack);  // Evaluating the right subtree

// 	  Register* top2 = regStack.top();
// 	  regStack.pop();
// 	  node1->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  string regName2 = top2->getName();
// 	  cout<<"addi("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);
// 	  swapTopReg(regStack); // top1 contanis the evaulated expression
// 	}
// 	else {

// 	  // only 2 reg left, need for a store in this case
// 	  node2->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  cout<<"pushi("<<regName1<<")"<<endl;   // store the value
// 	  node1->genCode(regStack);
	  
// 	  swapTopReg(regStack);
// 	  Register* top2 = regStack.top();
// 	  string regName2 = top2->getName();
// 	  // To load the esp into regName2
// 	  cout<<"loadi(ind(esp), "<<regName2<<")"<<endl;
// 	  cout<<"popi(1)"<<endl;
// 	  regStack.pop();
// 	  top1 = regStack.top();
// 	  regName1 = top1->getName();
// 	  cout<<"addi("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);   // store the reg back
// 	  swapTopReg(regStack);   // restore regName1 to top
// 	}
//       }
//     }
//     else if (astnode_type->basetype == Type::Float){
//       /* handling the FLOAT case here */

//       int lr = node1->getrType();
//       int rr = node2->getrType();
//       if (lr > 1 || rr > 1){

// 	// Case when one of subexpression is constant
// 	if (lr > 1 && rr > 1){
// 	  // both subtree are constants

// 	  float lval = ((FloatConst*)node1)->getValue(), rval = ((FloatConst*)node2)->getValue();
	  
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
// 	  cout<<"addf("<<rval<<","<<regName<<")"<<endl;
// 	}
// 	else if (lr > 1){
// 	  // left subtree is constant

// 	  node2->genCode(regStack);  // gencode for right
// 	  float lval = ((FloatConst*)node1)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addf("<<lval<<","<<regName<<")"<<endl;
// 	}
// 	else {
// 	  // right subtree is constant

// 	  node1->genCode(regStack);  // gencode for right
// 	  float rval = ((FloatConst*)node2)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addf("<<rval<<","<<regName<<")"<<endl;
// 	}
//       }
//       else {

// 	/* When none of subtree is constant */
// 	if (countReg > 2){

// 	  // Storeless add possible
// 	  swapTopReg(regStack);
// 	  node2->genCode(regStack);  // Evaluating the right subtree

// 	  Register* top2 = regStack.top();
// 	  regStack.pop();
// 	  node1->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  string regName2 = top2->getName();
// 	  cout<<"addf("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);
// 	  swapTopReg(regStack); // top1 contanis the evaulated expression
// 	}
// 	else {

// 	  // only 2 reg left, need for a store in this case
// 	  node2->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  cout<<"pushf("<<regName1<<")"<<endl;   // store the value
// 	  node1->genCode(regStack);
	  
// 	  swapTopReg(regStack);
// 	  Register* top2 = regStack.top();
// 	  string regName2 = top2->getName();
// 	  // To load the esp into regName2
// 	  cout<<"loadf(ind(esp), "<<regName2<<")"<<endl;
// 	  cout<<"popf(1)"<<endl;
// 	  regStack.pop();
// 	  top1 = regStack.top();
// 	  regName1 = top1->getName();
// 	  cout<<"addf("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);   // store the reg back
// 	  swapTopReg(regStack);   // restore regName1 to top
	
// 	}
//       }
//     }
//     else {
//       /* handling string here */
//       // TODO
//     }
//   }
//   /*  ----------------------------------------MINUS---------------------------------------------------------- */
//   else if (opr == "Minus"){
//     /* multiplication is handled here */
    
//     if (astnode_type->basetype == Type::Int){
//       /* handling the int plus here */
//       int lr = node1->getrType();
//       int rr = node2->getrType();
//       if (lr > 1 || rr > 1){

// 	// Case when one of subexpression is constant
// 	if (lr > 1 && rr > 1){
// 	  // both are constants
// 	  int lval = ((IntConst*)node1)->getValue(), rval = ((IntConst*)node2)->getValue();
// 	  // the constant is negated for using add
// 	  rval *= -1;        
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
// 	  cout<<"addi("<<rval<<","<<regName<<")"<<endl;
// 	}
// 	else if (lr > 1){
// 	  // left is constant
// 	  node2->genCode(regStack);          // gencode for right
// 	  int lval = ((IntConst*)node1)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"muli(-1, "<<regName<<")"<<endl;             // negating the register value by mult with -1
// 	  cout<<"addi("<<lval<<","<<regName<<")"<<endl;      // normal addition 
// 	}
// 	else {
// 	  // right is constant
// 	  node1->genCode(regStack);           // gencode for right
// 	  int rval = ((IntConst*)node2)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  // constant is negated to use add function
// 	  rval = -rval;
// 	  cout<<"addi("<<rval<<","<<regName<<")"<<endl;
// 	}
//       }
//       else {

// 	/* When none of subtree is constant */
// 	if (countReg > 2){

// 	  // Storeless add possible
// 	  swapTopReg(regStack);
// 	  node2->genCode(regStack);  // Evaluating the right subtree

// 	  Register* top2 = regStack.top();
// 	  regStack.pop();
// 	  node1->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();    // LHS
// 	  string regName2 = top2->getName();    // RHS
// 	  cout<<"muli(-1, "<<regName2<<")"<<endl;                // RHS = -RHS
// 	  cout<<"addi("<<regName1<<", "<<regName2<<")"<<endl;    // LHS + (-RHS)
// 	  regStack.push(top2);
// 	  swapTopReg(regStack); // top1 contanis the evaulated expression
// 	}
// 	else {

// 	  // only 2 reg left, need for a store in this case
// 	  node2->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  cout<<"muli(-1, "<<regName1<<")"<<endl;    // RHS = -RHS
// 	  cout<<"pushi("<<regName1<<")"<<endl;   // store the value, -RHS
// 	  node1->genCode(regStack);
	  
// 	  swapTopReg(regStack);
// 	  Register* top2 = regStack.top();
// 	  string regName2 = top2->getName();
// 	  // To load the esp into regName2
// 	  cout<<"loadi(ind(esp), "<<regName2<<")"<<endl;   // -RHS in regName2
// 	  cout<<"popi(1)"<<endl;
// 	  regStack.pop();
// 	  top1 = regStack.top();
// 	  regName1 = top1->getName();
// 	  cout<<"addi("<<regName1<<", "<<regName2<<")"<<endl;    // LHS + (-RHS)
// 	  regStack.push(top2);   // store the reg back
// 	  swapTopReg(regStack);   // restore regName1 to top
// 	}
//       }
//     }
//     else if (astnode_type->basetype == Type::Float){
//       /* handling the FLOAT case here */

//       int lr = node1->getrType();
//       int rr = node2->getrType();
//       if (lr > 1 || rr > 1){

// 	// Case when one of subexpression is constant
// 	if (lr > 1 && rr > 1){
// 	  // both subtree are constants

// 	  float lval = ((FloatConst*)node1)->getValue(), rval = ((FloatConst*)node2)->getValue();
// 	  rval = -rval;                         // RHS = -RHS 
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
// 	  cout<<"addf("<<rval<<","<<regName<<")"<<endl;    // LHS + (-RHS)
// 	}
// 	else if (lr > 1){
// 	  // left subtree is constant

// 	  node2->genCode(regStack);  // gencode for right
// 	  float lval = ((FloatConst*)node1)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"mulf(-1, "<<regName<<")"<<endl;            // RHS = -RHS
// 	  cout<<"addf("<<lval<<","<<regName<<")"<<endl;     // LHS + (-RHS)
// 	}
// 	else {
// 	  // right subtree is constant

// 	  node1->genCode(regStack);  // gencode for right
// 	  float rval = ((FloatConst*)node2)->getValue();
// 	  rval = -rval;                           // RHS = -RHS
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addf("<<rval<<","<<regName<<")"<<endl;    // LHS = LHS +  -RHS
// 	}
//       }
//       else {

// 	/* When none of subtree is constant */
// 	if (countReg > 2){

// 	  // Storeless add possible
// 	  swapTopReg(regStack);
// 	  node2->genCode(regStack);  // Evaluating the right subtree

// 	  Register* top2 = regStack.top();
// 	  regStack.pop();
// 	  node1->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  string regName2 = top2->getName();
// 	  cout<<"mulf(-1, "<<regName2<<")"<<endl;                      // RHS = -RHS
// 	  cout<<"addf("<<regName1<<", "<<regName2<<")"<<endl;          // LHS + -RHS
// 	  regStack.push(top2);
// 	  swapTopReg(regStack); // top1 contanis the evaulated expression
// 	}
// 	else {

// 	  // only 2 reg left, need for a store in this case
// 	  node2->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  cout<<"mulf(-1, "<<regName1<<")"<<endl;          // RHS = -RHS
// 	  cout<<"pushf("<<regName1<<")"<<endl;   // store the value, -RHS
// 	  node1->genCode(regStack);
	  
// 	  swapTopReg(regStack);
// 	  Register* top2 = regStack.top();
// 	  string regName2 = top2->getName();
// 	  // To load the esp into regName2
// 	  cout<<"loadf(ind(esp), "<<regName2<<")"<<endl;    // load -RHS
// 	  cout<<"popf(1)"<<endl;
// 	  regStack.pop();
// 	  top1 = regStack.top();
// 	  regName1 = top1->getName();
// 	  cout<<"addf("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);   // store the reg back
// 	  swapTopReg(regStack);   // restore regName1 to top
	
// 	}
//       }
//     }
//   }
//   /*  -----------------------------------------MULT---------------------------------------------------------- */
//   else if (opr == "Mult"){
//     /* Multiplication is handled here */
    
//     if (astnode_type->basetype == Type::Int){
//       /* handling the int plus here */
//       int lr = node1->getrType();
//       int rr = node2->getrType();
//       if (lr > 1 || rr > 1){

// 	// Case when one of subexpression is constant
// 	if (lr > 1 && rr > 1){
// 	  // both are constants
// 	  int lval = ((IntConst*)node1)->getValue(), rval = ((IntConst*)node2)->getValue();
	  
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
// 	  cout<<"muli("<<rval<<","<<regName<<")"<<endl;
// 	}
// 	else if (lr > 1){
// 	  // left is constant
// 	  node2->genCode(regStack);  // gencode for right
// 	  int lval = ((IntConst*)node1)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"muli("<<lval<<","<<regName<<")"<<endl;
// 	}
// 	else {
// 	  // right is constant
// 	  node1->genCode(regStack);  // gencode for right
// 	  int rval = ((IntConst*)node2)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"muli("<<rval<<","<<regName<<")"<<endl;
// 	}
//       }
//       else {

// 	/* When none of subtree is constant */
// 	if (countReg > 2){

// 	  // Storeless mul possible
// 	  swapTopReg(regStack);
// 	  node2->genCode(regStack);  // Evaluating the right subtree

// 	  Register* top2 = regStack.top();
// 	  regStack.pop();
// 	  node1->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  string regName2 = top2->getName();
// 	  cout<<"muli("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);
// 	  swapTopReg(regStack); // top1 contanis the evaulated expression
// 	}
// 	else {

// 	  // only 2 reg left, need for a store in this case
// 	  node2->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  cout<<"pushi("<<regName1<<")"<<endl;   // store the value
// 	  node1->genCode(regStack);
	  
// 	  swapTopReg(regStack);
// 	  Register* top2 = regStack.top();
// 	  string regName2 = top2->getName();
// 	  // To load the esp into regName2
// 	  cout<<"loadi(ind(esp), "<<regName2<<")"<<endl;
// 	  cout<<"popi(1)"<<endl;
// 	  regStack.pop();
// 	  top1 = regStack.top();
// 	  regName1 = top1->getName();
// 	  cout<<"muli("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);   // store the reg back
// 	  swapTopReg(regStack);   // restore regName1 to top
// 	}
//       }
//     }
//     else if (astnode_type->basetype == Type::Float){
//       /* handling the FLOAT case here */

//       int lr = node1->getrType();
//       int rr = node2->getrType();
//       if (lr > 1 || rr > 1){

// 	// Case when one of subexpression is constant
// 	if (lr > 1 && rr > 1){
// 	  // both subtree are constants

// 	  float lval = ((FloatConst*)node1)->getValue(), rval = ((FloatConst*)node2)->getValue();
	  
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
// 	  cout<<"mulf("<<rval<<","<<regName<<")"<<endl;
// 	}
// 	else if (lr > 1){
// 	  // left subtree is constant

// 	  node2->genCode(regStack);  // gencode for right
// 	  float lval = ((FloatConst*)node1)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"mulf("<<lval<<","<<regName<<")"<<endl;
// 	}
// 	else {
// 	  // right subtree is constant

// 	  node1->genCode(regStack);  // gencode for right
// 	  float rval = ((FloatConst*)node2)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"mulf("<<rval<<","<<regName<<")"<<endl;
// 	}
//       }
//       else {

// 	/* When none of subtree is constant */
// 	if (countReg > 2){

// 	  // Storeless mul possible
// 	  swapTopReg(regStack);
// 	  node2->genCode(regStack);  // Evaluating the right subtree

// 	  Register* top2 = regStack.top();
// 	  regStack.pop();
// 	  node1->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  string regName2 = top2->getName();
// 	  cout<<"mulf("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);
// 	  swapTopReg(regStack); // top1 contanis the evaulated expression
// 	}
// 	else {

// 	  // only 2 reg left, need for a store in this case
// 	  node2->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  cout<<"pushf("<<regName1<<")"<<endl;   // store the value
// 	  node1->genCode(regStack);
	  
// 	  swapTopReg(regStack);
// 	  Register* top2 = regStack.top();
// 	  string regName2 = top2->getName();
// 	  // To load the esp into regName2
// 	  cout<<"loadf(ind(esp), "<<regName2<<")"<<endl;
// 	  cout<<"popf(1)"<<endl;
// 	  regStack.pop();
// 	  top1 = regStack.top();
// 	  regName1 = top1->getName();
// 	  cout<<"mulf("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);   // store the reg back
// 	  swapTopReg(regStack);   // restore regName1 to top
	
// 	}
//       }
//     }
//   }
//   /*  -----------------------------------------DIV---------------------------------------------------------- */
//   else if (opr == "Div"){
//         /* addition is handled here */
    
//     if (astnode_type->basetype == Type::Int){
//       /* handling the int plus here */
//       int lr = node1->getrType();
//       int rr = node2->getrType();
//       if (lr > 1 || rr > 1){

// 	// Case when one of subexpression is constant
// 	if (lr > 1 && rr > 1){
// 	  // both are constants
// 	  int lval = ((IntConst*)node1)->getValue(), rval = ((IntConst*)node2)->getValue();
	  
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
// 	  cout<<"addi("<<rval<<","<<regName<<")"<<endl;
// 	}
// 	else if (lr > 1){
// 	  // left is constant
// 	  node2->genCode(regStack);  // gencode for right
// 	  int lval = ((IntConst*)node1)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addi("<<lval<<","<<regName<<")"<<endl;
// 	}
// 	else {
// 	  // right is constant
// 	  node1->genCode(regStack);  // gencode for right
// 	  int rval = ((IntConst*)node2)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addi("<<rval<<","<<regName<<")"<<endl;
// 	}
//       }
//       else {

// 	/* When none of subtree is constant */
// 	if (countReg > 2){

// 	  // Storeless add possible
// 	  swapTopReg(regStack);
// 	  node2->genCode(regStack);  // Evaluating the right subtree

// 	  Register* top2 = regStack.top();
// 	  regStack.pop();
// 	  node1->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  string regName2 = top2->getName();
// 	  cout<<"addi("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);
// 	  swapTopReg(regStack); // top1 contanis the evaulated expression
// 	}
// 	else {

// 	  // only 2 reg left, need for a store in this case
// 	  node2->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  cout<<"pushi("<<regName1<<")"<<endl;   // store the value
// 	  node1->genCode(regStack);
	  
// 	  swapTopReg(regStack);
// 	  Register* top2 = regStack.top();
// 	  string regName2 = top2->getName();
// 	  // To load the esp into regName2
// 	  cout<<"loadi(ind(esp), "<<regName2<<")"<<endl;
// 	  cout<<"popi(1)"<<endl;
// 	  regStack.pop();
// 	  top1 = regStack.top();
// 	  regName1 = top1->getName();
// 	  cout<<"addi("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);   // store the reg back
// 	  swapTopReg(regStack);   // restore regName1 to top
// 	}
//       }
//     }
//     else if (astnode_type->basetype == Type::Float){
//       /* handling the FLOAT case here */

//       int lr = node1->getrType();
//       int rr = node2->getrType();
//       if (lr > 1 || rr > 1){

// 	// Case when one of subexpression is constant
// 	if (lr > 1 && rr > 1){
// 	  // both subtree are constants

// 	  float lval = ((FloatConst*)node1)->getValue(), rval = ((FloatConst*)node2)->getValue();
	  
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
// 	  cout<<"addf("<<rval<<","<<regName<<")"<<endl;
// 	}
// 	else if (lr > 1){
// 	  // left subtree is constant

// 	  node2->genCode(regStack);  // gencode for right
// 	  float lval = ((FloatConst*)node1)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addf("<<lval<<","<<regName<<")"<<endl;
// 	}
// 	else {
// 	  // right subtree is constant

// 	  node1->genCode(regStack);  // gencode for right
// 	  float rval = ((FloatConst*)node2)->getValue();
// 	  Register* top = regStack.top();
// 	  string regName = top->getName();
// 	  cout<<"addf("<<rval<<","<<regName<<")"<<endl;
// 	}
//       }
//       else {

// 	/* When none of subtree is constant */
// 	if (countReg > 2){

// 	  // Storeless add possible
// 	  swapTopReg(regStack);
// 	  node2->genCode(regStack);  // Evaluating the right subtree

// 	  Register* top2 = regStack.top();
// 	  regStack.pop();
// 	  node1->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  string regName2 = top2->getName();
// 	  cout<<"addf("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);
// 	  swapTopReg(regStack); // top1 contanis the evaulated expression
// 	}
// 	else {

// 	  // only 2 reg left, need for a store in this case
// 	  node2->genCode(regStack);

// 	  Register* top1 = regStack.top();
// 	  string regName1 = top1->getName();
// 	  cout<<"pushf("<<regName1<<")"<<endl;   // store the value
// 	  node1->genCode(regStack);
	  
// 	  swapTopReg(regStack);
// 	  Register* top2 = regStack.top();
// 	  string regName2 = top2->getName();
// 	  // To load the esp into regName2
// 	  cout<<"loadf(ind(esp), "<<regName2<<")"<<endl;
// 	  cout<<"popf(1)"<<endl;
// 	  regStack.pop();
// 	  top1 = regStack.top();
// 	  regName1 = top1->getName();
// 	  cout<<"addf("<<regName1<<", "<<regName2<<")"<<endl;
// 	  regStack.push(top2);   // store the reg back
// 	  swapTopReg(regStack);   // restore regName1 to top
	
// 	}
//       }
//     }

//   }
//   /*  -----------------------------------------ASSIGN_EXP---------------------------------------------------------- */
//   else if (opr == "Assign_exp"){
    
//   }
//   else {
    
//   }
// }
