
#line	2	"a.y"
#include "a.h"

#line	4	"a.y"
typedef union 	{
	Sym	*sym;
	long	lval;
	double	dval;
	char	sval[8];
	Addr	addr;
	Gen	gen;
	Gen2	gen2;
} YYSTYPE;
extern	int	yyerrflag;
#ifndef	YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE	yylval;
YYSTYPE	yyval;
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
#define	LFCONST	57366
#define	LSCONST	57367
#define	LNAME	57368
#define	LLAB	57369
#define	LVAR	57370
#define YYEOFCODE 1
#define YYERRCODE 2
short	yyexca[] =
{-1, 1,
	1, -1,
	-2, 0,
};
#define	YYNPROD	89
#define	YYPRIVATE 57344
#define	yydebug	1
#define	YYLAST	349
short	yyact[] =
{
  31,  55,   2, 121, 120, 119, 117, 118, 112, 113,
 114, 115, 116,  39,  60,  57,  96, 171, 126, 164,
 161,  88,  93,  78, 159, 151,  78, 150, 129,  37,
  81, 127, 165,  90,  78, 128, 131, 104,  88, 170,
  94,  95,  38, 130, 160,  77,  25,  61,  79, 158,
  40, 109, 108,  48,  80,  39,  85,  57, 103, 107,
 102,  90,  78,  27,  46,  24,  51,  53,  23,  63,
  66,  37,  71,  74,  20, 105, 106,  41, 110, 111,
  84,  83,  44,  21,  38,  22,  90, 124, 125,  61,
  78, 136,  40, 155, 154, 156,  39,  60, 169, 133,
 134,  45, 135, 100, 114, 115, 116,  82,  57, 137,
 139, 101,  37,  78,  78,  78,  78,  78,  91,  67,
  78,  78,  78, 100,  97,  38,  98,  99,  26, 152,
  61,  92, 146,  40, 122, 140, 141, 142, 143, 144,
  98,  99, 147, 148, 149,  65,  78,  78,  73, 157,
 145,  39,  32, 119, 117, 118, 112, 113, 114, 115,
 116, 167, 138, 122,   6,  39,  60,  37, 162, 163,
 132, 172,  30,  42,  43,  36,  86,  87,  44,  76,
  38,  37,  75,  47,  69,  33,   1,  29,  40,  62,
  54,  52,  58,  59,  38,  39,  32,  72,  56,  61,
  64,  68,  40, 117, 118, 112, 113, 114, 115, 116,
  70,  37, 166,  50,  34, 168,  30,  42,  43,  36,
  39,  32,  44,  35,  38, 153,  28,   0,  49,  33,
   0,  29,  40,   0,   0,   0,  37,   0,   0,   0,
   0,  30,  42,  43,  36,   0,   7,  44,   0,  38,
   0,   0,   0,   0,  33,   0,  29,  40,   9,  10,
  11,  12,  13,  14,  15,  16,  17,  18,  19,  39,
  60,   0,   0,  39,  60,  39,  60,   0,   0,   0,
   4,   3,   8,   0,   5,  37,   0,   0,   0,  37,
   0,  37,  39,  60,   0,   0,  58,  59,  38,   0,
 123,   0,  38,  61,  38,   0,  40,  61,  37,  61,
  40,   0,  40, 112, 113, 114, 115, 116,   0,   0,
   0,  38,   0,   0,   0,   0,  89,   0,   0,  40,
 121, 120, 119, 117, 118, 112, 113, 114, 115, 116,
 120, 119, 117, 118, 112, 113, 114, 115, 116
};
short	yypact[] =
{
-1000, 244,-1000,  35,  44,-1000,  28,  25,   5, 211,
  22, 186, 211, 211, 156, 211, 186, 142, 211, 211,
-1000,-1000, 264,-1000,-1000, 264,-1000,  12,-1000,  46,
 167,  -5, 283,  87,-1000,-1000,-1000,-1000,-1000, 264,
 264,-1000,-1000,-1000, 117,-1000,-1000,-1000,-1000, 211,
-1000,  99,-1000,  18,-1000,-1000, 260,  -6, 131, 131,
 264, 264,-1000,  17,-1000,-1000,  12,-1000,-1000, 211,
-1000,  10,-1000,-1000,   9, 244, 244, 326,-1000, 326,
 211,-1000,-1000,-1000,-1000, 266, 264, 264, -13,   4,
-1000, -16,-1000,  -1,-1000,-1000,  -7, 162, 264, 264,
-1000, 264,-1000,-1000,  62,-1000,-1000, 260, 211, 211,
-1000,-1000, 264, 264, 264, 264, 264, 143, 124, 264,
 264, 264,-1000,-1000,-1000,-1000, -17,-1000, -19, 120,
-1000,  67, 131,-1000,-1000,   7, -20,-1000,   2, -22,
  93,  93,-1000,-1000,-1000, 264, 264, 196, 147, 335,
-1000,-1000,-1000, -25,-1000,-1000,-1000, -11, 211,-1000,
 264, 211, 304, 304,-1000,  71,-1000,  -3,-1000, -27,
 264,-1000,-1000
};
short	yypgo[] =
{
   0,   0,  22, 226, 225, 223,  16,  77, 214,  53,
   1, 101, 128, 183, 213, 210, 197, 191, 190, 189,
 186,   2, 182, 179, 164
};
short	yyr1[] =
{
   0,  20,  20,  22,  21,  23,  21,  21,  21,  21,
  24,  24,  24,  24,  24,  24,  24,  24,  24,  24,
  24,  24,  24,  24,  24,  11,  11,  17,  17,  13,
  13,  12,  18,  18,  19,  14,  15,  16,  16,  10,
  10,  10,   9,   9,   9,   9,   9,   9,   9,   9,
   9,   9,   9,   9,   3,   3,   5,   5,   5,   8,
   8,   8,   8,   8,   7,   7,   6,   6,   6,   4,
   4,   4,   1,   1,   1,   1,   1,   1,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2
};
short	yyr2[] =
{
   0,   0,   2,   0,   4,   0,   4,   1,   2,   2,
   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   0,   1,   1,   2,   1,
   2,   3,   1,   2,   3,   5,   7,   1,   5,   4,
   2,   2,   1,   2,   2,   2,   2,   3,   3,   3,
   1,   4,   4,   1,   1,   1,   1,   1,   1,   3,
   4,   2,   3,   1,   5,   7,   0,   2,   2,   1,
   1,   1,   1,   1,   2,   2,   2,   3,   1,   3,
   3,   3,   3,   3,   4,   4,   3,   3,   3
};
short	yychk[] =
{
-1000, -20, -21,  37,  36,  40, -24,   2,  38,  14,
  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
  39,  39,  41,  40,  40,  41, -12,  -9,  -3,  45,
  30,  -1,  10,  43,  -8,  -5,  33,  25,  38,   9,
  46,  -7,  31,  32,  36, -11,  42, -13,  -9,  42,
 -14,  -9, -17,  -9, -18, -10,  42,  -1,  36,  37,
  10,  43, -19,  -9, -13, -12,  -9, -11, -13,  42,
 -15,  -9, -16, -12,  -9, -22, -23,  -2,  -1,  -2,
  42,  -1,  -7,  35,  34,  10,   9,  10,  43,  43,
  -1,  31,  44,  -2,  -1,  -1,  -6,   7,   9,  10,
  -9,  12,  42, -10,  43,  -6,  -6,  42,  42,  42,
 -21, -21,   9,  10,  11,  12,  13,   7,   8,   6,
   5,   4,  -9,  34,  -1,  -1,  31,  44,  31,  44,
  44,  43,   8,  -1,  -1,  -1,  29, -10,  -9,  -1,
  -2,  -2,  -2,  -2,  -2,   7,   8,  -2,  -2,  -2,
  44,  44,   9,  -4,  27,  26,  28,  -6,  42,  44,
  42,  42,  -2,  -2,  44,  43,  -9,  -1,  -9,  27,
  42,  44,  -1
};
short	yydef[] =
{
   1,  -2,   2,   0,   0,   7,   0,   0,   0,   0,
  25,   0,   0,   0,   0,   0,   0,  25,   0,   0,
   3,   5,   0,   8,   9,   0,  12,   0,  42,   0,
  58,  50,   0,   0,  53,  54,  55,  72,  73,   0,
   0,  63,  56,  57,  66,  13,  26,  14,  29,   0,
  15,   0,  16,  27,  17,  32,   0,   0,  66,  66,
   0,   0,  18,   0,  19,  20,  29,  21,  22,  26,
  23,   0,  24,  37,   0,   0,   0,  10,  78,  11,
   0,  43,  44,  45,  46,   0,   0,   0,   0,   0,
  74,   0,  61,   0,  75,  76,   0,   0,   0,   0,
  30,   0,  28,  33,   0,  40,  41,   0,   0,   0,
   4,   6,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  31,  47,  48,  49,   0,  62,   0,  59,
  77,   0,  66,  67,  68,   0,   0,  34,   0,  50,
  79,  80,  81,  82,  83,   0,   0,  86,  87,  88,
  60,  51,  52,   0,  69,  70,  71,   0,   0,  39,
   0,   0,  84,  85,  64,   0,  35,   0,  38,   0,
   0,  65,  36
};
short	yytok1[] =
{
   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  45,  13,   6,   0,
  43,  44,  11,   9,  42,  10,   0,  12,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  39,  40,
   7,  41,   8,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   5,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   4,   0,  46
};
short	yytok2[] =
{
   2,   3,  14,  15,  16,  17,  18,  19,  20,  21,
  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  32,  33,  34,  35,  36,  37,  38
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
		
case 3:
#line	35	"a.y"
{
		if(yypt[-1].yyv.sym->value != pc)
			yyerror("redeclaration of %s", yypt[-1].yyv.sym->name);
		yypt[-1].yyv.sym->value = pc;
	} break;
case 5:
#line	42	"a.y"
{
		yypt[-1].yyv.sym->type = LLAB;
		yypt[-1].yyv.sym->value = pc;
	} break;
case 10:
#line	53	"a.y"
{
		yypt[-2].yyv.sym->type = LVAR;
		yypt[-2].yyv.sym->value = yypt[-0].yyv.lval;
	} break;
case 11:
#line	58	"a.y"
{
		if(yypt[-2].yyv.sym->value != yypt[-0].yyv.lval)
			yyerror("redeclaration of %s", yypt[-2].yyv.sym->name);
		yypt[-2].yyv.sym->value = yypt[-0].yyv.lval;
	} break;
case 12:
#line	63	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 13:
#line	64	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 14:
#line	65	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 15:
#line	66	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 16:
#line	67	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 17:
#line	68	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 18:
#line	69	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 19:
#line	70	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 20:
#line	71	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 21:
#line	72	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 22:
#line	73	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 23:
#line	74	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 24:
#line	75	"a.y"
{ outcode(yypt[-1].yyv.lval, &yypt[-0].yyv.gen2); } break;
case 25:
#line	78	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = nullgen;
	} break;
