var X1 : integer

defun foo(X: integer):integer
	if X = 0
	then foo := 1
	else foo := X + foo(X - 1)
	
X1 := foo(4)