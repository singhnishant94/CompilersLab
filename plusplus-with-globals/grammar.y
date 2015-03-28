%debug                   
%filenames Parser
%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token PP 
%polymorphic stacktype: Stacktype
%type<stacktype> E T S

%%
S : E  {std::cout << ($$).val << "\n";}
;
E :  
E '+' T    {($$).val = ($1).val + ($3).val + (($1).atomic ? ($3).pps : 0);
               ($$).pps = ($1).pps + ($3).pps;
               ($$).atomic = 0;}
  | 
     T        {($$).val = ($1).val;
               ($$).pps = ($1).pps;
               ($$).atomic = ($1).atomic;}
;
T : 
    '(' E ')' {($$).val = ($2).val;
               ($$).pps = ($2).pps;
               ($$).atomic = ($2).atomic;}
  |
    PP 'x'   {incr++; ($$).val = incr + 1;
              ($$).pps = 1;
              ($$).atomic = 1;}
  |    
   'x'       {($$).val = incr + 1;
              ($$).pps = 0;
              ($$).atomic = 1;}      
;
   


