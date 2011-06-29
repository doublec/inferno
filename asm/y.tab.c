
#line	2	"asm.y"
#include "asm.h"
union {
	uvlong	l;
	double	d;
} u;

#line	9	"asm.y"
typedef union 
{
	Inst*	inst;
	Addr*	addr;
	vlong	ival;
	double	fval;
	String*	string;
	Sym*	sym;
	List*	list;
} YYSTYPE;
extern	int	yyerrflag;
#ifndef	YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE	yylval;
YYSTYPE	yyval;
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
#define YYEOFCODE 1
#define YYERRCODE 2
short	yyexca[] =
{-1, 1,
	1, -1,
	-2, 0,
-1, 61,
	4, 54,
	5, 54,
	6, 54,
	7, 54,
	8, 54,
	9, 54,
	10, 54,
	11, 54,
	12, 54,
	13, 54,
	46, 54,
	-2, 46,
-1, 140,
	44, 44,
	-2, 50,
-1, 159,
	44, 43,
	-2, 45,
};
#define	YYNPROD	70
#define	YYPRIVATE 57344
#define	YYLAST	561
short	yyact[] =
{
  64,  59, 107,  65, 162, 161,  31, 160, 158,  34,
  42,  43,  44,  45, 156,  47,  48,  33,  50,  51,
  52,  30,  32,  54,  55, 148,  39,  38,  63,  66,
  67, 105, 100,  70,  99,  36,  98,  96,  90,  69,
  57, 172,  85,  81,  80,  79,  77,  78,  72,  73,
  74,  75,  76, 165, 163, 126, 151,  61,  58,  53,
  49, 101,  60,  41, 103,  40,  46, 102, 143, 144,
 106,  56, 108, 109, 110, 111, 112, 113, 153, 152,
 116, 117, 118,   7, 115, 114, 119, 108, 108, 120,
 121, 127, 128, 129, 130,   6, 132, 133, 134, 135,
 136, 131, 137,   1, 140, 103,  35, 145, 142, 146,
  29,  28,  27,  26,  68, 149, 150,   5,   8,   9,
  10,  11,  12,  13,  14,  16,  15,  17,  18,  19,
  20,  21,  22,  23,  24,  25,   4,  74,  75,  76,
 159,  62, 138, 125,   2,  82,  83,  84,   3, 164,
   0, 122,  29,  28,  27,  26, 166, 167, 168,   0,
 169,  81,  80,  79,  77,  78,  72,  73,  74,  75,
  76,   0, 173, 124, 123, 175,   0, 177,  81,  80,
  79,  77,  78,  72,  73,  74,  75,  76,  39,  38,
   0,   0,  39,  38,  63,   0,   0,  36, 143, 144,
   0,  36,   0, 141,  81,  80,  79,  77,  78,  72,
  73,  74,  75,  76,  72,  73,  74,  75,  76,  37,
 104,   0,   0,  61,   0,  41,   0,  40, 139,  41,
   0,  40,  81,  80,  79,  77,  78,  72,  73,  74,
  75,  76,   0,   0, 176,  81,  80,  79,  77,  78,
  72,  73,  74,  75,  76,  81,  80,  79,  77,  78,
  72,  73,  74,  75,  76,  77,  78,  72,  73,  74,
  75,  76, 174,  81,  80,  79,  77,  78,  72,  73,
  74,  75,  76,   0,   0, 171,  80,  79,  77,  78,
  72,  73,  74,  75,  76, 170,  81,  80,  79,  77,
  78,  72,  73,  74,  75,  76,   0,   0,   0,   0,
   0,   0,   0, 157,  81,  80,  79,  77,  78,  72,
  73,  74,  75,  76,  81,  80,  79,  77,  78,  72,
  73,  74,  75,  76,   0,   0, 155,  81,  80,  79,
  77,  78,  72,  73,  74,  75,  76,   0,   0,   0,
   0,   0,   0,   0, 154,  79,  77,  78,  72,  73,
  74,  75,  76,   0, 147,  81,  80,  79,  77,  78,
  72,  73,  74,  75,  76,   0,   0,  97,  81,  80,
  79,  77,  78,  72,  73,  74,  75,  76,  81,  80,
  79,  77,  78,  72,  73,  74,  75,  76,   0,   0,
   0,   0,   0,   0,   0,  95,  81,  80,  79,  77,
  78,  72,  73,  74,  75,  76,   0,   0,  94,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  93,  81,
  80,  79,  77,  78,  72,  73,  74,  75,  76,   0,
   0,   0,   0,   0,   0,   0,  92,  81,  80,  79,
  77,  78,  72,  73,  74,  75,  76,  81,  80,  79,
  77,  78,  72,  73,  74,  75,  76,   0,   0,  91,
  81,  80,  79,  77,  78,  72,  73,  74,  75,  76,
   0,   0,   0,   0,   0,   0,   0,  89,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  88,  81,  80,
  79,  77,  78,  72,  73,  74,  75,  76,   0,   0,
  87,  81,  80,  79,  77,  78,  72,  73,  74,  75,
  76,  39,  38,   0,   0,   0,   0,   0,   0,   0,
  36,   0,   0,   0,   0,   0,   0,   0,  86,  81,
  80,  79,  77,  78,  72,  73,  74,  75,  76,   0,
   0,  71,  37,   0,   0,   0,   0,   0,  41,   0,
  40
};
short	yypact[] =
{
-1000,-1000,  96,-1000, -22, -23,-1000,-1000, 512, 512,
 512, 512, 512,  26, 512, 512,  20, 512, 512, 512,
-1000,  19, 512, 512,  29,  16,  17,  17,  17,-1000,
 138,  -5, 512,-1000, 507,-1000,-1000,-1000, 512, 512,
 512, 512, 494, 466, 453, 443,  -6, 425, 402,-1000,
 384, 374, 361,  -7, 535, 333,  -8, -10,-1000, -12,
 512,-1000,-1000, 512, 174,-1000, -13,-1000,-1000, 512,
 535, 512, 512, 512, 512, 512, 512,  78,  76, 512,
 512, 512,-1000,-1000,-1000,  39, 512, 512, 133,  13,
 512, 512, 512, 512, -23, 512, 512, 512, 512, 512,
 183, 535,-1000, 157, 179,  17, 320, -19, 535, 126,
 126,-1000,-1000,-1000, 512, 512, 258, 349, 281,-1000,
 -19, -19,-1000,-1000,-1000,  38,-1000, 535, 310, 292,
 535, -30, 535, 535, 269, 535, 535,-1000, -36, 512,
-1000,  49, -40, -42, -43,-1000,-1000,  12, 512, 205,
 205,-1000,-1000,-1000,  11, 512, 512, 512,  17, 535,
-1000,-1000,-1000,-1000, 535,-1000, 251, 535, 241,-1000,
  -1, 512,-1000, 228, 512, 200, 512, 535
};
short	yypgo[] =
{
   0, 148, 144,  83, 106,   0,   6,   1, 142, 141,
   3,   2, 109, 103,  95
};
short	yyr1[] =
{
   0,  13,   2,   2,   1,   1,   1,   1,   6,   6,
  12,  12,  11,  11,   3,   3,   3,   3,   3,  14,
  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,
  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,
  14,  14,  14,   8,   8,   7,   7,   7,   9,   9,
   9,  10,  10,   4,   4,   4,   4,   4,   4,   5,
   5,   5,   5,   5,   5,   5,   5,   5,   5,   5
};
short	yyr2[] =
{
   0,   1,   0,   2,   3,   5,   1,   1,   2,   1,
   0,   2,   1,   3,   4,   6,   4,   2,   1,   4,
   4,   4,   4,   4,   4,   5,   5,   5,   4,   4,
   6,   8,   2,   4,   6,   4,   1,   4,   2,  12,
   4,   4,   2,   2,   1,   2,   1,   1,   2,   4,
   1,   4,   4,   1,   1,   2,   2,   2,   3,   1,
   3,   3,   3,   3,   3,   4,   4,   3,   3,   3
};
short	yychk[] =
{
-1000, -13,  -2,  -1,  40,  21, -14,  -3,  22,  23,
  24,  25,  26,  27,  28,  30,  29,  31,  32,  33,
  34,  35,  36,  37,  38,  39,  17,  16,  15,  14,
  43,  -6,  45,  40,  -5,  -4,  18,  40,  10,   9,
  48,  46,  -5,  -5,  -5,  -5,  40,  -5,  -5,  40,
  -5,  -5,  -5,  40,  -5,  -5,  42,  11,  42,  -7,
  45,  40,  -9,  11,  -5, -10,  -7,  -7,  -3,  44,
  -5,  44,   9,  10,  11,  12,  13,   7,   8,   6,
   5,   4,  -4,  -4,  -4,  -5,  44,  44,  44,  44,
  44,  44,  44,  44,  44,  44,  44,  44,  44,  44,
  44,  -5, -10,  -5,  46,  44,  -5, -11,  -5,  -5,
  -5,  -5,  -5,  -5,   7,   8,  -5,  -5,  -5,  47,
 -11, -11,  18,  41,  40,  10,  42,  -5,  -5,  -5,
  -5,  -6,  -5,  -5,  -5,  -5,  -5,  -7,  -8,  45,
 -10,  46, -10,  19,  20,  -7, -12,  44,  44,  -5,
  -5,  18,  41,  40,  44,  44,  44,  44,  44,  -5,
  47,  47,  47,  42,  -5,  42,  -5,  -5,  -5,  -7,
  44,  44,  42,  -5,  44,  -5,  44,  -5
};
short	yydef[] =
{
   2,  -2,   1,   3,   0,   0,   6,   7,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  36,   0,   0,   0,   0,   0,   0,   0,   0,  18,
   0,   0,   0,   9,   0,  59,  53,  54,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  32,
   0,   0,   0,   0,  38,   0,   0,   0,  42,   0,
   0,  -2,  47,   0,   0,  50,   0,  17,   4,   0,
   8,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  55,  56,  57,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  45,  48,   0,   0,   0,  10,  19,  12,  60,
  61,  62,  63,  64,   0,   0,  67,  68,  69,  58,
  20,  21,  22,  23,  24,   0,  28,  29,   0,   0,
  33,   0,  35,  37,   0,  40,  41,  14,   0,   0,
  -2,   0,   0,   0,   0,  16,   5,   0,   0,  65,
  66,  25,  26,  27,   0,   0,   0,   0,   0,  -2,
  49,  51,  52,  11,  13,  30,   0,  34,   0,  15,
   0,   0,  31,   0,   0,   0,   0,  39
};
short	yytok1[] =
{
   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  45,  13,   6,   0,
  46,  47,  11,   9,  44,  10,   0,  12,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  43,   0,
   7,   0,   8,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   5,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   4,   0,  48
};
short	yytok2[] =
{
   2,   3,  14,  15,  16,  17,  18,  19,  20,  21,
  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,
  42
};
long	yytok3[] =
{
   0
};
#define YYFLAG 		-1000
#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0

