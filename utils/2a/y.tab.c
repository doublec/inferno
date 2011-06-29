
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
#define	LWID	57366
#define	LFCONST	57367
#define	LSCONST	57368
#define	LNAME	57369
#define	LLAB	57370
#define	LVAR	57371
#define YYEOFCODE 1
#define YYERRCODE 2
short	yyexca[] =
{-1, 1,
	1, -1,
	-2, 0,
};
#define	YYNPROD	98
#define	YYPRIVATE 57344
#define	yydebug	1
#define	YYLAST	441
short	yyact[] =
{
  31,  55,  35,  34,  97, 101,   2, 132,  98, 163,
  98, 137,  94, 196, 192,  57, 187, 184, 180,  88,
  25, 133, 178,  78, 170, 169,  78, 166, 165,  48,
  81, 136, 188,  90,  96,  77,  98,  92,  79,  27,
  99, 100,  51,  53, 141,  63,  66, 144,  71,  74,
 109,  88, 195, 179, 177, 114, 113,  57, 108,  80,
 112,  90,  78, 107, 110, 111,  46, 126, 125, 124,
 122, 123, 117, 118, 119, 120, 121,  24,  21, 105,
  22,  23, 115, 116,  20, 171,  90, 129, 130, 134,
  78, 149, 131, 174, 173, 175,  78, 138,  41, 105,
 194, 142, 106, 186, 146, 147,  47, 148, 139, 167,
 127,  45, 159,  57, 150, 152, 103, 104,  78,  78,
  78,  78,  78,  64,  68,  78,  78,  78,  82,  67,
 153, 154, 155, 156, 157, 145, 164, 160, 161, 162,
 158,  26, 168, 151, 127, 143,  42,  43, 119, 120,
 121, 176,  86,  87,  39,  32,   6,  76,  65,  78,
  78,  73,  75,   1,  62,  39,  60,  54, 183,  52,
  37, 181, 182,  72,  70,  30,  42,  43,  36,  50,
 190,  37,  44, 102,  38, 103, 104, 193,  69,  33,
 172,  29,  40,  39,  32,  38, 197,  28, 185,   0,
  61,   0,   0,  40,   0,   0,   0, 189,   0,  37,
 191,   0,   0,   0,  30,  42,  43,  36,  39,  32,
   0,  44,   0,  38,   0,   0,   0,  49,  33,   0,
  29,  40,   0,   0,  37,   0,   0,   0,   0,  30,
  42,  43,  36,   0,   0,   7,  44,   0,  38,   0,
   0,   0,   0,  33,   0,  29,  40,   9,  10,  11,
  12,  13,  14,  15,  16,  17,  18,  19,   0,  39,
  60,   0,   0,  39,  60, 117, 118, 119, 120, 121,
   4,   3,   8,   0,   5,  37,  39,  85,   0,  37,
   0, 132,   0,   0,   0,  91,   0,  44,   0,  38,
   0,  44,  37,  38,  95, 133,   0,  40,  95,  93,
   0,  40,  84,  83,  44,   0,  38,  39,  60,   0,
   0,  61,   0,   0,  40,   0,   0,   0,  39,  60,
   0,   0,   0,  37,   0,   0,   0,  39,  60,   0,
   0,   0,   0,   0,  37,  58,  59,  38,  39,  60,
   0,  56,  61,  37,   0,  40,  58,  59,  38, 140,
   0,  39,  60,  61,  37,   0,  40,  38,  39,  60,
 135,   0,  61,  93,   0,  40,   0,  37,  38,   0,
   0,   0,   0,  61,  37,   0,  40, 128,   0,   0,
   0,  38,   0,   0,   0,   0,  61,   0,  38,  40,
   0,   0,   0,  89,   0,   0,  40, 126, 125, 124,
 122, 123, 117, 118, 119, 120, 121, 125, 124, 122,
 123, 117, 118, 119, 120, 121, 124, 122, 123, 117,
 118, 119, 120, 121, 122, 123, 117, 118, 119, 120,
 121
};
short	yypact[] =
{
-1000, 243,-1000,  44,  38,-1000,  40,  36, -22, 209,
  23, 184, 209, 209, 308, 209, 184, 145, 209, 209,
-1000,-1000, 156,-1000,-1000, 156,-1000,  16,-1000, 277,
 143,   7, 359, 264,  -8,-1000,-1000,-1000,-1000, 156,
 156,-1000,-1000,-1000, 176,-1000,-1000,-1000,-1000, 209,
-1000,  90,-1000,  20,-1000,-1000, 319,   6, 107, 107,
 156, 156,-1000,  17,-1000,-1000,  16,-1000,-1000, 209,
-1000,  13,-1000,-1000,  12, 243, 243, 403,-1000, 403,
 209,-1000,-1000,-1000,-1000, 352, 156, 156, 260, 339,
-1000, -14, -34,-1000,  63, 328,   0,-1000, 115,-1000,
-1000,   3, 127, 156, 156,-1000, 156,-1000,-1000,  62,
-1000,-1000, 319, 209, 209,-1000,-1000, 156, 156, 156,
 156, 156, 133, 104, 156, 156, 156,-1000,-1000,-1000,
-1000, -36, -17,-1000,   0, -18, 100,  -8, -20,-1000,
 -21, -24,  51,-1000,  67, 107,-1000,-1000,  11, -23,
-1000,  10, -25, 137, 137,-1000,-1000,-1000, 156, 156,
 427, 420, 412,  -8, -28,-1000,-1000,-1000,-1000,-1000,
-1000,  92, -29,-1000,-1000,-1000, -12, 209,-1000, 156,
 209, 266, 266,-1000,-1000, -31, 156,-1000,  73,-1000,
   9,-1000,-1000,-1000, -32, 156,-1000,-1000
};
short	yypgo[] =
{
   0,   0,  12, 198, 197, 190,   2,   5,  98,   3,
   4,  29,   1, 111, 141, 106, 179, 174, 173, 169,
 167, 164, 163,   6, 162, 157, 156
};
short	yyr1[] =
{
   0,  22,  22,  24,  23,  25,  23,  23,  23,  23,
  26,  26,  26,  26,  26,  26,  26,  26,  26,  26,
  26,  26,  26,  26,  26,  13,  13,  19,  19,  15,
  15,  14,  20,  20,  21,  16,  17,  18,  18,  12,
  12,  12,  11,  11,  11,  11,  11,  11,  11,  11,
  11,  11,  11,  11,  11,  11,  11,  11,  11,  11,
  11,   4,   4,  10,   6,   6,   6,   3,   9,   9,
   9,   9,   9,   8,   8,   7,   7,   7,   5,   5,
   5,   1,   1,   1,   1,   1,   1,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2
};
short	yyr2[] =
{
   0,   0,   2,   0,   4,   0,   4,   1,   2,   2,
   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   0,   1,   1,   2,   1,
   2,   3,   1,   2,   3,   5,   7,   1,   5,   4,
   2,   2,   1,   2,   2,   2,   2,   3,   3,   3,
   1,   4,   4,   1,   2,   4,   5,   3,   4,   4,
   5,   1,   1,   5,   1,   1,   1,   2,   3,   4,
   2,   3,   1,   5,   7,   0,   2,   2,   1,   1,
   1,   1,   1,   2,   2,   2,   3,   1,   3,   3,
   3,   3,   3,   4,   4,   3,   3,   3
};
short	yychk[] =
{
-1000, -22, -23,  38,  37,  41, -26,   2,  39,  14,
  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
  40,  40,  42,  41,  41,  42, -14, -11,  -4,  46,
  30,  -1,  10,  44,  -9,  -6,  33,  25,  39,   9,
  47,  -8,  31,  32,  37, -13,  43, -15, -11,  43,
 -16, -11, -19, -11, -20, -12,  43,  -1,  37,  38,
  10,  44, -21, -11, -15, -14, -11, -13, -15,  43,
 -17, -11, -18, -14, -11, -24, -25,  -2,  -1,  -2,
  43,  -1,  -8,  36,  35,  10,   9,  10,  44,  44,
  -1,  31,  -9,  45,  -2,  44,  -1, -10,  44,  -1,
  -1,  -7,   7,   9,  10, -11,  12,  43, -12,  44,
  -7,  -7,  43,  43,  43, -23, -23,   9,  10,  11,
  12,  13,   7,   8,   6,   5,   4, -11,  35,  -1,
  -1,  -9,  31,  45,  -1,  31,  45,  45, -10,  45,
  31,  44,  -6,  30,  44,   8,  -1,  -1,  -1,  29,
 -12, -11,  -1,  -2,  -2,  -2,  -2,  -2,   7,   8,
  -2,  -2,  -2,  45, -10,  45,  45,   9, -10,  45,
  45,  34,  -5,  27,  26,  28,  -7,  43,  45,  43,
  43,  -2,  -2, -10,  45,  -3,  11,  45,  44, -11,
  -1, -11,  45,  -1,  27,  43,  45,  -1
};
short	yydef[] =
{
   1,  -2,   2,   0,   0,   7,   0,   0,   0,   0,
  25,   0,   0,   0,   0,   0,   0,  25,   0,   0,
   3,   5,   0,   8,   9,   0,  12,   0,  42,   0,
  66,  50,   0,   0,  53,  61,  62,  81,  82,   0,
   0,  72,  64,  65,  75,  13,  26,  14,  29,   0,
  15,   0,  16,  27,  17,  32,   0,   0,  75,  75,
   0,   0,  18,   0,  19,  20,  29,  21,  22,  26,
  23,   0,  24,  37,   0,   0,   0,  10,  87,  11,
   0,  43,  44,  45,  46,   0,   0,   0,   0,   0,
  83,   0,   0,  70,   0,   0,  87,  54,   0,  84,
  85,   0,   0,   0,   0,  30,   0,  28,  33,   0,
  40,  41,   0,   0,   0,   4,   6,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  31,  47,  48,
  49,   0,   0,  71,   0,   0,  68,  57,   0,  86,
   0,   0,   0,  66,   0,  75,  76,  77,   0,   0,
  34,   0,  50,  88,  89,  90,  91,  92,   0,   0,
  95,  96,  97,  58,   0,  69,  51,  52,  55,  59,
  68,   0,   0,  78,  79,  80,   0,   0,  39,   0,
   0,  93,  94,  56,  60,   0,   0,  73,   0,  35,
   0,  38,  63,  67,   0,   0,  74,  36
};
short	yytok1[] =
{
   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  46,  13,   6,   0,
  44,  45,  11,   9,  43,  10,   0,  12,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  40,  41,
   7,  42,   8,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   5,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   4,   0,  47
};
short	yytok2[] =
{
   2,   3,  14,  15,  16,  17,  18,  19,  20,  21,
  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  32,  33,  34,  35,  36,  37,  38,  39
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
		if((yyval.gen.type & D_MASK) == D_NONE) {
			yyval.gen.index = D_NONE | I_INDEX1;
			yyval.gen.scale = 0;
			yyval.gen.displace = 0;
		}
	} break;
