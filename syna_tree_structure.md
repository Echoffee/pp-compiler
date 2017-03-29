
**.ROOT**
- Global variables
	- (Empty/Branches)
		- VDEF
			- NEW_VAR
			- TYPE 
- Functions/Procedures
	- (Empty/Branches)
		- PBODY/FBODY
			- PDEF/FDEF	**Context change**
				- (Empty/Branches)
					- VDEF
						- NEW_VAR
						- TYPE
				- (TYPE)
			- (Empty/Branches)
				- VDEF
					- NEW_VAR
					- TYPE
			- C
- C

##############################################################################

Node types :
NEMPTY : Useless node, should only be used on syntax analyser
NROOT : First node to be executed by syna_execute()
NOPI : Operation on integers
NOPB : Operation on boolean
NPBA : Block node (ie : {}()[])
NVALUE : Hardcoded value (int/bool)
NVAR : Variable (any type)
NNVAR :
NARRAY : Array node, still not sure
NBRANCH : Split node (ie: ;)
NITE : IfThenElse node
NWD : WhileDo node
NAAF : Value affectation to an Array
NVAF : Value affectation to a Variable
NSKIP : Skip instruction
NEXPR : Expression node (general)
NVDEF : Variable definition
NTYPE : Type definition (1d+)
NPDEF : Procedure definition (line)
NFDEF : Function definition (line)
NPBODY : Procedure ensemble (def + code)
NFBODY : Function ensemble (def + code)
NFPCALL : Function/Procedure call
NNA : New array creation