case 26:
#line	83	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = nullgen;
	} break;
case 27:
#line	90	"a.y"
{
		yyval.gen2.from = yypt[-0].yyv.gen;
		yyval.gen2.to = nullgen;
	} break;
case 28:
#line	95	"a.y"
{
		yyval.gen2.from = yypt[-1].yyv.gen;
		yyval.gen2.to = nullgen;
	} break;
case 29:
#line	102	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 30:
#line	107	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 31:
#line	114	"a.y"
{
		yyval.gen2.from = yypt[-2].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 32:
#line	121	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 33:
#line	126	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 34:
#line	133	"a.y"
{
		yyval.gen2.from = yypt[-2].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 35:
#line	140	"a.y"
{
		yypt[-4].yyv.gen.displace = yypt[-2].yyv.lval;
		yyval.gen2.from = yypt[-4].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 36:
#line	148	"a.y"
{
		yypt[-6].yyv.gen.field = yypt[-0].yyv.lval;
		yypt[-4].yyv.gen.field = yypt[-2].yyv.lval;
		yyval.gen2.from = yypt[-6].yyv.gen;
		yyval.gen2.to = yypt[-4].yyv.gen;
	} break;
case 38:
#line	158	"a.y"
{
		yypt[-4].yyv.gen.displace = yypt[-2].yyv.lval;
		yyval.gen2.from = yypt[-4].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 39:
#line	166	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_BRANCH;
		yyval.gen.s0.offset = yypt[-3].yyv.lval + pc;
	} break;
case 40:
#line	172	"a.y"
{
		yyval.gen = nullgen;
		if(pass == 2)
			yyerror("undefined label: %s", yypt[-1].yyv.sym->name);
		yyval.gen.type = D_BRANCH;
		yyval.gen.s0.sym = yypt[-1].yyv.sym;
		yyval.gen.s0.offset = yypt[-0].yyv.lval;
	} break;
case 41:
#line	181	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_BRANCH;
		yyval.gen.s0.sym = yypt[-1].yyv.sym;
		yyval.gen.s0.offset = yypt[-1].yyv.sym->value + yypt[-0].yyv.lval;
	} break;
case 42:
#line	190	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-0].yyv.lval;
	} break;
case 43:
#line	195	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_CONST;
		yyval.gen.s0.offset = yypt[-0].yyv.lval;
	} break;