case 54:
#line	275	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-1].yyv.addr.type;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-1].yyv.addr;
		}
		yyval.gen.index = yypt[-0].yyv.addr.type | I_INDEX1;
		yyval.gen.scale = yypt[-0].yyv.addr.offset;
	} break;
case 55:
#line	287	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-2].yyv.addr.type;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-2].yyv.addr;
		}
		yyval.gen.index = yypt[-0].yyv.addr.type | I_INDEX2;
		yyval.gen.scale = yypt[-0].yyv.addr.offset;
		yyval.gen.displace = 0;
	} break;
case 56:
#line	300	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-2].yyv.addr.type;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-2].yyv.addr;
		}
		yyval.gen.index = yypt[-0].yyv.addr.type | I_INDEX2;
		yyval.gen.scale = yypt[-0].yyv.addr.offset;
		yyval.gen.displace = yypt[-4].yyv.lval;
	} break;
case 57:
#line	313	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-1].yyv.addr.type;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-1].yyv.addr;
		}
		yyval.gen.index = D_NONE | I_INDEX3;
		yyval.gen.scale = 0;
		yyval.gen.displace = 0;
	} break;
case 58:
#line	326	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-1].yyv.addr.type;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-1].yyv.addr;
		}
		yyval.gen.index = D_NONE | I_INDEX3;
		yyval.gen.scale = 0;
		yyval.gen.displace = yypt[-3].yyv.lval;
	} break;
