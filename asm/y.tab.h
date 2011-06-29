
typedef union 
{
	Inst*	inst;
	Addr*	addr;
	vlong	ival;
	double	fval;
	String*	string;
	Sym*	sym;
	List*	list;
}	YYSTYPE;
extern	YYSTYPE	yylval;
#define	TOKI0	57346
#define	TOKI1	57347
#define	TOKI2	57348
#define	TOKI3	57349
#define	TCONST	57350
#define	TOKSB	57351
#define	TOKFP	57352
#define	TOKHEAP	57353
#define	TOKDB	57354
#define	TOKDW	57355
#define	TOKDL	57356
#define	TOKDF	57357
#define	TOKDS	57358
#define	TOKVAR	57359
#define	TOKEXT	57360
#define	TOKMOD	57361
#define	TOKLINK	57362
#define	TOKENTRY	57363
#define	TOKARRAY	57364
#define	TOKINDIR	57365
#define	TOKAPOP	57366
#define	TOKLDTS	57367
#define	TOKEXCS	57368
#define	TOKEXC	57369
#define	TOKETAB	57370
#define	TOKSRC	57371
#define	TID	57372
#define	TFCONST	57373
#define	TSTRING	57374
