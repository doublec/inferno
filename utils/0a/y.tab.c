
#line	2	"a.y"
#include "a.h"

#line	4	"a.y"
typedef union 
{
	Sym	*sym;
	long	lval;
	double	dval;
	char	sval[8];
	Gen	gen;
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
#define	LTYPEC	57357
#define	LTYPED	57358
#define	LTYPEE	57359
#define	LTYPEF	57360
#define	LTYPEG	57361
#define	LTYPEH	57362
#define	LTYPEI	57363
#define	LTYPEJ	57364
#define	LTYPEK	57365
#define	LCONST	57366
#define	LSP	57367
#define	LSB	57368
#define	LFP	57369
#define	LPC	57370
#define	LHI	57371
#define	LLO	57372
#define	LMREG	57373
#define	LTYPEX	57374
#define	LREG	57375
#define	LFREG	57376
#define	LFCREG	57377
#define	LR	57378
#define	LM	57379
#define	LF	57380
#define	LFCR	57381
#define	LSCHED	57382
#define	LFCONST	57383
#define	LSCONST	57384
#define	LNAME	57385
#define	LLAB	57386
#define	LVAR	57387
#define YYEOFCODE 1
#define YYERRCODE 2
short	yyexca[] =
{-1, 1,
	1, -1,
	-2, 0,
};
#define	YYNPROD	119
#define	YYPRIVATE 57344
#define	yydebug	1
#define	YYLAST	634
short	yyact[] =
{
 104,  68,  36,  46, 122, 180,  38, 138,  49,  39,
 133,  50,   2,  48,  43,  48,  40, 184,  57,  48,
  48,  82,  82, 124,  85,  86,  87,  78,  48,  48,
 264,  47, 258,  54,  53, 255, 103, 105, 244,  81,
  84, 107,  37,  37,  59, 134, 240,  75, 239,  41,
 238, 112,  42, 235, 118, 119, 120, 232,  51, 113,
 231,  76,  77, 186,  41,  92, 121,  42,  40, 256,
  88,  89,  90,  99, 111, 245, 189, 135, 135,  52,
  60,  29, 215, 198,  83,  93,  40,  55, 141, 153,
 147, 131,  48, 155, 100, 139, 139, 130,  45, 152,
  45, 129, 123, 108,  79,  80,  60, 249, 248, 247,
 246,  48, 177, 175, 159, 160, 173, 242, 118, 176,
 230, 179, 228, 142, 158, 151, 150, 191, 192,  48,
 194, 195, 196, 188, 149, 146, 145, 157, 174, 144,
 143,  37, 197, 199, 204, 153, 153, 209, 132, 210,
 202, 211, 212, 208, 206, 205, 207, 156, 128,  48,
 200, 201, 139, 110, 109,  54,  53, 218, 219, 220,
 221, 222, 106,  35, 225, 226, 227,  34, 216,  33,
  32,  54,  53,  30,  31, 234, 214,  73, 213, 203,
  51,  74,  74, 233, 193, 236, 178,  41, 243,  41,
  42, 263,  42, 182, 181, 183,  51, 148, 202, 154,
 137,  52, 199, 224, 237,  41,  83, 223,  42,  55,
 164, 165, 166, 190, 217,  58,  94,  52, 251, 252,
   8,  96,  56, 254,  50,  55, 102, 182, 181, 183,
 243,  63, 241,  95, 101,  41,  65, 153,  42, 126,
 127, 262, 261,   1, 260, 202,  61, 259,  64, 250,
 253,   0,  37,   9, 171, 170, 169, 167, 168, 162,
 163, 164, 165, 166, 257,  10,  11,  12,  13,  14,
  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
  25,  26,  27,  28,  44,  54,  53,   0, 171, 170,
 169, 167, 168, 162, 163, 164, 165, 166,   0,  62,
 125,   6, 126, 127,   4,   3,   5,   0,   0,   7,
  51, 229,  54,  53,   0,  97,  98,  69,   0,  41,
  73,  71,  42,  70,  74,  72,   0,   0,   0,  58,
   0,  52,   0,   0,   0,  60,  56,  51,   0,  55,
  54,  53,  97,  98,  69, 187,  41,  73,  71,  42,
  70,  74,  72, 182, 181, 183,  58,   0,  52,   0,
   0,  41,  91,  56,  42,  51,  55,  54,  53,   0,
  66,  67,  69,   0,  41,  73,  71,  42,  70,  74,
  72,   0,   0,   0,  58,   0,  52,  54, 117, 114,
   0,  56,  51,  50,  55,   0,   0,  97,  98,  69,
   0,  41,  73,  71,  42,  70,  74,  72,   0,  54,
  53,  58,  51,  52,   0,   0,   0,   0,  56,   0,
   0,  55,  54,  53,  54,  53,   0,   0,   0, 116,
 115,  58,   0,  52,  51,   0,   0,   0,  56,  54,
  53,  55,   0,  41,   0,   0,  42,  51,   0,  51,
   0,   0,   0,  58,   0,  52,  54,  53,  41,   0,
  56,  42,   0,  55,  51,   0, 154, 137,  52,   0,
  52,  54,  53,  83,   0,  83,  55,   0,  55,   0,
   0,  51,   0, 136, 137,  52,  54,  53,   0,   0,
 140,   0,   0,  55,   0,   0,  51,   0,  54,  53,
  58,   0,  52,   0,  54,  53,   0,  56,   0,   0,
  55,  51,   0,   0,   0,  58,   0,  52,   0,   0,
   0,   0, 140,  51,   0,  55,   0,   0, 185,  51,
   0,   0,  52,   0,   0,   0,   0,  83,   0,   0,
  55,   0,  58,   0,  52,   0,   0,   0,   0,  83,
  52,   0,  55,   0,   0,  83,   0,   0,  55, 171,
 170, 169, 167, 168, 162, 163, 164, 165, 166, 171,
 170, 169, 167, 168, 162, 163, 164, 165, 166, 170,
 169, 167, 168, 162, 163, 164, 165, 166, 169, 167,
 168, 162, 163, 164, 165, 166, 167, 168, 162, 163,
 164, 165, 166, 162, 163, 164, 165, 166,   0,   0,
   0,   0,   0, 172,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 161
};
short	yypact[] =
{
-1000, 261,-1000,  25, 127, 123, 121,-1000, 119, 115,
   6,   6, 172,  47, 341, 154,  47,  21, 410, 410,
 499, 499, 143, 143, 143,  47,  47, 313, 286,-1000,
-1000, 505, 505,-1000,-1000,-1000, 113,-1000,-1000,-1000,
 505,-1000,  43, 105, 104,-1000,-1000,-1000,  14,-1000,
 388,-1000,-1000, 505, 505, 505, 425,  42, 303,-1000,
-1000,  99,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
  41,-1000,  37,-1000,  31,  89, 440, 440,  81,  80,
  77,  76,  30, 505, 195,  75,  67,  66, 423, -51,
-1000, 368,  47,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
  65, 261, 261, 575,-1000, 565, 154,-1000, 505,   6,
 410, 328,  14,-1000, -45,-1000,-1000, 487,-1000,-1000,
-1000,   2, 294, 154,  16, 215, 505, 505, 368, 505,
 505, 505, 154,-1000,-1000,  23, 303, 240,-1000,-1000,
 425,-1000,-1000, 472, 156, 423,  24, 168, 505, 143,
 144, 142,-1000,  22, 240,-1000,-1000,-1000, 368,-1000,
-1000,-1000, 505, 505, 505, 505, 505, 210, 205, 505,
 505, 505,-1000,  63,-1000, 260,  61,-1000,-1000,  -1,
  -4,-1000,-1000,-1000, 457,-1000,-1000,-1000,  -8, 168,
 240,-1000,-1000,-1000, -11, -13, -15,  58, 202,-1000,
  16,-1000, -23,-1000,  15,-1000,  51,-1000,-1000,  50,
  49,-1000,-1000,  48,  47, 160,-1000,-1000, 209, 209,
-1000,-1000,-1000, 505, 505, 599, 592, 584, 154,-1000,
   6,-1000,-1000,-1000,  14,-1000, -26,   9,-1000,-1000,
-1000,-1000, 154, -29,-1000, 328, 423, -46, -51, 143,
-1000, 604, 604,-1000,-1000,-1000, 165,-1000,-1000,-1000,
-1000,-1000,-1000, -31,-1000
};
short	yypgo[] =
{
   0,   0,   4,   5,  23,   9,  85,  65, 294, 256,
  10,  31,   1, 243, 231,   6,   3,   7,  18,   8,
   2,  45, 226, 253,  12, 244, 236, 230,  44
};
short	yyr1[] =
{
   0,  23,  23,  25,  24,  26,  24,  24,  24,  24,
  24,  24,  24,  27,  27,  27,  27,  27,  27,  27,
  27,  27,  27,  27,  27,  27,  27,  27,  27,  27,
  27,  27,  27,  27,  27,  27,  27,  27,  27,  27,
  27,  27,  27,  27,  28,  28,  10,  10,  10,   9,
   9,   9,   9,   9,   9,   7,   7,   7,   7,   7,
   7,   8,   8,  22,  13,  13,  14,  14,  12,  12,
  16,  16,  16,  16,  16,  16,  21,  21,  21,  17,
   6,   6,   6,  19,  19,  19,  19,  20,  20,  15,
  11,   5,   5,  18,  18,  18,   4,   4,   4,   3,
   3,   3,   1,   1,   1,   1,   1,   1,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2
};
short	yyr2[] =
{
   0,   0,   2,   0,   4,   0,   4,   4,   4,   2,
   1,   2,   2,   6,   4,   6,   4,   4,   2,   4,
   5,   6,   3,   3,   3,   3,   4,   4,   6,   4,
   4,   6,   6,   4,   4,   6,   5,   3,   3,   2,
   3,   3,   2,   4,   0,   1,   4,   2,   2,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   4,   1,   4,   1,   4,
   2,   2,   4,   2,   2,   3,   1,   2,   1,   3,
   1,   1,   1,   1,   4,   3,   4,   1,   1,   2,
   1,   1,   4,   4,   5,   7,   0,   2,   2,   1,
   1,   1,   1,   1,   2,   2,   2,   3,   1,   3,
   3,   3,   3,   3,   4,   4,   3,   3,   3
};
short	yychk[] =
{
-1000, -23, -24,  54,  53,  55,  50,  58, -27,   2,
  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
  24,  25,  26,  27,  28,  29,  30,  31,  32,  56,
  56,  57,  57,  58,  58,  58, -20, -11, -15,  -5,
  62,  43,  46, -20,  -8,  -6, -16, -11,  -1, -19,
  62,  34,  55,  10,   9,  63,  60, -18,  53, -28,
  59,  -9,  -8, -22, -13, -14,  39,  40, -12,  41,
  47,  45,  49,  44,  48, -11, -28, -28,  -5,  -6,
  -6, -18,  -1,  60, -18, -12, -12, -12, -28, -28,
 -28,  59,  -7,  -6, -22, -13, -14,  39,  40, -28,
  -7, -25, -26,  -2,  -1,  -2,  59,  -1,  60,  59,
  59,  60,  -1, -19,  11,  52,  51,  10,  -1,  -1,
  -1,  -5,  -2,  60,  -4,   7,   9,  10,  59,  60,
  60,  60,  59, -10, -21,  -1,  53,  54, -17, -18,
  60, -10, -21,  59,  59,  59,  59,  60,  12,  59,
  59,  59, -10,  -1,  53, -16,  -7, -28,  59, -24,
 -24,  58,   9,  10,  11,  12,  13,   7,   8,   6,
   5,   4,  58,  -5, -11,  -2,  -5, -20,  -6,  -5,
  -3,  36,  35,  37,  62,  51,  61,  61,  -5,  60,
   8,  -1,  -1,  -7,  -1,  -1,  -1,  -5,  60, -17,
  -4,  -4,  -5, -21,  -1, -10,  -5, -10, -15,  -1,
  -1, -12, -12,  44,  44,  60,  -4,  -7,  -2,  -2,
  -2,  -2,  -2,   7,   8,  -2,  -2,  -2,  59,  61,
  59,  61,  61, -19,  -1,  61,  -3,  -4,  61,  61,
  61, -28,  59,  38,  61,  60,  59,  59,  59,  59,
 -28,  -2,  -2, -11, -20,  61,  60, -11,  61, -10,
 -15, -16, -12,  36,  61
};
short	yydef[] =
{
   1,  -2,   2,   0,   0,   0,   0,  10,   0,   0,
   0,   0,   0,  44,   0,   0,  44,  44,   0,   0,
   0,   0,   0,   0,   0,  44,  44,  44,  44,   3,
   5,   0,   0,   9,  11,  12,   0,  87,  88,  90,
   0,  91,   0,   0,   0,  61,  62,  80,  81,  82,
   0, 102, 103,   0,   0,   0,   0,  83,  96,  18,
  45,   0,  49,  50,  51,  52,  53,  54,  63,  64,
   0,  66,   0,  68,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  39,  45,  44,  55,  56,  57,  58,  59,  60,  42,
   0,   0,   0,   0, 108,   0,   0,  89,   0,   0,
   0,   0,  70,  71,   0,  73,  74,   0, 104, 105,
 106,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  22,  23,   0,  96,  96,  76,  78,
   0,  24,  25,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  37,   0,  96,  38,  40,  41,   0,   4,
   6,   7,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   8,  90,  14,   0,  90,  16,  17,   0,
   0,  99, 100, 101,   0,  75,  85, 107,   0,   0,
  96,  97,  98,  19,   0,   0,   0,  44,   0,  77,
  47,  48,   0,  26,   0,  27,   0,  29,  30,   0,
   0,  33,  34,  68,  44,   0,  47,  43, 109, 110,
 111, 112, 113,   0,   0, 116, 117, 118,   0,  92,
   0,  86,  93,  72,   0,  84,   0,   0,  65,  67,
  69,  20,  45,   0,  79,   0,   0,   0,   0,   0,
  36, 114, 115,  13,  15,  94,   0,  21,  46,  28,
  31,  32,  35,   0,  95
};
short	yytok1[] =
{
   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  62,  13,   6,   0,
  60,  61,  11,   9,  59,  10,   0,  12,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  56,  58,
   7,  57,   8,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   5,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   4,   0,  63
};
short	yytok2[] =
{
   2,   3,  14,  15,  16,  17,  18,  19,  20,  21,
  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,
  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,
  52,  53,  54,  55
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
#line	37	"a.y"
{
		if(yypt[-1].yyv.sym->value != pc)
			yyerror("redeclaration of %s", yypt[-1].yyv.sym->name);
		yypt[-1].yyv.sym->value = pc;
	} break;
case 5:
#line	44	"a.y"
{
		yypt[-1].yyv.sym->type = LLAB;
		yypt[-1].yyv.sym->value = pc;
	} break;
case 7:
#line	50	"a.y"
{
		yypt[-3].yyv.sym->type = LVAR;
		yypt[-3].yyv.sym->value = yypt[-1].yyv.lval;
	} break;
case 8:
#line	55	"a.y"
{
		if(yypt[-3].yyv.sym->value != yypt[-1].yyv.lval)
			yyerror("redeclaration of %s", yypt[-3].yyv.sym->name);
		yypt[-3].yyv.sym->value = yypt[-1].yyv.lval;
	} break;
case 9:
#line	61	"a.y"
{
		nosched = yypt[-1].yyv.lval;
	} break;
case 13:
#line	73	"a.y"
{
		outcode(yypt[-5].yyv.lval, &yypt[-4].yyv.gen, yypt[-2].yyv.lval, &yypt[-0].yyv.gen);
	} break;
case 14:
#line	77	"a.y"
{
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 15:
#line	84	"a.y"
{
		outcode(yypt[-5].yyv.lval, &yypt[-4].yyv.gen, yypt[-2].yyv.lval, &yypt[-0].yyv.gen);
	} break;
case 16:
#line	88	"a.y"
{
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 17:
#line	95	"a.y"
{
		if(!isreg(&yypt[-2].yyv.gen) && !isreg(&yypt[-0].yyv.gen))
			print("one side must be register\n");
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 18:
#line	104	"a.y"
{
		outcode(yypt[-1].yyv.lval, &nullgen, NREG, &nullgen);
	} break;
case 19:
#line	111	"a.y"
{
		if(!isreg(&yypt[-2].yyv.gen) && !isreg(&yypt[-0].yyv.gen))
			print("one side must be register\n");
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 20:
#line	120	"a.y"
{
		outcode(yypt[-4].yyv.lval, &yypt[-3].yyv.gen, yypt[-1].yyv.lval, &nullgen);
	} break;
case 21:
#line	124	"a.y"
{
		outcode(yypt[-5].yyv.lval, &yypt[-4].yyv.gen, yypt[-2].yyv.lval, &yypt[-0].yyv.gen);
	} break;
case 22:
#line	131	"a.y"
{
		outcode(yypt[-2].yyv.lval, &nullgen, NREG, &yypt[-0].yyv.gen);
	} break;
case 23:
#line	135	"a.y"
{
		outcode(yypt[-2].yyv.lval, &nullgen, NREG, &yypt[-0].yyv.gen);
	} break;
case 24:
#line	139	"a.y"
{
		outcode(yypt[-2].yyv.lval, &nullgen, NREG, &yypt[-0].yyv.gen);
	} break;
case 25:
#line	143	"a.y"
{
		outcode(yypt[-2].yyv.lval, &nullgen, NREG, &yypt[-0].yyv.gen);
	} break;
case 26:
#line	147	"a.y"
{
		outcode(yypt[-3].yyv.lval, &nullgen, yypt[-2].yyv.lval, &yypt[-0].yyv.gen);
	} break;
case 27:
#line	154	"a.y"
{
		if(!isreg(&yypt[-2].yyv.gen))
			print("left side must be register\n");
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 28:
#line	160	"a.y"
{
		if(!isreg(&yypt[-4].yyv.gen))
			print("left side must be register\n");
		outcode(yypt[-5].yyv.lval, &yypt[-4].yyv.gen, yypt[-2].yyv.lval, &yypt[-0].yyv.gen);
	} break;
case 29:
#line	169	"a.y"
{
		if(!isreg(&yypt[-2].yyv.gen))
			print("left side must be register\n");
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 30:
#line	178	"a.y"
{
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 31:
#line	182	"a.y"
{
		outcode(yypt[-5].yyv.lval, &yypt[-4].yyv.gen, yypt[-2].yyv.lval, &yypt[-0].yyv.gen);
	} break;
case 32:
#line	189	"a.y"
{
		outcode(yypt[-5].yyv.lval, &yypt[-4].yyv.gen, yypt[-2].yyv.lval, &yypt[-0].yyv.gen);
	} break;
case 33:
#line	196	"a.y"
{
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 34:
#line	200	"a.y"
{
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 35:
#line	204	"a.y"
{
		outcode(yypt[-5].yyv.lval, &yypt[-4].yyv.gen, yypt[-2].yyv.lval, &yypt[-0].yyv.gen);
	} break;
case 36:
#line	208	"a.y"
{
		outcode(yypt[-4].yyv.lval, &yypt[-3].yyv.gen, yypt[-1].yyv.lval, &nullgen);
	} break;
case 37:
#line	215	"a.y"
{
		outcode(yypt[-2].yyv.lval, &nullgen, NREG, &yypt[-0].yyv.gen);
	} break;
case 38:
#line	222	"a.y"
{
		outcode(yypt[-2].yyv.lval, &nullgen, NREG, &yypt[-0].yyv.gen);
	} break;
case 39:
#line	229	"a.y"
{
		outcode(yypt[-1].yyv.lval, &nullgen, NREG, &nullgen);
	} break;
case 40:
#line	233	"a.y"
{
		outcode(yypt[-2].yyv.lval, &nullgen, NREG, &yypt[-0].yyv.gen);
	} break;
case 41:
#line	237	"a.y"
{
		outcode(yypt[-2].yyv.lval, &yypt[-1].yyv.gen, NREG, &nullgen);
	} break;
case 42:
#line	244	"a.y"
{
		outcode(yypt[-1].yyv.lval, &nullgen, NREG, &nullgen);
	} break;
case 43:
#line	248	"a.y"
{
		outcode(yypt[-3].yyv.lval, &yypt[-2].yyv.gen, NREG, &yypt[-0].yyv.gen);
	} break;
case 46:
#line	257	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_BRANCH;
		yyval.gen.offset = yypt[-3].yyv.lval + pc;
	} break;
case 47:
#line	263	"a.y"
{
		yyval.gen = nullgen;
		if(pass == 2)
			yyerror("undefined label: %s", yypt[-1].yyv.sym->name);
		yyval.gen.type = D_BRANCH;
		yyval.gen.sym = yypt[-1].yyv.sym;
		yyval.gen.offset = yypt[-0].yyv.lval;
	} break;
case 48:
#line	272	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_BRANCH;
		yyval.gen.sym = yypt[-1].yyv.sym;
		yyval.gen.offset = yypt[-1].yyv.sym->value + yypt[-0].yyv.lval;
	} break;
case 53:
#line	285	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_HI;
	} break;
case 54:
#line	290	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_LO;
	} break;
case 59:
#line	301	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_HI;
	} break;
case 60:
#line	306	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_LO;
	} break;
case 64:
#line	320	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_MREG;
		yyval.gen.reg = yypt[-0].yyv.lval;
	} break;
case 65:
#line	326	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_MREG;
		yyval.gen.reg = yypt[-1].yyv.lval;
	} break;
case 66:
#line	334	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCREG;
		yyval.gen.reg = yypt[-0].yyv.lval;
	} break;
case 67:
#line	340	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCREG;
		yyval.gen.reg = yypt[-1].yyv.lval;
	} break;
case 68:
#line	348	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FREG;
		yyval.gen.reg = yypt[-0].yyv.lval;
	} break;
