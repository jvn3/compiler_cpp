Built a compiler using the language C++ and the following grammar:-

Prog := Slist
Slist := SC { Slist } | Stmt SC { Slist }
Stmt := IfStmt | PrintStmt | LetStmt | LoopStmt
IfStmt := IF Expr BEGIN Slist END
LetStmt := LET ID Expr
LoopStmt := LOOP Expr BEGIN Slist END
PrintStmt := PRINT Expr
Expr := Prod { (PLUS | MINUS) Prod }
Prod := Rev { (STAR | SLASH) Rev }
Rev := BANG Rev | PRIMARY
Rev := BANG Rev | PRIMARY

