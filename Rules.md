#Rules


##Const.
$\overline{G,H,E/k -> G,H,E/k}$
> Whatever k may be used for, k won't change its value.

##Loc. Var
$\overline{G,H,E/x \rightarrow G,H,E/E(x)}$

if x is local,
> then E(x) exists in E's context.
>
>> Whatever happens to x stays in the Environment E and does not affect any other env. Value retrieved in E, nowhere else. If E(x) doesn't exist, context error.

##Glob. Var

$\overline{G,H,E/x \rightarrow G,H,E/G(x)}$

-> if x is Global...
>Then G(x) exists.
>
>> Whatever happens to x affects G(x). Its value should be retrieved in G, not in E. If it occurs, there is a context error (G(x) does not exist).

##Unary op.
$\cfrac{G,H,E/_e \rightarrow G',H',E'/_v}{G,H,E/_{uop} \ e\rightarrow G,H,E/_{[[uop]](e)}}$

`Not B  ->  G,H,E/Not <Not B>->G,H,E/[[Not]](<Not B>)`
-> The expression 'e' is a correct unary operation ii :

>Case Not : It is a boolean result. Should appear as (Not Boolean). If a non-boolean value appears, file is not correctly typed.
>
>Case - : It is an Integer result. Should appear as (- Integer). If a non-integer value is used, file is not correctly typed.

##Bin op.
###Case Pl Mo Mu : $(\mathbb{Z}/n\mathbb{Z})^2 \rightarrow (\mathbb{Z}/n\mathbb{Z})$
>Is an integer result from 2-integer calculus. If one of the factors is a boolean, type error. Same goes if value is stocked in Boolean var.

###Case Lt Eq : $(\mathbb{Z}/n\mathbb{Z})^2 \rightarrow \mathbb{B}$
>Is a boolean result from 2-integer calculus. If one of the factors is a boolean, type error. Same goes if the value tries to get stocked in an Integer.

###Case Or And : $(\mathbb{B})^2 \rightarrow \mathbb{B}$
>Is a boolean result from 2-boolean calculus. If it involves Integer, albeit at the left or right, type error.

##Array :
###-Read:
>  Element is within Array's maximum size. Else : outofbounds
>
>  e2 results in Integer value. Else: Index error
>
>  Values should be retrievable in Heap. Else : Alloc error.

###-Alloc:
>  Should affect Heap size, in Array's definition domain. Else: Alloc error.
>
>  If array is Local, then writing should affect Local domain. Else: context error.

##Function call :
>Correct amount of args Else : too few args
>
>Correct types Else : type error.
>
>Creates local env Else : context error.
>
>Gets data from local env and global env. Else : context conflict
>
>Any same-name var from global is ignored in local env, prioritizing Local var.
>
>What occurs in functions only affect function's environment (G, H, E). else : context conflict
>
>Works on local environment only. Can read global. Else : context conflict

##Commands

###Sequence.
>let $S$ be $(G,H,E)$
>
>$Command1$ on $S$ will create $S_1$
>
>$Command2$ on $S_1$ will create $S_2$
>
>$Command1$ Se $Command2$ on $S$ will create $S_2$.
>
>> Else: Context error.

###Aff Loc. Var
>let $S$ be $G,H,E$.
>
> $expr$ on $S$ will create $S1$ = $G',H',E'$ containing $v$,the var containing $<result\ of\ e>$. with value in $domain(E')$
>
>$S/_x$ Af $e$ $\rightarrow G'H'E'_{[x\rightarrow v]}$
>
>let $x$ be $varname$
>
>$v := e(x)$
>
>if $x \in (G,H,E)$ and $x \in domain(E')$, $x:=v$
>
> Check for TypeError (Int != Bool, the other way around is possible with only 0 and 1 converted to T/F)
>
>> If $x$ not found in the environments readable from our scope, Non-defined var Error.
>>
>> Depends on expression's definition. If Error on expression, returns error from expression.

###Aff Global var
>let $S$ be $G,H,E$.
>
> $S/_e \rightarrow G'H'E'$, $E'/_v$ with $x$ in Global Env. If not, just check Local variable affectation & return its status.
>
> $S/_x$ Af $e \rightarrow G'_{[x->e(x)]},H',E'$
>
> let $x$ be $varname$
>
> $v := e(x)$
>
> if $x \in (G,H,E)$ and in $domain(G')$ strictly, $x:=v$
>
> Check for TypeError (same as Loc.)
>
>> If $x$ found in another environment than Global and currently in global, Non-defined var error.
>>
>> Depends on expression's definition. If error on expression, return error from expression.

### While
#### $e = True$
$\cfrac{S/_e \rightarrow S'/_{true}\ S'/_c\ Se \ Wh \ e\ Do \ c \rightarrow S'' }{S/_{Wh \ e\ Do \ c} \rightarrow S''}$
> $e$ has a boolean result Else : type Error
>
> applying expression $e$ to $S$ creates a new environment $S'$ where it will run any command stated under $Do$.
>
> applying commands to $S'$ linked to a While condition creates a new environment $S''$
>
> Thus applying the While condition on $e$ directly on $S$ will create $S''$. Else, context error.

#### $e = False$
$\cfrac{S/_e \rightarrow S'/_{false}}{S/_{Wh \ e\ Do \ c} \rightarrow S'}$
> $e$ has a boolean result Else : type error
>
> applying expression $e$ to $S$ creates $S'$ where $|e| := False$, meaning that it skips the $Do$ statement
>
>Thus applying $Wh\ e\ Do\ c$ on $S$ will create the same environment $S'$ Else : context error

#### Any Way
>> While depends on Expr and Commands. Any rule applying to those ones are also commited to it.

### Conditional (If .. Then .. Else ..)
Conditionals depend on Commands and Expressions. So basically the same rule as said above applies.
#### true
$\cfrac{S/_e \rightarrow S'/_{true}\ \ \  S'/_{c_1} \rightarrow S''}{S/If_eTh_{c_1}El_{c_2} \rightarrow S''}$
>$e$ has a boolean result Else : type error
>
> $c_2$ can't apply in that case since $e = true$. If it does, then there is a **context error** and **expression definition issue**, as $e$ should *always* state $true$ and thus avoid the $Else$ part.
>
> applying $e$ to $S$ should create a new environment $S'$ with $|e| := True$, meaning that the conditional will run $Then$ and skip $Else$.
>
> applying any command to $S'$ creates $S''$
>
> thus applying $If_eTh_{c_1}El_{c_2}$ to $S$ should create the same $S''$

#### false
$\cfrac{S/_e \rightarrow S'/_{false}\ \ \  S'/_{c_2} \rightarrow S''}{S/If_eTh_{c_1}El_{c_2} \rightarrow S''}$
>$e$ has a boolean result Else : type error
>
> $c_1$ can't apply in that case since $e = false$. If it does, then there is a **context error** and **expression definition issue**, as $e$ should *always* state $false$ and thus avoid the $Then$ part.
>
> applying $e$ to $S$ should create a new environment $S'$ with $|e| := False$, meaning that the conditional will skip the $Then$ statement and run $Else$.
>
> applying any command to $S'$ creates $S''$
>
> thus applying $If_eTh_{c_1}El_{c_2}$ to $S$ should create the same $S''$


###Array - Write
Depends on variable affectation.

> Affects Heap (array of values within current interpretation)
>
> Writing to an array is restricted to the type of the current Et of the array. (example : writing w in an Array of Array of Integer $aai$ needs :
>> in the first part, an Array of Array of Integer.
>>
>> in the second part, an Array of Integer. $aai[x] := ai$
>>
>> in the last part, an Integer. $aai[x][y] := i$
>>> Anything else leads to : TypeError.
>
>
