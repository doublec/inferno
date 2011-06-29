
#line	2	"a.y"
#include "a.h"

#line	4	"a.y"
typedef union 	{
	Sym	*sym;
	long	lval;
	double	dval;
	char	sval[8];
	Gen	gen;
	Gen2	gen2;
} YYSTYPE;
extern	int	yyerrflag;
#ifndef	YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE	yylval;
YYSTYPE	yyval;
#define	LTYPE0	57346
#define	LTYPE1	57347
#define	LTYPE2	57348
#define	LTYPE3	57349
#define	LTYPE4	57350
#define	LTYPEC	57351
#define	LTYPED	57352
#define	LTYPEN	57353
#define	LTYPER	57354
#define	LTYPET	57355
#define	LTYPES	57356
#define	LTYPEM	57357
#define	LTYPEI	57358
#define	LCONST	57359
#define	LFP	57360
#define	LPC	57361
#define	LSB	57362
#define	LBREG	57363
#define	LLREG	57364
#define	LSREG	57365
#define	LFREG	57366
#define	LFCONST	57367
#define	LSCONST	57368
#define	LSP	57369
#define	LNAME	57370
#define	LLAB	57371
#define	LVAR	57372
#define YYEOFCODE 1
#define YYERRCODE 2
short	yyexca[] =
{-1, 1,
	1, -1,
	-2, 0,
};
#define	YYNPROD	113
#define	YYPRIVATE 57344
#define	yydebug	1
#define	YYLAST	490
short	yyact[] =
{
  42,  54,  53, 106,   2,  40,  33,  73,  41,  52,
  55, 179, 219,  32, 218,  63,  59, 101,  80,  79,
  44,  32,  57,  78,  77,  95,  75, 216,  95,  87,
  89,  91,  74, 212,  97, 149,  68, 210, 203, 201,
 191, 190,  94, 181,  95,  96, 178,  62, 103, 104,
 105, 180, 150,  48,  47, 204, 111, 198, 182, 153,
 128, 120,  63, 102,  98, 193, 192,  95, 121, 122,
 148,  45, 132,  80,  79, 131, 112, 130,  78,  77,
 125,  75, 127, 126,  46, 129, 124, 118,  97,  66,
 117,  55,  49, 110, 119,  29,  27, 133, 134, 144,
 143, 142, 140, 141, 135, 136, 137, 138, 139, 155,
 156,  23, 195,  24,  26,  25,  95, 103, 194,  22,
 159, 188,  50, 186, 162, 164, 163, 208, 160, 209,
 187, 207, 200, 165, 166, 167,  95,  95,  95,  95,
  95, 151, 152,  95,  95,  95, 145, 147,  48,  47,
 183, 146, 213, 168, 169, 170, 171, 172, 189, 161,
 175, 176, 177,  30,  28, 123,  45, 137, 138, 139,
  35,  37,  39,  36,  95,  95,  38,  50, 214,  46,
 199, 174,  84,  83,  43, 202,  55,  49, 211, 108,
 109, 196, 197,  34, 205, 206,  48,  47,  76, 161,
 184, 154, 145, 147, 173,  48,  47, 146,   6,  93,
  70,  92, 215,   1,  45, 217,  90,  88,  35,  37,
  39,  36,  86,  45,  38,  81,  65,  46,  48,  47,
  76,  72,  43,  82,  55,  49,  46, 107,  71, 108,
 109,  66,  69,  67,  49,  58,  45,  48,  47,  56,
  35,  37,  39,  36,  51,  60,  38,  81,  65,  46,
 185,   0,   0,   0,  43,  45,  55,  49,   0,  35,
  37,  39,  36,  48,  47,  38,  50,   0,  46,   0,
   0,   0,  85,  43,   0,   0,  49,   0,   0,   0,
   0,  45,  48,  47,   0,  35,  37,  39,  36,   0,
   0,  38,  50,   0,  46,   0,   0,   0,  31,  43,
  45,   0,  49,   0,  35,  37,  39,  36,   0,   0,
  38,  50,   7,  46,   0,   0,   0,   0,  43,   0,
   0,  49,   0,   0,   9,  10,  11,  12,  13,  17,
  15,  18,  14,  16,  19,  20,  21,  48,  47, 143,
 142, 140, 141, 135, 136, 137, 138, 139,   4,   3,
   8,   0,   5,  48, 116,  45,   0,  48,  47,  35,
  37,  39,  36,   0,   0,  38,   0,   0,  46,  48,
  47,  45,   0,  43,   0,  45,  49,   0,   0, 114,
 113,   0,  50, 158,  46,  48,  47,  45,  46, 115,
   0,   0,  49,  66,  48,  47,  49,   0,  64,  65,
  46,  48,  47,  45,  61,  66,   0,   0,  49,   0,
  48,  47,  45,   0,  64,  65,  46,  99,   0,  45,
   0,  66, 100,   0,  49,  46,   0, 157,  45,   0,
  66,   0,  46,  49,   0,   0,   0,  66,   0,  50,
  49,  46,   0,   0,   0,   0,  43,   0,   0,  49,
 144, 143, 142, 140, 141, 135, 136, 137, 138, 139,
 142, 140, 141, 135, 136, 137, 138, 139, 140, 141,
 135, 136, 137, 138, 139, 135, 136, 137, 138, 139
};
short	yypact[] =
{
-1000, 320,-1000,  78,  70,-1000,  73,  72,  53,  51,
 264, 139, 139, 283, 370,  84, 411, 187, 238, 139,
 139, 139,-1000,-1000, 196,-1000,-1000, 196,-1000,-1000,
-1000, 283,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,  19, 395,  18,-1000,-1000, 196, 196, 196,
 230,-1000,  49,-1000,-1000, 354,-1000,  46,-1000,  43,
-1000, 386,-1000,  16, 180, 180, 196,-1000, 153,-1000,
  42,-1000, 219,-1000,-1000,-1000, 338,-1000,-1000,-1000,
  15, 230,-1000,-1000,-1000, 283,-1000,  33,-1000,  31,
-1000,  28, 320, 320, 456,-1000, 456,-1000, 114,  24,
   6,  95, 110,-1000,-1000,-1000,  14, 193, 196, 196,
-1000,-1000,-1000,-1000,-1000, 402, 358, 283, 139,-1000,
 130,-1000,-1000, 196,  44,-1000,-1000,-1000, 170,  14,
 283, 283, 283,-1000,-1000, 196, 196, 196, 196, 196,
 197, 173, 196, 196, 196,   0,   5,  -3,  13, 196,
-1000,-1000, 189,  93, 180,-1000,-1000,  -5,-1000,-1000,
-1000,  -6,  22,-1000,  21,  77,  71,-1000, 156, 156,
-1000,-1000,-1000, 196, 196, 471, 464, 344,  12, 196,
-1000,-1000, 100,  -7, 196,  -8,-1000,-1000,-1000,  10,
-1000,-1000, -37, -37,  99,  94, 476, 476,  97,  -9,
 177,-1000, -13,-1000, 122,-1000,-1000,-1000,-1000, 167,
-1000, 196,-1000, -19, 196, -32,-1000, -34,-1000,-1000
};
short	yypgo[] =
{
   0,   0,  17, 260,   3, 193,   1,   6,  20,  32,
   2,   9,   7,   5,   8, 164, 255, 163, 254, 249,
 245, 243, 242, 238, 233, 222, 217, 216, 213,   4,
 211, 209, 208
};
short	yyr1[] =
{
   0,  28,  28,  30,  29,  31,  29,  29,  29,  29,
  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
  32,  32,  32,  32,  32,  15,  15,  19,  20,  18,
  18,  17,  17,  16,  16,  21,  22,  22,  23,  23,
  24,  24,  25,  25,  26,  26,  27,  27,  27,  10,
  10,  12,  12,  12,  12,  12,  12,  12,  11,  11,
   9,   9,   9,   7,   7,   7,   7,   7,   6,   6,
   6,   6,   6,   6,   5,   5,  13,  13,  13,  13,
  13,  13,  13,  13,  13,  13,  14,  14,   8,   8,
   4,   4,   4,   3,   3,   3,   1,   1,   1,   1,
   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2
};
short	yyr2[] =
{
   0,   0,   2,   0,   4,   0,   4,   1,   2,   2,
   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   0,   1,   3,   3,   2,
   1,   2,   1,   2,   1,   5,   3,   5,   2,   1,
   1,   1,   3,   5,   3,   5,   2,   1,   3,   1,
   1,   1,   1,   2,   2,   1,   1,   1,   1,   1,
   4,   2,   2,   1,   1,   1,   1,   1,   2,   2,
   2,   2,   4,   3,   1,   1,   1,   4,   4,   6,
   9,   3,   3,   4,   5,   8,   1,   6,   5,   7,
   0,   2,   2,   1,   1,   1,   1,   1,   2,   2,
   2,   3,   1,   3,   3,   3,   3,   3,   4,   4,
   3,   3,   3
};
short	yychk[] =
{
-1000, -28, -29,  39,  38,  42, -32,   2,  40,  14,
  15,  16,  17,  18,  22,  20,  23,  19,  21,  24,
  25,  26,  41,  41,  43,  42,  42,  43, -15,  44,
 -17,  44, -10,  -7,  -5,  31,  34,  32,  37,  33,
 -13, -14,  -1,  45,  -8,  27,  40,  10,   9,  48,
  38, -18, -11, -10,  -6,  47, -19, -11, -20, -10,
 -16,  44,  -9,  -1,  38,  39,  45, -21,  -8, -22,
  -5, -23,  44, -12,  -9, -14,  11,  -7, -13,  -6,
  -1,  38, -24, -15, -17,  44, -25, -11, -26, -11,
 -27, -11, -30, -31,  -2,  -1,  -2, -10,  45,  32,
  37,  -2,  45,  -1,  -1,  -1,  -4,   7,   9,  10,
  44,  -1,  -8,  36,  35,  45,  10,  44,  44,  -9,
  45,  -4,  -4,  12,  44, -12,  -7, -13,  45,  -4,
  44,  44,  44, -29, -29,   9,  10,  11,  12,  13,
   7,   8,   6,   5,   4,  32,  37,  33,  46,  11,
  46,  46,  32,  45,   8,  -1,  -1,  35,  35, -10,
 -11,  29,  -1,  -6,  -1, -10, -10, -10,  -2,  -2,
  -2,  -2,  -2,   7,   8,  -2,  -2,  -2,  46,  11,
  46,  46,  45,  -1,  11,  -3,  30,  37,  28,  -4,
  46,  46,  44,  44,  41,  41,  -2,  -2,  45,  -1,
  32,  46,  -1,  46,  45,  -6,  -6,  32,  33,  32,
  46,  11,  46,  30,  11,  -1,  46,  -1,  46,  46
};
short	yydef[] =
{
   1,  -2,   2,   0,   0,   7,   0,   0,   0,  25,
   0,   0,   0,   0,   0,   0,   0,   0,  25,   0,
   0,   0,   3,   5,   0,   8,   9,   0,  12,  26,
  13,   0,  32,  49,  50,  63,  64,  65,  66,  67,
  74,  75,  76,   0,  86,  96,  97,   0,   0,   0,
  90,  14,  30,  58,  59,   0,  15,   0,  16,   0,
  17,   0,  34,   0,  90,  90,   0,  18,   0,  19,
   0,  20,   0,  39,  51,  52,   0,  55,  56,  57,
  76,  90,  21,  40,  41,  26,  22,   0,  23,   0,
  24,  47,   0,   0,  10, 102,  11,  31,   0,   0,
   0,   0,   0,  98,  99, 100,   0,   0,   0,   0,
  29,  68,  69,  70,  71,   0,   0,   0,   0,  33,
   0,  61,  62,   0,   0,  38,  53,  54,   0,  61,
   0,   0,  46,   4,   6,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  81,   0,
  82, 101,   0,   0,  90,  91,  92,   0,  73,  27,
  28,   0,   0,  36,   0,  42,  44,  48, 103, 104,
 105, 106, 107,   0,   0, 110, 111, 112,  77,   0,
  78,  83,   0,   0,   0,   0,  93,  94,  95,   0,
  72,  60,   0,   0,   0,   0, 108, 109,   0,   0,
   0,  84,   0,  88,   0,  35,  37,  43,  45,   0,
  79,   0,  87,   0,   0,   0,  89,   0,  85,  80
};
short	yytok1[] =
{
   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  47,  13,   6,   0,
  45,  46,  11,   9,  44,  10,   0,  12,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  41,  42,
   7,  43,   8,   0,   0,   0,   0,   0,   0,   0,
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
  32,  33,  34,  35,  36,  37,  38,  39,  40
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
		yyval.gen2.from = yypt[-2].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 28:
#line	97	"a.y"
{
		yyval.gen2.from = yypt[-2].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 29:
#line	104	"a.y"
{
		yyval.gen2.from = yypt[-1].yyv.gen;
		yyval.gen2.to = nullgen;
	} break;
case 30:
#line	109	"a.y"
{
		yyval.gen2.from = yypt[-0].yyv.gen;
		yyval.gen2.to = nullgen;
	} break;
case 31:
#line	116	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 32:
#line	121	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 33:
#line	128	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 34:
#line	133	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 35:
#line	140	"a.y"
{
		yyval.gen2.from = yypt[-4].yyv.gen;
		yyval.gen2.from.scale = yypt[-2].yyv.lval;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 36:
#line	148	"a.y"
{
		yyval.gen2.from = yypt[-2].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 37:
#line	153	"a.y"
{
		yyval.gen2.from = yypt[-4].yyv.gen;
		yyval.gen2.from.scale = yypt[-2].yyv.lval;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 38:
#line	161	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 39:
#line	166	"a.y"
{
		yyval.gen2.from = nullgen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 42:
#line	177	"a.y"
{
		yyval.gen2.from = yypt[-2].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 43:
#line	182	"a.y"
{
		yyval.gen2.from = yypt[-4].yyv.gen;
		yyval.gen2.to = yypt[-2].yyv.gen;
		if(yyval.gen2.from.index != D_NONE)
			yyerror("dp shift with lhs index");
		yyval.gen2.from.index = yypt[-0].yyv.lval;
	} break;
case 44:
#line	192	"a.y"
{
		yyval.gen2.from = yypt[-2].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 45:
#line	197	"a.y"
{
		yyval.gen2.from = yypt[-4].yyv.gen;
		yyval.gen2.to = yypt[-2].yyv.gen;
		if(yyval.gen2.to.index != D_NONE)
			yyerror("dp move with lhs index");
		yyval.gen2.to.index = yypt[-0].yyv.lval;
	} break;
case 46:
#line	207	"a.y"
{
		yyval.gen2.from = yypt[-1].yyv.gen;
		yyval.gen2.to = nullgen;
	} break;
case 47:
#line	212	"a.y"
{
		yyval.gen2.from = yypt[-0].yyv.gen;
		yyval.gen2.to = nullgen;
	} break;
case 48:
#line	217	"a.y"
{
		yyval.gen2.from = yypt[-2].yyv.gen;
		yyval.gen2.to = yypt[-0].yyv.gen;
	} break;
case 53:
#line	230	"a.y"
{
		yyval.gen = yypt[-0].yyv.gen;
	} break;
case 54:
#line	234	"a.y"
{
		yyval.gen = yypt[-0].yyv.gen;
	} break;
case 60:
#line	247	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_BRANCH;
		yyval.gen.offset = yypt[-3].yyv.lval + pc;
	} break;
case 61:
#line	253	"a.y"
{
		yyval.gen = nullgen;
		if(pass == 2)
			yyerror("undefined label: %s", yypt[-1].yyv.sym->name);
		yyval.gen.type = D_BRANCH;
		yyval.gen.sym = yypt[-1].yyv.sym;
		yyval.gen.offset = yypt[-0].yyv.lval;
	} break;
case 62:
#line	262	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_BRANCH;
		yyval.gen.sym = yypt[-1].yyv.sym;
		yyval.gen.offset = yypt[-1].yyv.sym->value + yypt[-0].yyv.lval;
	} break;
case 63:
#line	271	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-0].yyv.lval;
	} break;
case 64:
#line	276	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-0].yyv.lval;
	} break;
case 65:
#line	281	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-0].yyv.lval;
	} break;
case 66:
#line	286	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_SP;
	} break;
case 67:
#line	291	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-0].yyv.lval;
	} break;
case 68:
#line	298	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_CONST;
		yyval.gen.offset = yypt[-0].yyv.lval;
	} break;