#ifdef	yydebug
#include	"y.debug"
#else
#define	yydebug		0
char*	yytoknames[1];		/* for debugging */
char*	yystates[1];		/* for debugging */
#endif

/*	parser for yacc output	*/

int	yynerrs = 0;		/* number of errors */
int	yyerrflag = 0;		/* error recovery flag */

extern	int	fprint(int, char*, ...);
extern	int	sprint(char*, char*, ...);

char*
yytokname(int yyc)
{
	static char x[16];

	if(yyc > 0 && yyc <= sizeof(yytoknames)/sizeof(yytoknames[0]))
	if(yytoknames[yyc-1])
		return yytoknames[yyc-1];
	sprint(x, "<%d>", yyc);
	return x;
}

char*
yystatname(int yys)
{
	static char x[16];

	if(yys >= 0 && yys < sizeof(yystates)/sizeof(yystates[0]))
	if(yystates[yys])
		return yystates[yys];
	sprint(x, "<%d>\n", yys);
	return x;
}

long
yylex1(void)
{
	long yychar;
	long *t3p;
	int c;

	yychar = yylex();
	if(yychar <= 0) {
		c = yytok1[0];
		goto out;
	}
	if(yychar < sizeof(yytok1)/sizeof(yytok1[0])) {
		c = yytok1[yychar];
		goto out;
	}
	if(yychar >= YYPRIVATE)
		if(yychar < YYPRIVATE+sizeof(yytok2)/sizeof(yytok2[0])) {
			c = yytok2[yychar-YYPRIVATE];
			goto out;
		}
	for(t3p=yytok3;; t3p+=2) {
		c = t3p[0];
		if(c == yychar) {
			c = t3p[1];
			goto out;
		}
		if(c == 0)
			break;
	}
	c = 0;

out:
	if(c == 0)
		c = yytok2[1];	/* unknown char */
	if(yydebug >= 3)
		fprint(2, "lex %.4lux %s\n", yychar, yytokname(c));
	return c;
}

