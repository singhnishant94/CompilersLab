%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP EQ_OP NE_OP LE_OP GE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN IDENTIFIER INC_OP
  
  %polymorphic expAst : ExpAst*; stmtAst : StmtAst*; Int : int; Float : float; String : string;

%type <stmtAst> selection_statement iteration_statement assignment_statement translation_unit function_definition compound_statement statement statement_list

%type <expAst> expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression postfix_expression primary_expression l_expression constant_expression expression_list unary_operator

%type <Int> INT_CONSTANT
%type <Float> FLOAT_CONSTANT
%type <String> STRING_LITERAL IDENTIFIER

%type <Int> type_specifier
%%

pre_translation_unit
        : translation_unit
	{
	    globalTab->print();
	}
        ;

translation_unit
	: function_definition 
	{
	    $$ = $1; $$->print();
	    cout<<endl;
	}
	| translation_unit function_definition 
	{
	  $2->print();
	  cout<<endl;
	}
        ;

function_definition
	: type_specifier fun_declarator 
	{
	    curOffset = -4;
	}
	compound_statement 
	{
	  $$ = $4;
	  currentTab = globalTab;
	}
	;

type_specifier
	: VOID
	{
	    currentType = BasicVarType::VOID;
	    width = 0;
	}
        | INT   
	{
	    currentType = BasicVarType::INT;
	    width = 4;
	}
	| FLOAT 
	{
	    currentType = BasicVarType::FLOAT;
	    width = 4;
	}
        ;

fun_declarator
	: IDENTIFIER '(' 
	{
	    curOffset = 4;
	    BasicType* retType = new BasicType(currentType);
	    func = new FuncRecord(retType, $1);
	    if (currentTab->find(RecordType::FUNC, $1)){
	      cout<<"Function already defined, Line : "<<lineNo<<endl; exit(0);
	    }
	    func->offset = GlOffset;
	    currentTab->add(func);
	    currentTab = func->localSymTab;
	    
	}
	parameter_list ')' 
	{
	    func->paramList = headParam;
	    headParam = 0;
	    curParam = 0;
	    curOffset = 0;
	}
        | IDENTIFIER '(' ')' 
	{
	    curOffset = 0;
	    BasicType* retType = new BasicType(currentType);
	    func = new FuncRecord(retType, $1);
	    if (currentTab->find(RecordType::FUNC, $1)){
		cout<<"Function already defined, Line "<<lineNo<<endl; exit(0);
	    }		
	    func->offset = GlOffset;
	    currentTab->add(func);
	    currentTab = func->localSymTab;
	    GlOffset += curOffset;
	    curOffset = 0;
	}
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration 
	;

parameter_declaration
	: type_specifier declarator 
	{
	  if (currentType == BasicVarType::VOID){
	    cout<<"Variable declared void at line : "<<lineNo<<endl; exit(0);
	  }
	  
	    GlType* temp = new BasicType(currentType);
	    if (head != 0){
		current->typeName = temp;
		temp = head;
		width = ((ArrayType*)temp)->calcSize();
		head = 0;
		current = 0;
	    }
	    VarRecord* var = new VarRecord(temp, curName);
	    if (!currentTab->find(RecordType::VAR, curName)){
		var->offset = curOffset;
		curOffset += width;
		currentTab->add(var);
		
		if (headParam == 0){
		    headParam = new Param(var);
		    curParam = headParam;
		}
		else{
		    curParam = curParam->add(var);
		}
	    }
	    else{
	      cout<<"Variable "<<curName<<" already declared. line : "<<lineNo<<endl; exit(0);
	    }
	}
        ;

declarator
	: IDENTIFIER 
	{
	    curName = $1;
	}
	| declarator '[' INT_CONSTANT ']' 
	{
	    if (head == 0){
		head = new ArrayType();
		current = head;
		current->dim = $3;
	    }
	    else{
		current->typeName = new ArrayType();
		current = (ArrayType*)current->typeName;
		current->dim = $3;
	    }
	}
        ;

