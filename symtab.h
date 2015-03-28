#include <string>
#include <map>
using namespace std;

enum RecordType{
    VAR,
    FUNC,
};

enum VarType{
    BASIC,
    ARR
};

enum BasicVarType{
    INT,
    VOID,
    FLOAT,
    STRING
};


class GlType {
 public:
    VarType type;
    
};


class GlRecord {
 public:
    RecordType type;
    string name;
    int offset;
};

class SymTab;
class VarRecord;

class Param {
 public:
    VarRecord* rec;
    Param* next;

    Param(VarRecord*);
    Param* add(VarRecord*);
};

class FuncRecord : public GlRecord {
 public:
    GlType* returnType;
    SymTab* localSymTab;
    Param* paramList;
    FuncRecord(GlType*, string);
    bool match(string);
    void print(){}
};

class VarRecord : public GlRecord {
 public:
    GlType* keyType;
    VarRecord(GlType*, string);
    GlRecord* match(string);
    void print();
};

class BasicType : public GlType {
 public:
    BasicVarType typeName;
    
    //constructor
    BasicType(BasicVarType);
    void print();
};

class ArrayType : public GlType {
 public:
    int dim;
    GlType* typeName;

    //constructor
    ArrayType();
    ArrayType(GlType*);
    void print();
    int calcSize();
};

class SymTab {
 public:
    map<string, GlRecord*> entries;
    GlRecord* find(RecordType, string); 

    SymTab();
    void add(GlRecord*);
    void print();
};