case 69:
#line	304	"a.y"
{
		yyval.gen = yypt[-0].yyv.gen;
		yyval.gen.index = yypt[-0].yyv.gen.type;
		yyval.gen.type = D_ADDR;
		/*
		if($2.type == D_AUTO || $2.type == D_PARAM)
			yyerror("constant cannot be automatic: %s",
				$2.sym->name);
		 */
	} break;
case 70:
#line	315	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_SCONST;
		memcpy(yyval.gen.sval, yypt[-0].yyv.sval, sizeof(yyval.gen.sval));
	} break;
case 71:
#line	321	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCONST;
		yyval.gen.dval = yypt[-0].yyv.dval;
	} break;
case 72:
#line	327	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCONST;
		yyval.gen.dval = yypt[-1].yyv.dval;
	} break;
case 73:
#line	333	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCONST;
		yyval.gen.dval = -yypt[-0].yyv.dval;
	} break;
case 76:
#line	345	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+D_NONE;
		yyval.gen.offset = yypt[-0].yyv.lval;
	} break;
case 77:
#line	351	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+yypt[-1].yyv.lval;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 78:
#line	357	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+D_SP;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 79:
#line	363	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+D_NONE;
		yyval.gen.offset = yypt[-5].yyv.lval;
		yyval.gen.index = yypt[-3].yyv.lval;
		yyval.gen.scale = yypt[-1].yyv.lval;
		checkscale(yyval.gen.scale);
	} break;
