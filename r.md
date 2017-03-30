#Rules


##Const.
G,H,E/k -> G,H,E/k
> Whatever k may be used for, k won't change its value.

##Loc. Var
G,H,E/x -> G,H,E/E(x)
if x is local,
> then E(x) exists in E's context.
>> Whatever happens to x stays in the Environment E and does not affect any other env. Value retrieved in E, nowhere else. If E(x) doesn't exist, context error.

##Glob. Var

G,H,E/x -> G,H,E/G(x)
-> if x is Global...
>Then G(x) exists.
>> Whatever happens to x affects G(x). Its value should be retrieved in G, not in E. If it occurs, there is a context error (G(x) does not exist).

##Unary op.
G,H,E/e -> G',H',E'/v |- G,H,E/uop e->G,H,E/[[uop]](e)
`Not B  ->  G,H,E/Not <Not B>->G,H,E/[[Not]](<Not B>)`
-> The expression 'e' is a correct unary operation ii :
>Case Not : It is a boolean result. Should appear as (Not Boolean). If a non-boolean value appears, file is not correctly typed.
>Case - : It is an Integer result. Should appear as (- Integer). If a non-integer value is used, file is not correctly typed.

##Bin op.
###Case Pl Mo Mu : (Z/nZ)^2 -> (Z/nZ)
Is an integer result from 2-integer calculus. If one of the factors is a boolean, type error. Same goes if value is stocked in Boolean var.
###Case Lt Eq : (Z/nZ)^2 -> B
Is a boolean result from 2-integer calculus. If one of the factors is a boolean, type error. Same goes if the value tries to get stocked in an Integer.
###Case Or And : (B)^2 -> B
Is a boolean result from 2-boolean calculus. If it involves Integer, albeit at the left or right, type error.

##Array :
###-Read:
>  Element is within Array's maximum size. Else : outofbounds
>  e2 results in Integer value. Else: Index error
>  Values should be retrievable in Heap. Else : Alloc error.

###-Alloc:
>  Should affect Heap size, in Array's definition domain. Else: Alloc error.
>  If array is Local, then writing should affect Local domain. Else: context error.

##Function call :
>Correct amount of args Else : too few args
>Correct types Else : type error.
>Creates local env Else : context error.
>Gets data from local env and global env. Else : context conflict
>Any same-name var from global is ignored in local env, prioritizing Local var.
>What occurs in functions only affect function's environment (G, H, E). else : context conflict
>Works on local environment only. Can read global. Else : context conflict

##Commands

###Sequence.
>let S be (G,H,E)
>Command1 on S will create S1
>Command2 on S1 will create S2
>Command1 Se C2 on S will create S2.
>> Else: Context error.

###Aff Loc. Var
>let S be G,H,E.
> expr on S will create S1 = G',H',E' containing v,the var containing <result of e>. with value in domain(E')
>S/x Af e -> G'H'E'[x->v]
>let x be varname
>v := e(x)
>if x in (G,H,E) and in domain(E'), x:=v
> Check for TypeError (Int != Bool, the other way around is possible with only 0 and 1 converted to T/F)
>> If x not found in the environments readable from our scope, Non-defined var Error.
>> Depends on expression's definition. If Error on expression, returns error from expression.

###Aff Global var
>let S be G,H,E.
> S/e -> G'H'E', E'/v with x in Global Env. If not, just check Local variable affectation & return its status.
> S/x Af e -> G'[x->e(x)],H',E'
> let x be varname
> v := e(x)
> if x in (G,H,E) and in domain(G') strictly, x:=v
> Check for TypeError (same as Loc.)
>> If x found in another environment than Global and currently in global, Non-defined var error.
>> Depends on expression's definition. If error on expression, return error from expression.
