(Block [(Return (Id "b"))])
(Block [(Ass (Id "l") ((Id "printf") (Id "d") (StringConst "aditya") ))
	(Ass (ArrayRef (Id "a") (IntConst 3) (IntConst 5) (IntConst 6)) (TO_INT (Id "d")))
	(For (Assign_exp (Id "d") (TO_FLOAT (Id "l")))
	     (Assign_exp (Id "l") (TO_INT (Id "d")))
	     (PP (Id "i"))
	     (Block [(Empty)]))
	(Ass (Id "l") (TO_INT (Plus_Float (Id "d") (TO_FLOAT (Id "l")))))
	(Ass (Id "l") (TO_INT (Id "d")))
	(Ass (Id "l") ((Id "func") (Id "l") ))
	((Id "func") (Id "l") )
	((Id "printf") (StringConst "ok") )
	(Ass (Id "l") ((Id "printf") ))
	((Id "printf") )])
Local symbol-table for function: func
	b 	INT Offset: 4

Local symbol-table for function: main
	a 	(array 4, (array 5, (array 6, INT ))) Offset: -12
	d 	FLOAT Offset: -4
	e 	FLOAT Offset: -8
	i 	INT Offset: 4
	j 	INT Offset: 8
	l 	INT Offset: -492