case 80:
#line	372	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+yypt[-6].yyv.lval;
		yyval.gen.offset = yypt[-8].yyv.lval;
		yyval.gen.index = yypt[-3].yyv.lval;
		yyval.gen.scale = yypt[-1].yyv.lval;
		checkscale(yyval.gen.scale);
	} break;
case 81:
#line	381	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+yypt[-1].yyv.lval;
	} break;
case 82:
#line	386	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+D_SP;
	} break;
case 83:
#line	391	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+yypt[-1].yyv.lval;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 84:
#line	397	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+D_NONE;
		yyval.gen.index = yypt[-3].yyv.lval;
		yyval.gen.scale = yypt[-1].yyv.lval;
		checkscale(yyval.gen.scale);
	} break;
case 85:
#line	405	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_INDIR+yypt[-6].yyv.lval;
		yyval.gen.index = yypt[-3].yyv.lval;
		yyval.gen.scale = yypt[-1].yyv.lval;
		checkscale(yyval.gen.scale);
	} break;
case 86:
#line	415	"a.y"
{
		yyval.gen = yypt[-0].yyv.gen;
	} break;
case 87:
#line	419	"a.y"
{
		yyval.gen = yypt[-5].yyv.gen;
		yyval.gen.index = yypt[-3].yyv.lval;
		yyval.gen.scale = yypt[-1].yyv.lval;
		checkscale(yyval.gen.scale);
	} break;