case 59:
#line	339	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-2].yyv.addr.type;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-2].yyv.addr;
		}
		yyval.gen.index = yypt[-1].yyv.addr.type | I_INDEX3;
		yyval.gen.scale = yypt[-1].yyv.addr.offset;
		yyval.gen.displace = 0;
	} break;
case 60:
#line	352	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = yypt[-2].yyv.addr.type;
		{
			Addr *a;
			a = &yyval.gen.s0;
			*a = yypt[-2].yyv.addr;
		}
		yyval.gen.index = yypt[-1].yyv.addr.type | I_INDEX3;
		yyval.gen.scale = yypt[-1].yyv.addr.offset;
		yyval.gen.displace = yypt[-4].yyv.lval;
	} break;
case 63:
#line	381	"a.y"
{
		yyval.addr.type = yypt[-3].yyv.lval;
		yyval.addr.offset = yypt[-2].yyv.lval+yypt[-1].yyv.lval;
		yyval.addr.sym = S;
	} break;
case 67:
#line	394	"a.y"
{
		switch(yypt[-0].yyv.lval) {
		case 1:
			yyval.lval = 0;
			break;

		case 2:
			yyval.lval = 1;
			break;

		default:
			yyerror("bad scale: %ld", yypt[-0].yyv.lval);

		case 4:
			yyval.lval = 2;
			break;

		case 8:
			yyval.lval = 3;
			break;
		}
	} break;
