%debug                   
%filenames Parser
%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token PP 

 //%baseclass­preinclude globals.cc

%%

S : 
    E  
    {std::cout << ($1 == -1 ? x : a[$1]) << std::endl;}
  ;
E :
    E  '+' T      
       {($$) = i;
	      a[i++] = ($1 == -1 ? x : a[$1]) + ($3 == -1 ? x : a[$3]);}
   |  
    T 
       {$$ = $1;}
   ;

T : 
     '(' E ')' 
     {$$ = $1;}
  |
     PP 'x'   
     { 
       x++;
       $$ = -1;
     }
  |    
     'x'       
      {$$ = -1;}     
  ;
