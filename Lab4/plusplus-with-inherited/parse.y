%debug                   
%filenames Parser
%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token PP 
%polymorphic stacktype: Stacktype
%type<stacktype> E T S

%%
S : {$<stacktype>$.incr = 0;}  
    E  
    {std::cout << ($<stacktype>2).val << "\n";}
  ;

E :
      {$<stacktype>$.incr = $<stacktype>0.incr;}  
     
     E  '+' 

      {($<stacktype>$).incr = ($<stacktype>0).incr + ($<stacktype>2).pps;} 

      T    
      {($<stacktype>$).val = ($<stacktype>2).val + 
                             ($<stacktype>5).val + 
                             (($<stacktype>2).atomic ? ($<stacktype>5).pps : 0);
       ($<stacktype>$).pps = ($<stacktype>2).pps + ($<stacktype>5).pps;
       ($<stacktype>$).atomic = 0;
      }

      |

     {($<stacktype>$).incr = ($<stacktype>0).incr;}

      T   
     
     {($<stacktype>$).val = ($<stacktype>2).val;
       ($<stacktype>$).pps = ($<stacktype>2).pps;
       ($<stacktype>$).atomic = ($<stacktype>2).atomic;}
  ;

T : 
    '(' 
     {($<stacktype>$).incr = ($<stacktype>0).incr;} 
     E 
    ')' {($<stacktype>$).val = ($<stacktype>3).val;
         ($<stacktype>$).pps = ($<stacktype>3).pps;
         ($<stacktype>$).atomic = ($<stacktype>3).atomic;}
  |
    PP 'x'   
    {($<stacktype>$).val = ($<stacktype>0).incr + 2;
      ($<stacktype>$).pps = 1;
      ($<stacktype>$).atomic = 1;}
  |    
   'x'       
      {($<stacktype>$).val = ($<stacktype>0).incr + 1;
      ($<stacktype>$).pps = 0;
      ($<stacktype>$).atomic = 1;}      
;
   


