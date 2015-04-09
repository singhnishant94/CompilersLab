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
}

Type::Type(Kind kindval, Type* ptd) :
  tag(kindval), pointed(ptd) {
}

Type::~Type()
{switch (tag) {
   case Pointer: delete pointed;
   break;
}}

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
    return new Type(Type::Base, relBaseType(((BasicType*)typ)->typeName));
  }
  else {
    return new Type(Type::Pointer, getVarType(((ArrayType*)typ)->typeName));
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
}

void Identifier :: print(){
  cout<<"(Id \""<<val<<"\")";
}

void Identifier :: printFold(){
  this->print();
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
  
  if (node1->getrType() > 1){
    rType = 2;
  }
  else {
    rType = 1;
  }

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

  if (node1->getrType() > 1){
    rType = 2;
  }
  else {
    rType = 1;
  }
}

void ToInt::print(){
  cout<<"(TO_INT ";
  node1->print();
  cout<<")";
}

//////////////////////////////////////////////////////
/* global functions to help register management */

stack<Register*> regStack;   // tmp fix

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


//////////////////////////////////////////////////////
/* the genCode functions for all the ast are defined here */


void Identifier::genCode(){}
void BlockAst::genCode(){}
void Ass::genCode(){}
void While::genCode(){}
void For::genCode(){}
void Return::genCode(){}
void If::genCode(){}

void Op::genCode(){
  string opr = op_value[op];
  if (opr == "Plus"){
    if (astnode_type->basetype == Type::Int){
      /* handling the int plus here */
      int lr = node1->getrType();
      int rr = node2->getrType();
      if (lr > 1 || rr > 1){
	if (lr > 1 && rr > 1){
	  // both are constants
	  int lval = ((IntConst*)node1)->getValue(), rval = ((IntConst*)node2)->getValue();
	  
	  Register* top = regStack.top();
	  string regName = top->getName();
	  cout<<"move("<<lval<<","<<regName<<")"<<endl;
	  cout<<"addi("<<rval<<","<<regName<<")"<<endl;
	}
	else if (lr > 1){
	  // left is constant
	  node2->genCode();  // gencode for right
	  int lval = ((IntConst*)node1)->getValue();
	  Register* top = regStack.top();
	  string regName = top->getName();
	  cout<<"addi("<<lval<<","<<regName<<")"<<endl;
	}
	else {
	  // right is constant
	  node1->genCode();  // gencode for right
	  int rval = ((IntConst*)node2)->getValue();
	  Register* top = regStack.top();
	  string regName = top->getName();
	  cout<<"addi("<<rval<<","<<regName<<")"<<endl;
	}
      }
      else if (lr > 0 || rr > 0){
	// one of the exp is an identifier
	if (lr > 0){
	  // left exp is an identifier
	  node2->genCode(); // gen code for right,top reg has value
	  swapTopReg(regStack);
	  node1->genCode();  // gen loadi code from the LHS
	  swapTopReg(regStack);
	  
	  // adding the top two registers
	  Register* top1 = regStack.top();  
	  string regName1 = top1->getName();
	  regStack.pop();
	  
	  Register* top2 = regStack.top(); 
	  string regName2 = top2->getName();
	  
	  regStack.push(top1);
	  cout<<"add("<<regName1<<","<<regName2<<")"<<endl;

	}
	else {
	  // right expression is an identifier
	  node1->genCode(); // gen code for left,top reg has value
	  swapTopReg(regStack);
	  node2->genCode();  // gen loadi code from the RHS
	  swapTopReg(regStack);
	  
	  // adding the top two registers
	  Register* top1 = regStack.top();  
	  string regName1 = top1->getName();
	  regStack.pop();
	  
	  Register* top2 = regStack.top(); 
	  string regName2 = top2->getName();
	  
	  regStack.push(top1);
	  cout<<"add("<<regName1<<","<<regName2<<")"<<endl;

	}
      }
      else {
      
      }
    }
    else if (astnode_type->basetype == Type::Float){
      // handling the float case here
    }
    else {
      /* handling string here */
      
    }
  }
  else if (opr == "Minus"){
  
  }
  else if (opr == "Mult"){
  
  }
  else if (opr == "Div"){
  
  }
  else if (opr == "Assign_exp"){
    
  }
  else {
    
  }
}

void UnOp::genCode(){}
void Funcall::genCode(){}
void FloatConst::genCode(){}
void IntConst::genCode(){}
void StringConst::genCode(){}
void Index::genCode(){}
void FuncallStmt::genCode(){}
void ToFloat::genCode(){}
void ToInt::genCode(){}