constant_expression 
        : INT_CONSTANT
	{
	  $$ = new IntConst($1);
	  $$->setType(new Type(Type::Base, Type::Int));
	} 
        | FLOAT_CONSTANT 
	{
	  $$ = new FloatConst($1);
	  $$->setType(new Type(Type::Base, Type::Float));
	}
        ;

compound_statement
	: '{' '}'
	{
	  $$ = new BlockAst();
	  $$->setType(new Type(Type::Ok));
	} 
	| '{' statement_list '}' 
	{
	  $$ = $2;
	}
        | '{' declaration_list statement_list '}' 
	{
	  $$ = $3;
	}
	;

statement_list
	: statement
	{
	  StmtAst* temp = $1;
	  $$ = new BlockAst();
	  Type *t = temp->getType();
	  
	  if (t->tag == Type::Error){
	    $$->setType(new Type(Type::Error));
	  }
	  else {
	    $$->setType(new Type(Type::Ok));
	  }
	  
	  ((BlockAst*)$$)->add(temp);
	}		
        | statement_list statement
	{
	  $$ = $1;
	  Type *t2 = $2->getType();
	  Type *t1 = $1->getType();
	  
	  if (t1->tag != Type::Error){
	    if (t2->tag == Type::Error){
	      delete t1;
	      $$->setType(new Type(Type::Error));
	    }
	  }
	  
	  ((BlockAst*)$$)->add($2);
	}	
	;

statement
        : '{' statement_list '}'  //a solution to the local decl problem
	{
	  $$ = $2;
	}
        | selection_statement 	
	{
	  $$ = $1;
	}
        | iteration_statement 	
	{
	  $$ = $1;
	}
	| assignment_statement	
	{
	  $$ = $1;
	}
        | RETURN expression ';'	
	{
	  $$ = new Return($2);
	  Type *t = $2->getType();
	  if (t->tag == Type::Error){
	    $$->setType(new Type(Type::Error));
	  }
	  else {
	    $$->setType(new Type(Type::Ok));
	  }
	}
        | IDENTIFIER '(' ')' ';'
	{
	  
	}
	| IDENTIFIER '(' expression_list ')' ';'
	{
	  
	}
        ;

assignment_statement
        : ';'
	{
	  $$ = new Ass(0, 0);
	  $$->setType(new Type(Type::Ok));
	}
        |  l_expression '=' expression ';'	
	{
	  Type *t1 = $1->getType();
	  Type *t3 = $3->getType();
	  $$ = new Ass($1, $3);
	  
	  if (t1->tag == Type::Error || t3->tag == Type::Error){
	    $$->setType(new Type(Type::Error));
	  }
	  else {
	      if (compatible(t1, t3)){
		  $$->setType(new Type(Type::Ok));
	      }
	      else {
		  $$->setType(new Type(Type::Error));
		  cout<<"Incompatible types at line number: "<<lineNo<<endl; exit(0);
	      }
	  }
	}
	;

expression
        : logical_and_expression 
	{
	    $$ = $1;
	}
        | expression OR_OP logical_and_expression
	{
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::OR_OP);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
	;

logical_and_expression
        : equality_expression
	{
	  $$ = $1;
	}
        | logical_and_expression AND_OP equality_expression 
	{
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::AND_OP);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
	;

equality_expression
	: relational_expression 
	{
	  $$ = $1;
	}
        | equality_expression EQ_OP relational_expression 	
	{
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::EQ_OP);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
	| equality_expression NE_OP relational_expression
	{ 
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::NE_OP);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
	;
relational_expression
	: additive_expression
	{
	  $$ = $1;
	}
        | relational_expression '<' additive_expression 
	{ 
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::LT);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
        | relational_expression '>' additive_expression
	{ 
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::GT);
	  $$->setType(formType(t, $3->getType(), lineNo));
	} 
	| relational_expression LE_OP additive_expression 
	{ 
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::LE_OP);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
        | relational_expression GE_OP additive_expression 
	{ 
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::GE_OP);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
	;