case 69:
#line	354	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FREG;
		yyval.gen.reg = yypt[-1].yyv.lval;
	} break;
case 70:
#line	361	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_CONST;
		yyval.gen.offset = yypt[-0].yyv.lval;
	} break;
case 71:
#line	367	"a.y"
{
		yyval.gen = yypt[-0].yyv.gen;
		yyval.gen.type = D_CONST;
	} break;
case 72:
#line	372	"a.y"
{
		yyval.gen = yypt[-0].yyv.gen;
		yyval.gen.type = D_OCONST;
	} break;
case 73:
#line	377	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_SCONST;
		memcpy(yyval.gen.sval, yypt[-0].yyv.sval, sizeof(yyval.gen.sval));
	} break;
case 74:
#line	383	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCONST;
		yyval.gen.dval = yypt[-0].yyv.dval;
	} break;
case 75:
#line	389	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_FCONST;
		yyval.gen.dval = -yypt[-0].yyv.dval;
	} break;
case 77:
#line	398	"a.y"
{
		if(yypt[-1].yyv.lval != 0)
			yyerror("offset must be zero");
		yyval.gen = yypt[-0].yyv.gen;
	} break;
case 78:
#line	404	"a.y"
{
		yyval.gen = yypt[-0].yyv.gen;
		if(yypt[-0].yyv.gen.name != D_EXTERN && yypt[-0].yyv.gen.name != D_STATIC) {
		}
	} break;