case 44:
#line	201	"a.y"
{
		yyval.gen = nullgen;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-0].yyv.addr;
		}
		if(yypt[-0].yyv.addr.type == D_AUTO || yypt[-0].yyv.addr.type == D_PARAM)
			yyerror("constant cannot be automatic: %s",
				yypt[-0].yyv.addr.sym->name);
		yyval.gen.type = yypt[-0].yyv.addr.type | I_ADDR;
	} break;
case 45:
#line	214	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_SCONST;
		memcpy(yyval.gen.sval, yypt[-0].yyv.sval, sizeof(yyval.gen.sval));
	} break;
case 46:
#line	220	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCONST;
		yyval.gen.dval = yypt[-0].yyv.dval;
	} break;
case 47:
#line	226	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCONST;
		yyval.gen.dval = -yypt[-0].yyv.dval;
	} break;
case 48:
#line	232	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_STACK;
		yyval.gen.s0.offset = yypt[-0].yyv.lval;
	} break;
case 49:
#line	238	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_STACK;
		yyval.gen.s0.offset = -yypt[-0].yyv.lval;
	} break;
case 50:
#line	244	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_CONST | I_INDIR;
		yyval.gen.s0.offset = yypt[-0].yyv.lval;
	} break;
case 51:
#line	250	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-1].yyv.lval | I_INDDEC;
	} break;