additive_expression 
	: multiplicative_expression
	{
	  $$ = $1;
	}
	| additive_expression '+' multiplicative_expression 
	{ 
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::PLUS);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
	| additive_expression '-' multiplicative_expression 
	{
	  Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::MINUS);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
	;

multiplicative_expression
	: unary_expression
	{
	  $$ = $1;
	}
	| multiplicative_expression '*' unary_expression 
	{ Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::MULT);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
	| multiplicative_expression '/' unary_expression 
	{ Type *t = $1->getType();
	  $$ = new Op($1, $3, OpType::DIV);
	  $$->setType(formType(t, $3->getType(), lineNo));
	}
        ;
unary_expression
	: postfix_expression
	{
	  $$ = $1;
	}  				
	| unary_operator postfix_expression 
	{
	  $$ = $1;
	  ((UnOp*)$$)->setExp($2);
	  Type* t = $2->getType();
	  if (t->tag == Type::Error){
	      $$->setType(new Type(Type::Error));
	  }
	  else{
	      if (t->basetype == Type::Int){
		  $$->setType(new Type(Type::Base ,Type::Int));
	      }
	      else if (t->basetype == Type::Float){
		  $$->setType(new Type(Type::Base ,Type::Float));
	      }
	  }
	}
	;

postfix_expression
	: primary_expression
	{
	    $$ = $1;
	}
	| IDENTIFIER '(' ')'
	{
	    $$ = new Funcall();
	    Identifier* id = new Identifier($1);
	    ((Funcall*)$$)->setName(id);
	  
	    if (!globalTab->find(RecordType::FUNC, $1)){
	      if ($1 == "printf"){ // case for library function
		// setting the return type to Int for printf
		$$->setType(new Type(Type::Base, Type::Int));
	      }
	      else {
		cout<<"Function "<<$1<<" not found. Line No : "<<lineNo<<endl; exit(0);
		$$->setType(new Type(Type::Error));
	      }
	    }
	    else {
		func = (FuncRecord*)globalTab->find(RecordType::FUNC, $1);
		if (func->paramList != 0){
		  cout<<"Too Few Arguments to function at line no : "<<lineNo<<endl; exit(0);
		}
		else $$->setType(getVarType(func->returnType));
	    }
	}
	| IDENTIFIER '(' 
	{
	    func = (FuncRecord*)globalTab->find(RecordType::FUNC, $1);
	    if (!func){
	      if ($1 == "printf"){
		libFunc = 1;
	      }
	      else {
		cout<<"Function "<<$1<<" not found. line : "<<lineNo<<endl; exit(0);
		libFunc = 0;
	      }
	      curParam = 0;
	    }
	    else{
	      curParam = func->paramList;
	      libFunc = 0;
	    }
	}
	expression_list ')' 
	{

	    $$ = $4;
	    Identifier* id = new Identifier($1);
	    ((Funcall*)$$)->setName(id);

	    Type *t = $$->getType(); 	    
	    if (t == 0){
	      if (libFunc){ // case for library function
		$$->setType(new Type(Type::Base, Type::Int));
		libFunc = 0;
	      }
	      else {
		if (curParam != 0){
		  cout<<"Less Number of parameters provided!! LineNo : "<<lineNo<<endl; exit(0);
		  $$->setType(new Type(Type::Error));
		}
		else {
		  $$->setType(getVarType(func->returnType));
		}
	      }
	    }
	    else {
	      libFunc = 0; // reset the value
	    }
	}
	| l_expression INC_OP
	{
	  Type* t = $1->getType();
	  $$ = new UnOp($1, UnOpType::PP);
	  
	  
	    if (t->tag != Type::Error && t->basetype == Type::Int){
		$$->setType(t);
	    }
	    else{
		$$->setType(new Type(Type::Error));
	    }
		
	}
	;