case 88:
#line	428	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-1].yyv.lval;
		yyval.gen.sym = yypt[-4].yyv.sym;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 89:
#line	435	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_STATIC;
		yyval.gen.sym = yypt[-6].yyv.sym;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 90:
#line	443	"a.y"
{
		yyval.lval = 0;
	} break;
case 91:
#line	447	"a.y"
{
		yyval.lval = yypt[-0].yyv.lval;
	} break;
case 92:
#line	451	"a.y"
{
		yyval.lval = -yypt[-0].yyv.lval;
	} break;
case 94:
#line	458	"a.y"
{
		yyval.lval = D_AUTO;
	} break;
case 97:
#line	466	"a.y"
{
		yyval.lval = yypt[-0].yyv.sym->value;
	} break;
case 98:
#line	470	"a.y"
{
		yyval.lval = -yypt[-0].yyv.lval;
	} break;
case 99:
#line	474	"a.y"
{
		yyval.lval = yypt[-0].yyv.lval;
	} break;
case 100:
#line	478	"a.y"
{
		yyval.lval = ~yypt[-0].yyv.lval;
	} break;
case 101:
#line	482	"a.y"
{
		yyval.lval = yypt[-1].yyv.lval;
	} break;
case 103:
#line	489	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval + yypt[-0].yyv.lval;
	} break;
case 104:
#line	493	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval - yypt[-0].yyv.lval;
	} break;
case 105:
#line	497	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval * yypt[-0].yyv.lval;
	} break;
case 106:
#line	501	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval / yypt[-0].yyv.lval;
	} break;
case 107:
#line	505	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval % yypt[-0].yyv.lval;
	} break;
case 108:
#line	509	"a.y"
{
		yyval.lval = yypt[-3].yyv.lval << yypt[-0].yyv.lval;
	} break;
case 109:
#line	513	"a.y"
{
		yyval.lval = yypt[-3].yyv.lval >> yypt[-0].yyv.lval;
	} break;
case 110:
#line	517	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval & yypt[-0].yyv.lval;
	} break;
case 111:
#line	521	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval ^ yypt[-0].yyv.lval;
	} break;
case 112:
#line	525	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval | yypt[-0].yyv.lval;
	} break;
	}
	goto yystack;  /* stack new state and value */
}
