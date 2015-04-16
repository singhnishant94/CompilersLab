whitespaces		[ \t]+
newLine                 [\n]

//constants
C-integer-constant		([1-9][0-9]*)|(0[bB][01]*)|(0[0-7]*)|(0[xX][0-9a-fA-F]*)
exponent-part		([eE][+-][0-9]*)|([eE][0-9]*)		
integer-part		[0-9]+
fractional-part		[0-9]+		
floating-point-constant		(({integer-part}\.{fractional-part})|([0-9]+\.)|(\.[0-9]+))({exponent-part}?)
C-constant		{C-integer-constant}|{floating-point-constant}

//identifier
identifier		[[:alpha:]_][[:alpha:][:digit:]_]*
separator		[\(\)\{\}\[\]\;]
operator		[\+\-\*\/\<\>\^\&\|\!]]

OR_OP	\|\|
AND_OP	\&\&
EQ_OP	\=\=
NE_OP	\!\=
LE_OP	\<\=
GE_OP	\>\=

INC_OP		\+\+
STRINGLITERAL		\"[^\\"]*(((\\\\)*(\\[nt"])?[^\\"]*)?)*\"


//keywords
VOID		void
INT			int
FLOAT		float
FOR			for
IF			if
ELSE		else
WHILE		while
RETURN         return

keyword		{VOID}|{INT}|{FLOAT}|{FOR}|{IF}|{ELSE}|{WHILE}

OTHER		.

%%
{whitespaces}
{newLine}                    {Parser::lineNo++;}
{C-integer-constant}		{
                                    
                                    istringstream ss(matched());
                                    int t;
                                    ss >> t;
                                    Parser::d_val__ = t;
                                    return Parser::INT_CONSTANT;
                                }
{floating-point-constant}	{
                                    istringstream ss(matched()); 
                                    float t;
                                    ss >> t;
                                    Parser::d_val__ = t; 
	                            return Parser::FLOAT_CONSTANT;
                                }
{VOID}		return Parser::VOID;
{INT}		return Parser::INT;
{FLOAT}		return Parser::FLOAT;
{FOR}		return Parser::FOR;
{IF}		return Parser::IF;
{ELSE}		return Parser::ELSE;
{WHILE}		return Parser::WHILE;
{RETURN}        return Parser::RETURN;

{OR_OP}		return Parser::OR_OP;
{AND_OP}	return Parser::AND_OP;
{EQ_OP}		return Parser::EQ_OP;
{NE_OP}		return Parser::NE_OP;
{LE_OP}		return Parser::LE_OP;
{GE_OP}		return Parser::GE_OP;
{INC_OP}	return Parser::INC_OP;

{STRINGLITERAL}         {
                           Parser::d_val__ = std::string(matched());
                           return Parser::STRING_LITERAL;
                        }
{identifier}		{
                           Parser::d_val__ = std::string(matched());
                           return Parser::IDENTIFIER;
                        }

{OTHER}		return matched()[0];