case 52:
#line	255	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-2].yyv.lval | I_INDINC;
	} break;
case 53:
#line	260	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-0].yyv.addr.type;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-0].yyv.addr;
		}
	} break;
case 59:
#line	281	"a.y"
{
		yyval.addr.type = yypt[-1].yyv.lval | I_INDIR;
		yyval.addr.sym = S;
		yyval.addr.offset = 0;
	} break;
case 60:
#line	287	"a.y"
{
		yyval.addr.type = yypt[-1].yyv.lval | I_INDIR;
		yyval.addr.sym = S;
		yyval.addr.offset = yypt[-3].yyv.lval;
	} break;
case 61:
#line	293	"a.y"
{
		yyval.addr.type = D_NONE | I_INDIR;
		yyval.addr.sym = S;
		yyval.addr.offset = 0;
	} break;
case 62:
#line	299	"a.y"
{
		yyval.addr.type = D_NONE | I_INDIR;
		yyval.addr.sym = S;
		yyval.addr.offset = yypt[-2].yyv.lval;
	} break;
case 64:
#line	308	"a.y"
{
		yyval.addr.type = yypt[-1].yyv.lval;
		yyval.addr.sym = yypt[-4].yyv.sym;
		yyval.addr.offset = yypt[-3].yyv.lval;
	} break;
case 65:
#line	314	"a.y"
{
		yyval.addr.type = D_STATIC;
		yyval.addr.sym = yypt[-6].yyv.sym;
		yyval.addr.offset = yypt[-3].yyv.lval;
	} break;
case 66:
#line	321	"a.y"
{
		yyval.lval = 0;
	} break;
case 67:
#line	325	"a.y"
{
		yyval.lval = yypt[-0].yyv.lval;
	} break;
case 68:
#line	329	"a.y"
{
		yyval.lval = -yypt[-0].yyv.lval;
	} break;
case 73:
#line	341	"a.y"
{
		yyval.lval = yypt[-0].yyv.sym->value;
	} break;
case 74:
#line	345	"a.y"
{
		yyval.lval = -yypt[-0].yyv.lval;
	} break;
case 75:
#line	349	"a.y"
{
		yyval.lval = yypt[-0].yyv.lval;
	} break;
case 76:
#line	353	"a.y"
{
		yyval.lval = ~yypt[-0].yyv.lval;
	} break;
case 77:
#line	357	"a.y"
{
		yyval.lval = yypt[-1].yyv.lval;
	} break;
case 79:
#line	364	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval + yypt[-0].yyv.lval;
	} break;
case 80:
#line	368	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval - yypt[-0].yyv.lval;
	} break;
case 81:
#line	372	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval * yypt[-0].yyv.lval;
	} break;
case 82:
#line	376	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval / yypt[-0].yyv.lval;
	} break;
case 83:
#line	380	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval % yypt[-0].yyv.lval;
	} break;
case 84:
#line	384	"a.y"
{
		yyval.lval = yypt[-3].yyv.lval << yypt[-0].yyv.lval;
	} break;
case 85:
#line	388	"a.y"
{
		yyval.lval = yypt[-3].yyv.lval >> yypt[-0].yyv.lval;
	} break;
case 86:
#line	392	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval & yypt[-0].yyv.lval;
	} break;
case 87:
#line	396	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval ^ yypt[-0].yyv.lval;
	} break;
case 88:
#line	400	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval | yypt[-0].yyv.lval;
	} break;
	}
	goto yystack;  /* stack new state and value */
}
