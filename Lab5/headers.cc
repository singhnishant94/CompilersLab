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


class abstract_astnode
{
public:
  /* rType = 2, constant exp, rType = 1, identifier, 0 otherwise */
  int rType; 
  
  virtual void print () = 0;
  //virtual std::string generate_code(const symbolTable&) = 0;
  virtual Type* getType() = 0;
  virtual void setType(Type*) = 0;
  virtual void genCode() = 0;
  virtual int getrType();
  
protected:
  Type* astnode_type;
  
private:
  
};



class StmtAst : public abstract_astnode {
public:
  //  virtual void print () = 0;
  Type* getType();
  void setType(Type*);
  void setLeaf();
};

class ExpAst : public abstract_astnode {
public:
  //  virtual void print () = 0;
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

public:
  Identifier(string _val);
  void print();
  void printFold();
  void genCode();
  string getIdentifierName();
};

class BlockAst : public StmtAst {
protected:
  vector<StmtAst*> vec;
  
public:
  BlockAst();
  void print();
  void add(StmtAst* stmtAst);
  void genCode();
};

class Ass : public StmtAst {
protected:
  ExpAst* node1;
  ExpAst* node2;

public:
  Ass(ExpAst* node1, ExpAst* node2);
  void print();
  void genCode();
};

class While : public StmtAst {
protected:
  ExpAst* node1;
  StmtAst* node2;

public:
  While(ExpAst* node1, StmtAst* node2);
  void print();
  void genCode();
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
  void genCode();
};

class Return : public StmtAst {
protected:
  ExpAst* node1;
  
public:
  Return(ExpAst* node1);
  void print();
  void genCode();
};


class If : public StmtAst {
protected:
  ExpAst* node1;
  StmtAst* node2;
  StmtAst* node3;
  
public:
  If(ExpAst* node1, StmtAst* node2, StmtAst* node3);
  void print();
  void genCode();
};


class Op : public ExpAst {
protected:
  ExpAst* node1;
  ExpAst* node2;
  OpType op;
  
public:
  Op(ExpAst* _node1, ExpAst* _node2, OpType _op);
  void print();
  void genCode();
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
  void genCode();
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
  void genCode();
};

class FloatConst : public ExpAst {
protected:
  float val;

public:
  FloatConst(float _val);
  void print();
  void genCode();
  float getValue();
};

class IntConst : public ExpAst {
protected:
  int val;

public:
  IntConst(int _val);
  void print();
  void genCode();
  int getValue();
};

class StringConst : public ExpAst {
protected:
  string val;

public:
  StringConst(string _val);
  void print();
  void genCode();
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
  void genCode();
};



class FuncallStmt : public StmtAst {
protected:
  Funcall* node1;
  
public:
  FuncallStmt(Funcall* node1);
  void print();
  void genCode();
};


class ToFloat : public ExpAst {
protected:
  ExpAst* node1;
  
public:
  ToFloat(ExpAst* node1);
  void print();
  void genCode();
};

class ToInt : public ExpAst {
protected:
  ExpAst* node1;
  
public:
  ToInt(ExpAst* node1);
  void print();
  void genCode();
};