primary_expression
	: l_expression
	{
	  $$ = $1;
	}
        | l_expression '=' expression // added this production
	{
	  Type* t1 = $1->getType();
	  Type* t2 = $3->getType();
	  $$ = new Op($1, $3, OpType::ASSIGN);
	  if (compatible(t1, t2)){
	      $$->setType(new Type(Type::Base, Type::Int));
	  }
	  else{
	      $$->setType(new Type(Type::Error));
	      cout<<"Incompatible Types at lineNo :"<<lineNo<<endl; exit(0);
	  }

	}
	| INT_CONSTANT
	{
	  $$ = new IntConst($1);
	  $$->setType(new Type(Type::Base, Type::Int));
	}
	| FLOAT_CONSTANT
	{
	  $$ = new FloatConst($1);
	  $$->setType(new Type(Type::Base, Type::Float));
	}
        | STRING_LITERAL
	{
	  $$ = new StringConst($1);
	  $$->setType(new Type(Type::Base, Type::String));
	}
	| '(' expression ')'
	{
	  $$ = $2;
	}
	;

l_expression
        : IDENTIFIER
	{
	    $$ = new Identifier($1);
	    VarRecord* temp;
	    temp = (VarRecord*)currentTab->find(RecordType::VAR, $1);
	    
	    if (!temp){
		cout<<"Variable "<<$1<<" hasn't been declared. line : "<<lineNo<<endl; exit(0);
		$$->setType(new Type(Type::Error));
	    }
	    else{
		curGlType = temp->keyType;
		Type *t = getVarType(curGlType);
		$$->setType(t);
	    }
	}
        | l_expression '[' expression ']' 	
	{
	  Type* t1 = $1->getType();
	  Type* t3 = $3->getType();

	  $$ = new Index((ArrayRef*)$1, $3);

	    if (t3->basetype != Type::Int){
		cout<<"Index Of array not integer !!, Lineno : "<<lineNo<<endl; exit(0);
		$$->setType(new Type(Type::Error));
	    }
	    else if (t1->tag != Type::Pointer){
	        cout<<"Cannot Dereference non pointer type, lineNo : "<<lineNo<<endl; exit(0);
		$$->setType(new Type(Type::Error));
	    }
	    else {
		$$->setType(t1->pointed);
	    }
	  
	    if (curGlType->type != VarType::BASIC){
		//cout<<"Incompatible Types. line :"<<lineNo<<endl; exit(0);
		curGlType = ((ArrayType*)curGlType)->typeName;
	    }
	}
        ;


expression_list
        : expression
	{

	    ExpAst* temp = $1;
	    $$ = new Funcall();
	    ((Funcall*)$$)->addExp(temp);
	    $$->setType(0);
	    
	    if (libFunc){ // this part is for bypassing the library functions as printf
	      Type* t = temp->getType();
	      if (t->tag == Type::Error){ // handling the error in expression
		$$->setType(t);
	      }
	    }
	    else {
	      if (func == 0){
		$$->setType(new Type(Type::Error));
	      }
	      else {
		if (curParam == 0){
		  cout<<"Function accepts no parameters!! lineNo : "<<lineNo<<endl; exit(0);
		  $$->setType(new Type(Type::Error));
		}
		else {
		  Type* t = getVarType((curParam->rec)->keyType);
		  if (equal(t, temp->getType())){
		    curParam = curParam->next;
		  }
		  else {
		    cout<<"Mismatched parameter!! lineno: "<<lineNo<<endl; exit(0);
		    $$->setType(new Type(Type::Error));
		  }
		}
	      }
	    }
	}
        | expression_list ',' expression
	{
	  
	    $$ = $1;
	    ((Funcall*)$$)->addExp($3);
	    Type* t = $$->getType();
	    
	    if (t == 0){
	      if (libFunc){ // handling the library func
		Type *t3 = $3->getType();
		if (t3->tag == Type::Error){   // chcking for error in expression $3
		  $$->setType(t3);
		}
	      }
	      else {     // user defined function
		if (curParam == 0){
		  cout<<"More than required parameters provided!! lineNo : "<<lineNo<<endl; exit(0);
		  $$->setType(new Type(Type::Error));
		}
		else {
		  t = getVarType((curParam->rec)->keyType);
		  if (equal(t, $3->getType())){
		    curParam = curParam->next;
		  }
		  else {
		    cout<<"Mismatched parameter!! lineno: "<<lineNo<<endl; exit(0);
		    $$->setType(new Type(Type::Error));
		  }
		}
	      } 
	    }
	}
        ;

