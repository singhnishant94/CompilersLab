//#include "symtab.h"

Param::Param(VarRecord* _rec){
    rec = _rec;
    next = 0;
}

Param* Param::add(VarRecord* _rec){
    Param* newP = new Param(_rec);
    next = newP;
    return newP;
}

FuncRecord::FuncRecord(GlType* retType, string _name){
    returnType = retType;
    name = _name;
    type = FUNC;
    paramList = 0;
    offset = 0;
    localSymTab = new SymTab();
}

VarRecord::VarRecord(GlType* _keyType, string _name){
    keyType = _keyType;
    type = VAR;
    name = _name;
    offset = 0;
}

void VarRecord::print(){
    cout<<name<<" ";
    if (keyType->type == BASIC){
	cout<<"\t";
	((BasicType*)keyType)->print();
	cout<<"Offset: "<<offset;
    }
    else{
	cout<<"\t";
	((ArrayType*)keyType)->print();
	cout<<" Offset: "<<offset;
    }
}
	
void FuncRecord::print(){
    localSymTab->print();
}

BasicType::BasicType(BasicVarType bvt){
    typeName = bvt;
    type = BASIC;
}

void BasicType::print(){
    switch (typeName){
    case INT : cout<<"INT"<<" ";
	break;
	    
    case FLOAT : cout<<"FLOAT"<<" ";
	break;
    }
}

ArrayType::ArrayType(GlType* glt){
    typeName = glt;
    type = ARR;
}

void ArrayType::print(){
    cout<<"(array "<<dim<<", ";
    if (typeName->type == BASIC){
	((BasicType*)typeName)->print();
    }
    else{
	((ArrayType*)typeName)->print();
    }
    cout<<")";
}

ArrayType::ArrayType(){
    typeName = 0;
    type = ARR;
}

int ArrayType::calcSize(){
    if (typeName->type == VarType::BASIC){
	if (((BasicType*)typeName)->typeName == BasicVarType::INT){
	    return 4*dim;
	}
	if (((BasicType*)typeName)->typeName == BasicVarType::FLOAT){
	    return 4*dim;
	}
	else return 0;
    }
    else{
	return dim*(((ArrayType*)typeName)->calcSize());
    }
}
	
GlRecord* SymTab::find(RecordType r, string n) {
    GlRecord* cur = 0;
    if (entries.find(n) != entries.end()){
	cur = entries[n];
    }
    else return 0;
    if (cur->type == r) return cur;
    return 0;
}

void SymTab::add(GlRecord* rec){
    entries[rec->name] = rec;
}

SymTab::SymTab(){
  
}

void SymTab::print(){
    map<string, GlRecord*>::iterator itr;
    GlRecord* temp;
    for (itr = entries.begin(); itr != entries.end() ;++itr){
	temp = itr->second;
	
	if (temp->type == VAR){
	    cout<<"\t";
	    ((VarRecord*)temp)->print();
	}
	else{
	    cout<<"Local symbol-table for function: "<<itr->first<<endl;
	    
	    ((FuncRecord*)temp)->print();
	}
	cout<<endl;
    }
}
//int main(){return 0;}