int
yyparse(void)
{
	struct
	{
		YYSTYPE	yyv;
		int	yys;
	} yys[YYMAXDEPTH], *yyp, *yypt;
	short *yyxi;
	int yyj, yym, yystate, yyn, yyg;
	long yychar;
	YYSTYPE save1, save2;
	int save3, save4;

	save1 = yylval;
	save2 = yyval;
	save3 = yynerrs;
	save4 = yyerrflag;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyp = &yys[-1];
	goto yystack;

ret0:
	yyn = 0;
	goto ret;

ret1:
	yyn = 1;
	goto ret;

ret:
	yylval = save1;
	yyval = save2;
	yynerrs = save3;
	yyerrflag = save4;
	return yyn;

yystack:
	/* put a state and value onto the stack */
	if(yydebug >= 4)
		fprint(2, "char %s in %s", yytokname(yychar), yystatname(yystate));

	yyp++;
	if(yyp >= &yys[YYMAXDEPTH]) {
		yyerror("yacc stack overflow");
		goto ret1;
	}
	yyp->yys = yystate;
	yyp->yyv = yyval;

yynewstate:
	yyn = yypact[yystate];
	if(yyn <= YYFLAG)
		goto yydefault; /* simple state */
	if(yychar < 0)
		yychar = yylex1();
	yyn += yychar;
	if(yyn < 0 || yyn >= YYLAST)
		goto yydefault;
	yyn = yyact[yyn];
	if(yychk[yyn] == yychar) { /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if(yyerrflag > 0)
			yyerrflag--;
		goto yystack;
	}

yydefault:
	/* default state action */
	yyn = yydef[yystate];
	if(yyn == -2) {
		if(yychar < 0)
			yychar = yylex1();

		/* look through exception table */
		for(yyxi=yyexca;; yyxi+=2)
			if(yyxi[0] == -1 && yyxi[1] == yystate)
				break;
		for(yyxi += 2;; yyxi += 2) {
			yyn = yyxi[0];
			if(yyn < 0 || yyn == yychar)
				break;
		}
		yyn = yyxi[1];
		if(yyn < 0)
			goto ret0;
	}
	if(yyn == 0) {
		/* error ... attempt to resume parsing */
		switch(yyerrflag) {
		case 0:   /* brand new error */
			yyerror("syntax error");
			yynerrs++;
			if(yydebug >= 1) {
				fprint(2, "%s", yystatname(yystate));
				fprint(2, "saw %s\n", yytokname(yychar));
			}

		case 1:
		case 2: /* incompletely recovered error ... try again */
			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */
			while(yyp >= yys) {
				yyn = yypact[yyp->yys] + YYERRCODE;
				if(yyn >= 0 && yyn < YYLAST) {
					yystate = yyact[yyn];  /* simulate a shift of "error" */
					if(yychk[yystate] == YYERRCODE)
						goto yystack;
				}

				/* the current yyp has no shift onn "error", pop stack */
				if(yydebug >= 2)
					fprint(2, "error recovery pops state %d, uncovers %d\n",
						yyp->yys, (yyp-1)->yys );
				yyp--;
			}
			/* there is no state on the stack with an error shift ... abort */
			goto ret1;

		case 3:  /* no shift yet; clobber input char */
			if(yydebug >= 2)
				fprint(2, "error recovery discards %s\n", yytokname(yychar));
			if(yychar == YYEOFCODE)
				goto ret1;
			yychar = -1;
			goto yynewstate;   /* try again in the same state */
		}
	}

	/* reduction by production yyn */
	if(yydebug >= 2)
		fprint(2, "reduce %d in:\n\t%s", yyn, yystatname(yystate));

	yypt = yyp;
	yyp -= yyr2[yyn];
	yyval = (yyp+1)->yyv;
	yym = yyn;

	/* consult goto table to find next state */
	yyn = yyr1[yyn];
	yyg = yypgo[yyn];
	yyj = yyg + yyp->yys + 1;

	if(yyj >= YYLAST || yychk[yystate=yyact[yyj]] != -yyn)
		yystate = yyact[yyg];
	switch(yym) {
		
case 1:
#line	41	"asm.y"
{
		assem(yypt[-0].yyv.inst);
	} break;
case 2:
#line	47	"asm.y"
{ yyval.inst = nil; } break;
case 3:
#line	49	"asm.y"
{
		if(yypt[-0].yyv.inst != nil) {
			yypt[-0].yyv.inst->link = yypt[-1].yyv.inst;
			yyval.inst = yypt[-0].yyv.inst;
		}
		else
			yyval.inst = yypt[-1].yyv.inst;
	} break;
case 4:
#line	60	"asm.y"
{
		yypt[-0].yyv.inst->sym = yypt[-2].yyv.sym;
		yyval.inst = yypt[-0].yyv.inst;
	} break;
case 5:
#line	65	"asm.y"
{
		heap(yypt[-3].yyv.ival, yypt[-1].yyv.ival, yypt[-0].yyv.string);
		yyval.inst = nil;
	} break;
case 6:
#line	70	"asm.y"
{
		yyval.inst = nil;
	} break;
case 8:
#line	77	"asm.y"
{
		yyval.ival = yypt[-0].yyv.ival;
	} break;
case 9:
#line	81	"asm.y"
{
		yypt[-0].yyv.sym->value = heapid++;
		yyval.ival = yypt[-0].yyv.sym->value;
	} break;
case 10:
#line	88	"asm.y"
{ yyval.string = nil; } break;
case 11:
#line	90	"asm.y"
{
		yyval.string = yypt[-0].yyv.string;
	} break;
case 12:
#line	96	"asm.y"
{
		yyval.list = newi(yypt[-0].yyv.ival, nil);
	} break;
case 13:
#line	100	"asm.y"
{
		yyval.list = newi(yypt[-0].yyv.ival, yypt[-2].yyv.list);
	} break;
case 14:
#line	106	"asm.y"
{
		yyval.inst = ai(yypt[-3].yyv.ival);
		yyval.inst->src = yypt[-2].yyv.addr;
		yyval.inst->dst = yypt[-0].yyv.addr;
	} break;
case 15:
#line	112	"asm.y"
{
		yyval.inst = ai(yypt[-5].yyv.ival);
		yyval.inst->src = yypt[-4].yyv.addr;
		yyval.inst->reg = yypt[-2].yyv.addr;
		yyval.inst->dst = yypt[-0].yyv.addr;
	} break;
case 16:
#line	119	"asm.y"
{
		yyval.inst = ai(yypt[-3].yyv.ival);
		yyval.inst->src = yypt[-2].yyv.addr;
		yyval.inst->dst = yypt[-0].yyv.addr;
	} break;
case 17:
#line	125	"asm.y"
{
		yyval.inst = ai(yypt[-1].yyv.ival);
		yyval.inst->dst = yypt[-0].yyv.addr;
	} break;
case 18:
#line	130	"asm.y"
{
		yyval.inst = ai(yypt[-0].yyv.ival);
	} break;
case 19:
#line	136	"asm.y"
{
		data(DEFB, yypt[-2].yyv.ival, yypt[-0].yyv.list);
	} break;
case 20:
#line	140	"asm.y"
{
		data(DEFW, yypt[-2].yyv.ival, yypt[-0].yyv.list);
	} break;
case 21:
#line	144	"asm.y"
{
		data(DEFL, yypt[-2].yyv.ival, yypt[-0].yyv.list);
	} break;
case 22:
#line	148	"asm.y"
{
		data(DEFF, yypt[-2].yyv.ival, newi(dtocanon((double)yypt[-0].yyv.ival), nil));
	} break;
case 23:
#line	152	"asm.y"
{
		data(DEFF, yypt[-2].yyv.ival, newi(dtocanon(yypt[-0].yyv.fval), nil));
	} break;
case 24:
#line	156	"asm.y"
{
		if(strcmp(yypt[-0].yyv.sym->name, "Inf") == 0 || strcmp(yypt[-0].yyv.sym->name, "Infinity") == 0) {
			u.l = (uvlong)0x7ff00000<<32;
			data(DEFF, yypt[-2].yyv.ival, newi(dtocanon(u.d), nil));
		} else if(strcmp(yypt[-0].yyv.sym->name, "NaN") == 0) {
			u.l = ((uvlong)0x7fffffff<<32) | (uvlong)0xffffffffUL;
			data(DEFF, yypt[-2].yyv.ival, newi(dtocanon(u.d), nil));
		} else
			diag("bad value for real: %s", yypt[-0].yyv.sym->name);
	} break;
case 25:
#line	167	"asm.y"
{
		data(DEFF, yypt[-3].yyv.ival, newi(dtocanon(-(double)yypt[-0].yyv.ival), nil));
	} break;
case 26:
#line	171	"asm.y"
{
		data(DEFF, yypt[-3].yyv.ival, newi(dtocanon(-yypt[-0].yyv.fval), nil));
	} break;
case 27:
#line	175	"asm.y"
{
		if(strcmp(yypt[-0].yyv.sym->name, "Inf") == 0 || strcmp(yypt[-0].yyv.sym->name, "Infinity") == 0) {
			u.l = (uvlong)0xfff00000<<32;
			data(DEFF, yypt[-3].yyv.ival, newi(dtocanon(u.d), nil));
		} else
			diag("bad value for real: %s", yypt[-0].yyv.sym->name);
	} break;
case 28:
#line	183	"asm.y"
{
		data(DEFS, yypt[-2].yyv.ival, news(yypt[-0].yyv.string, nil));
	} break;
case 29:
#line	187	"asm.y"
{
		if(yypt[-2].yyv.sym->ds != 0)
			diag("%s declared twice", yypt[-2].yyv.sym->name);
		yypt[-2].yyv.sym->ds = yypt[-0].yyv.ival;
		yypt[-2].yyv.sym->value = dseg;
		dseg += yypt[-0].yyv.ival;
	} break;
case 30:
#line	195	"asm.y"
{
		ext(yypt[-4].yyv.ival, yypt[-2].yyv.ival, yypt[-0].yyv.string);
	} break;
case 31:
#line	199	"asm.y"
{
		mklink(yypt[-6].yyv.ival, yypt[-4].yyv.ival, yypt[-2].yyv.ival, yypt[-0].yyv.string);
	} break;
case 32:
#line	203	"asm.y"
{
		if(module != nil)
			diag("this module already defined as %s", yypt[-0].yyv.sym->name);
		else
			module = yypt[-0].yyv.sym;
	} break;
case 33:
#line	210	"asm.y"
{
		if(pcentry >= 0)
			diag("this module already has entry point %d, %d" , pcentry, dentry);
		pcentry = yypt[-2].yyv.ival;
		dentry = yypt[-0].yyv.ival;
	} break;
case 34:
#line	217	"asm.y"
{
		data(DEFA, yypt[-4].yyv.ival, newa(yypt[-2].yyv.ival, yypt[-0].yyv.ival));
	} break;
case 35:
#line	221	"asm.y"
{
		data(DIND, yypt[-2].yyv.ival, newa(yypt[-0].yyv.ival, 0));
	} break;
case 36:
#line	225	"asm.y"
{
		data(DAPOP, 0, newa(0, 0));
	} break;
case 37:
#line	229	"asm.y"
{
		ldts(yypt[-0].yyv.ival);
	} break;
case 38:
#line	233	"asm.y"
{
		excs(yypt[-0].yyv.ival);
	} break;
case 39:
#line	237	"asm.y"
{
		exc(yypt[-10].yyv.ival, yypt[-8].yyv.ival, yypt[-6].yyv.ival, yypt[-4].yyv.ival, yypt[-2].yyv.ival, yypt[-0].yyv.ival);
	} break;
case 40:
#line	241	"asm.y"
{
		etab(yypt[-2].yyv.string, yypt[-0].yyv.ival);
	} break;
case 41:
#line	245	"asm.y"
{
		etab(nil, yypt[-0].yyv.ival);
	} break;
case 42:
#line	249	"asm.y"
{
		source(yypt[-0].yyv.string);
	} break;
case 43:
#line	255	"asm.y"
{
		yyval.addr = aa(yypt[-0].yyv.ival);
		yyval.addr->mode = AXIMM;
		if(yyval.addr->val > 0x7FFF || yyval.addr->val < -0x8000)
			diag("immediate %d too large for middle operand", yyval.addr->val);
	} break;
case 44:
#line	262	"asm.y"
{
		if(yypt[-0].yyv.addr->mode == AMP)
			yypt[-0].yyv.addr->mode = AXINM;
		else
			yypt[-0].yyv.addr->mode = AXINF;
		if(yypt[-0].yyv.addr->mode == AXINM && (ulong)yypt[-0].yyv.addr->val > 0xFFFF)
			diag("register offset %d(mp) too large", yypt[-0].yyv.addr->val);
		if(yypt[-0].yyv.addr->mode == AXINF && (ulong)yypt[-0].yyv.addr->val > 0xFFFF)
			diag("register offset %d(fp) too large", yypt[-0].yyv.addr->val);
		yyval.addr = yypt[-0].yyv.addr;
	} break;
case 45:
#line	276	"asm.y"
{
		yyval.addr = aa(yypt[-0].yyv.ival);
		yyval.addr->mode = AIMM;
	} break;
case 46:
#line	281	"asm.y"
{
		yyval.addr = aa(0);
		yyval.addr->sym = yypt[-0].yyv.sym;
	} break;
case 48:
#line	289	"asm.y"
{
		yypt[-0].yyv.addr->mode |= AIND;
		yyval.addr = yypt[-0].yyv.addr;
	} break;
case 49:
#line	294	"asm.y"
{
		yypt[-1].yyv.addr->mode |= AIND;
		if(yypt[-1].yyv.addr->val & 3)
			diag("indirect offset must be word size");
		if(yypt[-1].yyv.addr->mode == (AMP|AIND) && ((ulong)yypt[-1].yyv.addr->val > 0xFFFF || (ulong)yypt[-3].yyv.ival > 0xFFFF))
			diag("indirect offset %d(%d(mp)) too large", yypt[-3].yyv.ival, yypt[-1].yyv.addr->val);
		if(yypt[-1].yyv.addr->mode == (AFP|AIND) && ((ulong)yypt[-1].yyv.addr->val > 0xFFFF || (ulong)yypt[-3].yyv.ival > 0xFFFF))
			diag("indirect offset %d(%d(fp)) too large", yypt[-3].yyv.ival, yypt[-1].yyv.addr->val);
		yypt[-1].yyv.addr->off = yypt[-1].yyv.addr->val;
		yypt[-1].yyv.addr->val = yypt[-3].yyv.ival;
		yyval.addr = yypt[-1].yyv.addr;
	} break;
case 51:
#line	310	"asm.y"
{
		yyval.addr = aa(yypt[-3].yyv.ival);
		yyval.addr->mode = AMP;
	} break;
case 52:
#line	315	"asm.y"
{
		yyval.addr = aa(yypt[-3].yyv.ival);
		yyval.addr->mode = AFP;
	} break;
case 54:
#line	323	"asm.y"
{
		yyval.ival = yypt[-0].yyv.sym->value;
	} break;
case 55:
#line	327	"asm.y"
{
		yyval.ival = -yypt[-0].yyv.ival;
	} break;
case 56:
#line	331	"asm.y"
{
		yyval.ival = yypt[-0].yyv.ival;
	} break;
case 57:
#line	335	"asm.y"
{
		yyval.ival = ~yypt[-0].yyv.ival;
	} break;
case 58:
#line	339	"asm.y"
{
		yyval.ival = yypt[-1].yyv.ival;
	} break;
case 60:
#line	346	"asm.y"
{
		yyval.ival = yypt[-2].yyv.ival + yypt[-0].yyv.ival;
	} break;
case 61:
#line	350	"asm.y"
{
		yyval.ival = yypt[-2].yyv.ival - yypt[-0].yyv.ival;
	} break;
case 62:
#line	354	"asm.y"
{
		yyval.ival = yypt[-2].yyv.ival * yypt[-0].yyv.ival;
	} break;
case 63:
#line	358	"asm.y"
{
		yyval.ival = yypt[-2].yyv.ival / yypt[-0].yyv.ival;
	} break;
case 64:
#line	362	"asm.y"
{
		yyval.ival = yypt[-2].yyv.ival % yypt[-0].yyv.ival;
	} break;
case 65:
#line	366	"asm.y"
{
		yyval.ival = yypt[-3].yyv.ival << yypt[-0].yyv.ival;
	} break;
case 66:
#line	370	"asm.y"
{
		yyval.ival = yypt[-3].yyv.ival >> yypt[-0].yyv.ival;
	} break;
case 67:
#line	374	"asm.y"
{
		yyval.ival = yypt[-2].yyv.ival & yypt[-0].yyv.ival;
	} break;
case 68:
#line	378	"asm.y"
{
		yyval.ival = yypt[-2].yyv.ival ^ yypt[-0].yyv.ival;
	} break;
case 69:
#line	382	"asm.y"
{
		yyval.ival = yypt[-2].yyv.ival | yypt[-0].yyv.ival;
	} break;
	}
	goto yystack;  /* stack new state and value */
}
