#include <vector>
#include <string>
using namespace std;

enum OpType{
  OR_OP = 0,
  AND_OP, 
  EQ_OP,
  NE_OP,
  LT,
  GT,
  LE_OP,
  GE_OP,
  PLUS,
  MINUS,
  MULT,
  ASSIGN,
  DIV
};

enum UnOpType{
  UMINUS = 0,
  NOT,
  PP
};

class Type {
public:
  int dim;
  enum Kind {
    Base, Pointer, Error, Ok
  };
  enum Basetype {
      Int, Float, Void, String
  };
  Kind tag;
  union {
    Basetype basetype;
    Type* pointed;
  };
  Type();    // Default
  Type(Kind); // Error, Ok
  Type(Kind, Basetype); //Int, Float, String
  Type(Kind, Type*);   // Pointer
  ~Type();
  void printType();
  int calcSize();
  Basetype getBasetype();
};


/* class representing the registers */

class Register {
private:
  /* the name of register eg. eax */
  string name;

public:
  /* Constructor needs name */
  Register(string _name);

  /* getter method to obtain the register name */
  string getName();
};


////////////////////////////////
/* class for holding the code */
////////////////////////////////


class Code {
public:
  /* Code representation, 
     funcall, argument1, <optional argument2> */
  int argCount;
  string func, arg1, arg2; 
  
  /* Code may have a label attached */
  int hasLabel;
  string label;
  
  /* Goto statements are special */
  int isGoto;
  
  /* operations , int = 1 means goto type */
  Code(int, string, string, string);
  Code(int, string, string);
  
  /* backpatch the goto with another Code */
  void backPatch(Code*);
  
  /* set label from a list of global labels */
  void setLabel();
  
  /* get current Label */
  string getLabel();
  
  /* prints the Code accordingly */
  void print();
};




///////////////////////////////////////////////////////
/* classes for handling loops, proper goto direction */
///////////////////////////////////////////////////////


/* Represents the list of Code lines 
   Eg. TrueList, FalseList, NextList */

class CList {
public:
  /* pointer to Codes aggregated under this list */
  vector<Code*> arr;
  
  /* add new Code to the array */
  void add(Code*);
  
  /* backPatch the arr with sprecified Code line */
  void backPatch(Code*);
};


//////////////////////
/* ast defined here */
//////////////////////


class abstract_astnode
{
public:
  /* rType = 2, constant exp, rType = 1, identifier, 0 otherwise */
  int rType; 
  
  virtual void print () = 0;
  //virtual std::string generate_code(const symbolTable&) = 0;
  virtual Type* getType() = 0;
  virtual void setType(Type*) = 0;
  virtual void genCode(stack<Register*> &regStack) = 0;
  virtual int getrType();
  
protected:
  Type* astnode_type;
  
private:
  
};



class StmtAst : public abstract_astnode {
public:
  //  virtual void print () = 0;
  CList *nextList;           // nextlist for this 
  
  StmtAst();
  Type* getType();
  void setType(Type*);
  void setLeaf();
};

class ExpAst : public abstract_astnode {
public:
  //  virtual void print () = 0;
  int fall;                                  // denotes fall through
  CList *nextList, *trueList, *falseList;    // lists

  ExpAst();
  Type* getType();
  void setType(Type*);
  void setLeaf();
};

class ArrayRef : public ExpAst {
public:
  ArrayRef();
  virtual void printFold () = 0;
};

class Identifier : public ArrayRef {
protected:
  string val;
  GlRecord* rec;

public:
  Identifier(string _val);
  void print();
  void printFold();
  void genCode(stack<Register*> &regStack);
  string getIdentifierName();
  void setRecord(GlRecord*);
  GlRecord* getRecord();
};

class BlockAst : public StmtAst {
protected:
  vector<StmtAst*> vec;
  
public:
  BlockAst();
  void print();
  void add(StmtAst* stmtAst);
  void genCode(stack<Register*> &regStack);
};