case 68:
#line	419	"a.y"
{
		yyval.addr.type = yypt[-1].yyv.lval | I_INDIR;
		yyval.addr.sym = S;
		yyval.addr.offset = 0;
	} break;
case 69:
#line	425	"a.y"
{
		yyval.addr.type = yypt[-1].yyv.lval | I_INDIR;
		yyval.addr.sym = S;
		yyval.addr.offset = yypt[-3].yyv.lval;
	} break;
case 70:
#line	431	"a.y"
{
		yyval.addr.type = D_NONE | I_INDIR;
		yyval.addr.sym = S;
		yyval.addr.offset = 0;
	} break;
case 71:
#line	437	"a.y"
{
		yyval.addr.type = D_NONE | I_INDIR;
		yyval.addr.sym = S;
		yyval.addr.offset = yypt[-2].yyv.lval;
	} break;
case 73:
#line	446	"a.y"
{
		yyval.addr.type = yypt[-1].yyv.lval;
		yyval.addr.sym = yypt[-4].yyv.sym;
		yyval.addr.offset = yypt[-3].yyv.lval;
	} break;
case 74:
#line	452	"a.y"
{
		yyval.addr.type = D_STATIC;
		yyval.addr.sym = yypt[-6].yyv.sym;
		yyval.addr.offset = yypt[-3].yyv.lval;
	} break;
case 75:
#line	459	"a.y"
{
		yyval.lval = 0;
	} break;
case 76:
#line	463	"a.y"
{
		yyval.lval = yypt[-0].yyv.lval;
	} break;
case 77:
#line	467	"a.y"
{
		yyval.lval = -yypt[-0].yyv.lval;
	} break;
case 82:
#line	479	"a.y"
{
		yyval.lval = yypt[-0].yyv.sym->value;
	} break;
case 83:
#line	483	"a.y"
{
		yyval.lval = -yypt[-0].yyv.lval;
	} break;
case 84:
#line	487	"a.y"
{
		yyval.lval = yypt[-0].yyv.lval;
	} break;
case 85:
#line	491	"a.y"
{
		yyval.lval = ~yypt[-0].yyv.lval;
	} break;
case 86:
#line	495	"a.y"
{
		yyval.lval = yypt[-1].yyv.lval;
	} break;
case 88:
#line	502	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval + yypt[-0].yyv.lval;
	} break;
case 89:
#line	506	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval - yypt[-0].yyv.lval;
	} break;
case 90:
#line	510	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval * yypt[-0].yyv.lval;
	} break;
case 91:
#line	514	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval / yypt[-0].yyv.lval;
	} break;
case 92:
#line	518	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval % yypt[-0].yyv.lval;
	} break;
case 93:
#line	522	"a.y"
{
		yyval.lval = yypt[-3].yyv.lval << yypt[-0].yyv.lval;
	} break;
case 94:
#line	526	"a.y"
{
		yyval.lval = yypt[-3].yyv.lval >> yypt[-0].yyv.lval;
	} break;
case 95:
#line	530	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval & yypt[-0].yyv.lval;
	} break;
case 96:
#line	534	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval ^ yypt[-0].yyv.lval;
	} break;
case 97:
#line	538	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval | yypt[-0].yyv.lval;
	} break;
	}
	goto yystack;  /* stack new state and value */
}
