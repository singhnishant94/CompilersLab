
%%

"x"                               {return 'x';}
"+"                               {return '+';}
"++"                              {return Parser::PP;}
"("                               {return '(';}
")"                               {return ')';}
[ \t\n]+                          // skip white space chars.
.                                 // skip anything else
