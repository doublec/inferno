
typedef union 	{
	Sym	*sym;
	long	lval;
	double	dval;
	char	sval[8];
	Addr	addr;
	Gen	gen;
	Gen2	gen2;
}	YYSTYPE;
extern	YYSTYPE	yylval;
#define	LTYPE1	57346
#define	LTYPE2	57347
#define	LTYPE3	57348
#define	LTYPE4	57349
#define	LTYPE5	57350
#define	LTYPE6	57351
#define	LTYPE7	57352
#define	LTYPE8	57353
#define	LTYPE9	57354
#define	LTYPEA	57355
#define	LTYPEB	57356
#define	LCONST	57357
#define	LSP	57358
#define	LSB	57359
#define	LFP	57360
#define	LPC	57361
#define	LTOS	57362
#define	LAREG	57363
#define	LDREG	57364
#define	LFREG	57365
#define	LWID	57366
#define	LFCONST	57367
#define	LSCONST	57368
#define	LNAME	57369
#define	LLAB	57370
#define	LVAR	57371