case 79:
#line	412	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_OREG;
		yyval.gen.reg = yypt[-1].yyv.lval;
		yyval.gen.offset = 0;
	} break;
case 81:
#line	422	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_OREG;
		yyval.gen.offset = yypt[-0].yyv.lval;
	} break;
case 84:
#line	432	"a.y"
{
		yyval.gen = yypt[-3].yyv.gen;
		yyval.gen.type = D_OREG;
		yyval.gen.reg = yypt[-1].yyv.lval;
	} break;
case 85:
#line	438	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_OREG;
		yyval.gen.reg = yypt[-1].yyv.lval;
		yyval.gen.offset = 0;
	} break;
case 86:
#line	445	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_OREG;
		yyval.gen.reg = yypt[-1].yyv.lval;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 89:
#line	457	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_CONST;
		yyval.gen.offset = yypt[-0].yyv.lval;
	} break;
case 90:
#line	465	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_REG;
		yyval.gen.reg = yypt[-0].yyv.lval;
	} break;
case 92:
#line	474	"a.y"
{
		if(yyval.lval < 0 || yyval.lval >= NREG)
			print("register value out of range\n");
		yyval.lval = yypt[-1].yyv.lval;
	} break;
case 93:
#line	482	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_OREG;
		yyval.gen.name = yypt[-1].yyv.lval;
		yyval.gen.sym = S;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 94:
#line	490	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_OREG;
		yyval.gen.name = yypt[-1].yyv.lval;
		yyval.gen.sym = yypt[-4].yyv.sym;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 95:
#line	498	"a.y"
{
		yyval.gen = nullgen;
		yyval.gen.type = D_OREG;
		yyval.gen.name = D_STATIC;
		yyval.gen.sym = yypt[-6].yyv.sym;
		yyval.gen.offset = yypt[-3].yyv.lval;
	} break;
case 96:
#line	507	"a.y"
{
		yyval.lval = 0;
	} break;
case 97:
#line	511	"a.y"
{
		yyval.lval = yypt[-0].yyv.lval;
	} break;
case 98:
#line	515	"a.y"
{
		yyval.lval = -yypt[-0].yyv.lval;
	} break;
case 103:
#line	527	"a.y"
{
		yyval.lval = yypt[-0].yyv.sym->value;
	} break;
case 104:
#line	531	"a.y"
{
		yyval.lval = -yypt[-0].yyv.lval;
	} break;
case 105:
#line	535	"a.y"
{
		yyval.lval = yypt[-0].yyv.lval;
	} break;
case 106:
#line	539	"a.y"
{
		yyval.lval = ~yypt[-0].yyv.lval;
	} break;
case 107:
#line	543	"a.y"
{
		yyval.lval = yypt[-1].yyv.lval;
	} break;
case 109:
#line	550	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval + yypt[-0].yyv.lval;
	} break;
case 110:
#line	554	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval - yypt[-0].yyv.lval;
	} break;
case 111:
#line	558	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval * yypt[-0].yyv.lval;
	} break;
case 112:
#line	562	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval / yypt[-0].yyv.lval;
	} break;
case 113:
#line	566	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval % yypt[-0].yyv.lval;
	} break;
case 114:
#line	570	"a.y"
{
		yyval.lval = yypt[-3].yyv.lval << yypt[-0].yyv.lval;
	} break;
case 115:
#line	574	"a.y"
{
		yyval.lval = yypt[-3].yyv.lval >> yypt[-0].yyv.lval;
	} break;
case 116:
#line	578	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval & yypt[-0].yyv.lval;
	} break;
case 117:
#line	582	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval ^ yypt[-0].yyv.lval;
	} break;
case 118:
#line	586	"a.y"
{
		yyval.lval = yypt[-2].yyv.lval | yypt[-0].yyv.lval;
	} break;
	}
	goto yystack;  /* stack new state and value */
}