class Ass : public StmtAst {
protected:
  ExpAst* node1;
  ExpAst* node2;

public:
  Ass(ExpAst* node1, ExpAst* node2);
  void print();
  void genCode(stack<Register*> &regStack);
  template<class T, class R>
  void genCodeTemplate(T d1, R d2, stack<Register*> &regStack, string type);
};

class While : public StmtAst {
protected:
  ExpAst* node1;
  StmtAst* node2;

public:
  While(ExpAst* node1, StmtAst* node2);
  void print();
  void genCode(stack<Register*> &regStack);
};

class For : public StmtAst {
protected:
  ExpAst* node1;
  ExpAst* node2;
  ExpAst* node3;
  StmtAst* node4;

public:
  For(ExpAst* node1, ExpAst* node2, ExpAst* node3, StmtAst* node4);
  void print();
  void genCode(stack<Register*> &regStack);
};

class Return : public StmtAst {
protected:
  ExpAst* node1;
  
public:
  Return(ExpAst* node1);
  void print();
  void genCode(stack<Register*> &regStack);
};


class If : public StmtAst {
protected:
  ExpAst* node1;
  StmtAst* node2;
  StmtAst* node3;
  
public:
  If(ExpAst* node1, StmtAst* node2, StmtAst* node3);
  void print();
  void genCode(stack<Register*> &regStack);
};


class Op : public ExpAst {
protected:
  ExpAst* node1;
  ExpAst* node2;
  OpType op;
  
public:
  Op(ExpAst* _node1, ExpAst* _node2, OpType _op);
  void print();
  void genCode(stack<Register*> &regStack);

  template<class T, class Rtype>
  void genCodeTemplate(T d1, Rtype d2, string type, stack<Register*>&regStack, string opr);
};


class UnOp : public ExpAst {
protected:
  ExpAst* node1;
  UnOpType op;
  
public:
  UnOp(ExpAst* node1, UnOpType _op);
  UnOp(UnOpType _op);
  void print();
  void setExp(ExpAst* node1);
  void genCode(stack<Register*> &regStack);

  template<class T, class R>
  void genCode(T d1, R d2, stack<Register*> &regStack, string type);
};


class Funcall : public ExpAst {
protected:
  vector<ExpAst*> vec;
  Identifier* funName;

public:
  Funcall();
  void setName(Identifier* _funName);
  void print();
  void addExp(ExpAst* exp);
  void genCode(stack<Register*> &regStack);
};

class FloatConst : public ExpAst {
protected:
  float val;

public:
  FloatConst(float _val);
  void print();
  void genCode(stack<Register*> &regStack);
  float getValue();
};

class IntConst : public ExpAst {
protected:
  int val;

public:
  IntConst(int _val);
  void print();
  void genCode(stack<Register*> &regStack);
  int getValue();
};

class StringConst : public ExpAst {
protected:
  string val;

public:
  StringConst(string _val);
  void print();
  void genCode(stack<Register*> &regStack);
  string getValue();
};


class Index : public ArrayRef {
protected:
  ArrayRef* node1;
  ExpAst* node2;

public:
  Index(ArrayRef* node1, ExpAst* node2);
  void print();
  void printFold();
  void genCode(stack<Register*> &regStack);
  void genCodeInternal(stack<Register*> &regStack);
  void genCodeLExp(stack<Register*>&regStack);
};



class FuncallStmt : public StmtAst {
protected:
  Funcall* node1;
  
public:
  FuncallStmt(Funcall* node1);
  void print();
  void genCode(stack<Register*> &regStack);
};


class ToFloat : public ExpAst {
protected:
  ExpAst* node1;
  
public:
  ToFloat(ExpAst* node1);
  void print();
  void genCode(stack<Register*> &regStack);
};

class ToInt : public ExpAst {
protected:
  ExpAst* node1;
  
public:
  ToInt(ExpAst* node1);
  void print();
  void genCode(stack<Register*> &regStack);
};