unary_operator
        : '-'	
	{
	  $$ = new UnOp(UnOpType::UMINUS);
	}
	| '!' 	
	{
	  $$ = new UnOp(UnOpType::NOT);
	}
	;

selection_statement
        : IF '(' expression ')' statement ELSE statement 
	{
	  Type *t3 = $3->getType();
	  Type *t5 = $5->getType();
	  Type *t7 = $7->getType();
	  $$ = new If($3, $5, $7);
	  if (t3->tag == Type::Error || t5->tag == Type::Error || t7->tag == Type::Error){
	    $$->setType(new Type(Type::Error));
	  }
	  else{
	    $$->setType(new Type(Type::Ok));
	  }
	}
	;

iteration_statement
	: WHILE '(' expression ')' statement 	
	{
	  Type *t3 = $3->getType();
	  Type *t5 = $5->getType();
	  $$ = new While($3, $5);
	  if (t3->tag == Type::Error || t5->tag == Type::Error){
	    $$->setType(new Type(Type::Error));
	  }
	  else{
	    $$->setType(new Type(Type::Ok));
	  }
	}
        | FOR '(' expression ';' expression ';' expression ')' statement  //modified this production
	{
	  Type *t3 = $3->getType();
	  Type *t5 = $5->getType();
	  Type *t7 = $7->getType();
	  Type *t9 = $9->getType();
	  $$ = new For($3, $5, $7, $9);
	  if (t3->tag == Type::Error || t5->tag == Type::Error || t7->tag == Type::Error || t9->tag == Type::Error){
	    $$->setType(new Type(Type::Error));
	  }
	  else{
	    $$->setType(new Type(Type::Ok));
	  }
	}
        ;

declaration_list
        : declaration  					
        | declaration_list declaration
	;

declaration
	: type_specifier 
	{
	  if (currentType == BasicVarType::VOID){
	    cout<<"Variable declared void at line : "<<lineNo<<endl; exit(0);
	  }
	}
	declarator_list';'
	;

declarator_list
	: declarator
	{
	    GlType* temp = new BasicType(currentType);
	    if (head != 0){
		current->typeName = temp;
		temp = head;
		width = ((ArrayType*)temp)->calcSize();
		head = 0;
		current = 0;
	    }
	    VarRecord* var = new VarRecord(temp, curName);
	    if (!currentTab->find(RecordType::VAR, curName)){
		var->offset = curOffset;
		curOffset -= width;
		currentTab->add(var);
	    }
	    else{
		cout<<"Variable "<<curName<<" already declared. Line: "<<lineNo<<endl; exit(0);
	    }
	    
	}
	| declarator_list ',' declarator 
	{
	    GlType* temp = new BasicType(currentType);
	    if (head != 0){
		current->typeName = temp;
		temp = head;
		head = 0;
		current = 0;
		
	    }
	    VarRecord* var = new VarRecord(temp, curName);
	    if (!currentTab->find(RecordType::VAR, curName)){
		var->offset = curOffset;
		curOffset -= width;
		currentTab->add(var);
	    }
	    else{
		cout<<"Variable "<<curName<<" already declared.Line no "<<lineNo<<endl; exit(0);
	    }
	}
	;

