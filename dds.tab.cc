/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse ddsparse
#define yylex   ddslex
#define yyerror ddserror
#define yylval  ddslval
#define yychar  ddschar
#define yydebug ddsdebug
#define yynerrs ddsnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SCAN_WORD = 258,
     SCAN_DATASET = 259,
     SCAN_LIST = 260,
     SCAN_SEQUENCE = 261,
     SCAN_STRUCTURE = 262,
     SCAN_FUNCTION = 263,
     SCAN_GRID = 264,
     SCAN_BYTE = 265,
     SCAN_INT16 = 266,
     SCAN_UINT16 = 267,
     SCAN_INT32 = 268,
     SCAN_UINT32 = 269,
     SCAN_FLOAT32 = 270,
     SCAN_FLOAT64 = 271,
     SCAN_STRING = 272,
     SCAN_URL = 273
   };
#endif
#define SCAN_WORD 258
#define SCAN_DATASET 259
#define SCAN_LIST 260
#define SCAN_SEQUENCE 261
#define SCAN_STRUCTURE 262
#define SCAN_FUNCTION 263
#define SCAN_GRID 264
#define SCAN_BYTE 265
#define SCAN_INT16 266
#define SCAN_UINT16 267
#define SCAN_INT32 268
#define SCAN_UINT32 269
#define SCAN_FLOAT32 270
#define SCAN_FLOAT64 271
#define SCAN_STRING 272
#define SCAN_URL 273




/* Copy the first part of user declarations.  */
#line 47 "dds.y"


#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <stack>
#include <sstream>

#include "DDS.h"
#include "Array.h"
#include "Error.h"
#include "parser.h"
#include "util.h"

 using namespace std;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a structure (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 
// ERROR is no longer used. These parsers now signal problems by throwing
// exceptions. 5/22/2002 jhrg
#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)

#define YYPARSE_PARAM arg

extern int dds_line_num;	/* defined in dds.lex */

// No global static objects in the dap library! 1/24/2000 jhrg
static stack<BaseType *> *ctor;
static BaseType *current;
static string *id;
static Part part = nil;		/* Part is defined in BaseType */

static char *NO_DDS_MSG =
"The descriptor object returned from the dataset was null.\n\
Check that the URL is correct.";

static char *BAD_DECLARATION =
"In the dataset descriptor object: Expected a variable declaration\n\
(e.g., Int32 i;). Make sure that the variable name is not the name\n\
of a datatype and that the Array: and Maps: sections of a Grid are\n\
labeled properly.";
 
int ddslex();
void ddserror(char *s);
void error_exit_cleanup();
void add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, 
	       Part p);
void invalid_declaration(parser_arg *arg, string semantic_err_msg, 
			 char *type, char *name);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 109 "dds.y"
typedef union YYSTYPE {
    bool boolean;
    char word[ID_MAX];
} YYSTYPE;
/* Line 190 of yacc.c.  */
#line 185 "dds.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 197 "dds.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   194

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  22
/* YYNRULES -- Number of rules. */
#define YYNRULES  56
/* YYNRULES -- Number of states. */
#define YYNSTATES  88

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   273

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    22,    21,
       2,    25,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    23,     2,    24,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    19,     2,    20,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    12,    19,    21,    22,
      24,    27,    31,    32,    40,    41,    49,    50,    51,    52,
      67,    69,    71,    73,    75,    77,    79,    81,    83,    85,
      87,    89,    91,    93,    95,    98,   100,   102,   104,   106,
     108,   110,   112,   114,   116,   118,   120,   122,   124,   126,
     130,   131,   132,   140,   142,   144,   146
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      27,     0,    -1,    -1,    28,    29,    -1,    30,    -1,    29,
      30,    -1,     4,    19,    31,    20,    47,    21,    -1,     1,
      -1,    -1,    32,    -1,    31,    32,    -1,    41,    42,    21,
      -1,    -1,    38,    19,    31,    20,    33,    42,    21,    -1,
      -1,    39,    19,    31,    20,    34,    42,    21,    -1,    -1,
      -1,    -1,    40,    19,     3,    22,    35,    32,     3,    22,
      36,    31,    20,    37,    42,    21,    -1,     1,    -1,     7,
      -1,     6,    -1,     9,    -1,    10,    -1,    11,    -1,    12,
      -1,    13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,
      -1,    18,    -1,    43,    -1,    42,    44,    -1,     3,    -1,
      10,    -1,    11,    -1,    13,    -1,    12,    -1,    14,    -1,
      15,    -1,    16,    -1,    17,    -1,    18,    -1,     7,    -1,
       6,    -1,     9,    -1,     5,    -1,    23,     3,    24,    -1,
      -1,    -1,    23,     3,    45,    25,     3,    46,    24,    -1,
       1,    -1,    43,    -1,     4,    -1,     1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   138,   138,   138,   148,   149,   152,   156,   166,   170,
     171,   178,   200,   199,   219,   218,   238,   250,   262,   237,
     282,   293,   299,   305,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   322,   323,   326,   326,   326,   326,   326,
     327,   327,   327,   327,   328,   328,   328,   328,   329,   332,
     354,   358,   353,   382,   393,   394,   395
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SCAN_WORD", "SCAN_DATASET", "SCAN_LIST",
  "SCAN_SEQUENCE", "SCAN_STRUCTURE", "SCAN_FUNCTION", "SCAN_GRID",
  "SCAN_BYTE", "SCAN_INT16", "SCAN_UINT16", "SCAN_INT32", "SCAN_UINT32",
  "SCAN_FLOAT32", "SCAN_FLOAT64", "SCAN_STRING", "SCAN_URL", "'{'", "'}'",
  "';'", "':'", "'['", "']'", "'='", "$accept", "start", "@1", "datasets",
  "dataset", "declarations", "declaration", "@2", "@3", "@4", "@5", "@6",
  "structure", "sequence", "grid", "base_type", "var", "var_name",
  "array_decl", "@7", "@8", "name", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   123,
     125,    59,    58,    91,    93,    61
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    26,    28,    27,    29,    29,    30,    30,    31,    31,
      31,    32,    33,    32,    34,    32,    35,    36,    37,    32,
      32,    38,    39,    40,    41,    41,    41,    41,    41,    41,
      41,    41,    41,    42,    42,    43,    43,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    43,    44,
      45,    46,    44,    44,    47,    47,    47
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     2,     6,     1,     0,     1,
       2,     3,     0,     7,     0,     7,     0,     0,     0,    14,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       0,     0,     7,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     0,     1,     7,     0,     0,     4,     0,     5,
      20,    22,    21,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,     0,     9,     0,     0,     0,     0,     0,
      10,     0,     0,     0,    35,    48,    46,    45,    47,    36,
      37,    39,    38,    40,    41,    42,    43,    44,     0,    33,
      56,    55,    54,     0,     0,     0,     0,    53,    11,     0,
      34,     6,    12,    14,    16,    50,     0,     0,     0,    49,
       0,     0,     0,     0,     0,    13,    15,     0,    51,    17,
       0,     0,    52,     0,    18,     0,     0,    19
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     1,     2,     6,     7,    23,    24,    66,    67,    68,
      81,    85,    25,    26,    27,    28,    48,    49,    60,    70,
      80,    53
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -53
static const short int yypact[] =
{
     -53,    11,     9,   -53,   -53,    -7,     5,   -53,    52,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   -53,    70,   -53,    -3,    -2,    -1,   176,   142,
     -53,    52,    52,    16,   -53,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,     2,   -53,
     -53,   -53,   -53,     0,    88,   106,    12,   -53,   -53,    17,
     -53,   -53,   -53,   -53,   -53,    13,   176,   176,   160,   -53,
      10,     3,     6,    19,    33,   -53,   -53,    18,   -53,   -53,
      14,    52,   -53,   124,   -53,   176,     7,   -53
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -53,   -53,   -53,   -53,    35,   -30,   -23,   -53,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,   -52,    15,   -53,   -53,
     -53,   -53
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -9
static const yysigned_char yytable[] =
{
      30,    54,    55,    57,    57,    -3,     4,    57,    57,     5,
       4,     3,     8,     5,    71,    72,    31,    32,    33,    56,
      65,    61,    77,    58,    75,    59,    59,    76,    87,    59,
      59,    30,    30,    86,    64,    74,    78,    69,    82,     0,
      79,     9,     0,     0,    52,    73,     0,     0,     0,     0,
       0,    83,     0,    10,     0,     0,     0,     0,    11,    12,
      30,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    10,    -8,     0,     0,     0,    11,    12,     0,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    10,
      29,     0,     0,     0,    11,    12,     0,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    10,    62,     0,
       0,     0,    11,    12,     0,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    10,    63,     0,     0,     0,
      11,    12,     0,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    50,    84,    34,    51,    35,    36,    37,
       0,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    10,     0,     0,     0,     0,    11,    12,     0,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    34,
       0,    35,    36,    37,     0,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47
};

static const yysigned_char yycheck[] =
{
      23,    31,    32,     1,     1,     0,     1,     1,     1,     4,
       1,     0,    19,     4,    66,    67,    19,    19,    19,     3,
       3,    21,     3,    21,    21,    23,    23,    21,    21,    23,
      23,    54,    55,    85,    22,    25,     3,    24,    24,    -1,
      22,     6,    -1,    -1,    29,    68,    -1,    -1,    -1,    -1,
      -1,    81,    -1,     1,    -1,    -1,    -1,    -1,     6,     7,
      83,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     1,    20,    -1,    -1,    -1,     6,     7,    -1,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,     1,
      20,    -1,    -1,    -1,     6,     7,    -1,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,     1,    20,    -1,
      -1,    -1,     6,     7,    -1,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     1,    20,    -1,    -1,    -1,
       6,     7,    -1,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,     1,    20,     3,     4,     5,     6,     7,
      -1,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     1,    -1,    -1,    -1,    -1,     6,     7,    -1,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,     3,
      -1,     5,     6,     7,    -1,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    27,    28,     0,     1,     4,    29,    30,    19,    30,
       1,     6,     7,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    31,    32,    38,    39,    40,    41,    20,
      32,    19,    19,    19,     3,     5,     6,     7,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    42,    43,
       1,     4,    43,    47,    31,    31,     3,     1,    21,    23,
      44,    21,    20,    20,    22,     3,    33,    34,    35,    24,
      45,    42,    42,    32,    25,    21,    21,     3,     3,    22,
      46,    36,    24,    31,    20,    37,    42,    21
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  yyvsp[0] = yylval;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 138 "dds.y"
    {
		    /* On entry to the parser, make the BaseType stack. */
		    ctor = new stack<BaseType *>;
                ;}
    break;

  case 3:
#line 143 "dds.y"
    {
		    delete ctor; ctor = 0;
		;}
    break;

  case 6:
#line 153 "dds.y"
    {
		    (yyval.boolean) = (yyvsp[-3].boolean) && (yyvsp[-1].word);
		;}
    break;

  case 7:
#line 157 "dds.y"
    {
		    parse_error((parser_arg *)arg, NO_DDS_MSG,
 				dds_line_num, (yyvsp[0].word));
		    error_exit_cleanup();
		    YYABORT;
		;}
    break;

  case 8:
#line 166 "dds.y"
    {
		    (yyval.boolean) = true;
		;}
    break;

  case 9:
#line 170 "dds.y"
    { (yyval.boolean) = true; ;}
    break;

  case 10:
#line 171 "dds.y"
    { (yyval.boolean) = true; ;}
    break;

  case 11:
#line 179 "dds.y"
    { 
		    string smsg;
		    if (current->check_semantics(smsg)) {
			/* BaseType *current_save = current ; */
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
			/* FIX
			if( current_save == current )
			{
			    delete current ;
			    current = 0 ;
			}
			*/
		    } else {
		      invalid_declaration((parser_arg *)arg, smsg, (yyvsp[-2].word), (yyvsp[-1].word));
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strcpy((yyval.word),(yyvsp[-1].word));
		;}
    break;

  case 12:
#line 200 "dds.y"
    { 
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		;}
    break;

  case 13:
#line 206 "dds.y"
    { 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, (yyvsp[-6].word), (yyvsp[-1].word));
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strcpy((yyval.word),(yyvsp[-1].word));
		;}
    break;

  case 14:
#line 219 "dds.y"
    { 
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		;}
    break;

  case 15:
#line 225 "dds.y"
    { 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, (yyvsp[-6].word), (yyvsp[-1].word));
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strcpy((yyval.word),(yyvsp[-1].word));
		;}
    break;

  case 16:
#line 238 "dds.y"
    { 
		    if (is_keyword(string((yyvsp[-1].word)), "array"))
			part = array; 
		    else {
			ostringstream msg;
			msg << BAD_DECLARATION;
			parse_error((parser_arg *)arg, msg.str().c_str(),
				    dds_line_num, (yyvsp[-1].word));
			YYABORT;
		    }
                ;}
    break;

  case 17:
#line 250 "dds.y"
    { 
		    if (is_keyword(string((yyvsp[-1].word)), "maps"))
			part = maps; 
		    else {
			ostringstream msg;
			msg << BAD_DECLARATION;
			parse_error((parser_arg *)arg, msg.str().c_str(),
				    dds_line_num, (yyvsp[-1].word));
			YYABORT;
		    }
                ;}
    break;

  case 18:
#line 262 "dds.y"
    {
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		;}
    break;

  case 19:
#line 268 "dds.y"
    {
		    string smsg;
		    if (current->check_semantics(smsg)) {
			part = nil; 
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    }
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, (yyvsp[-13].word), (yyvsp[-1].word));
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strcpy((yyval.word),(yyvsp[-1].word));
		;}
    break;

  case 20:
#line 283 "dds.y"
    {
		    ostringstream msg;
		    msg << BAD_DECLARATION;
		    parse_error((parser_arg *)arg, msg.str().c_str(),
				dds_line_num, (yyvsp[0].word));
		    YYABORT;
		;}
    break;

  case 21:
#line 294 "dds.y"
    { 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewStructure()); 
		;}
    break;

  case 22:
#line 300 "dds.y"
    { 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewSequence()); 
		;}
    break;

  case 23:
#line 306 "dds.y"
    { 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewGrid()); 
		;}
    break;

  case 24:
#line 311 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewByte(); ;}
    break;

  case 25:
#line 312 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewInt16(); ;}
    break;

  case 26:
#line 313 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUInt16(); ;}
    break;

  case 27:
#line 314 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewInt32(); ;}
    break;

  case 28:
#line 315 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUInt32(); ;}
    break;

  case 29:
#line 316 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewFloat32(); ;}
    break;

  case 30:
#line 317 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewFloat64(); ;}
    break;

  case 31:
#line 318 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewStr(); ;}
    break;

  case 32:
#line 319 "dds.y"
    { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUrl(); ;}
    break;

  case 33:
#line 322 "dds.y"
    { current->set_name((yyvsp[0].word)); ;}
    break;

  case 49:
#line 333 "dds.y"
    { 
		     if (!check_int32((yyvsp[-1].word))) {
			 string msg = "In the dataset descriptor object:\n";
			 msg += "Expected an array subscript.\n";
			 parse_error((parser_arg *)arg, msg.c_str(), 
				 dds_line_num, (yyvsp[-1].word));
		     }
		     if (current->type() == dods_array_c
			 && check_int32((yyvsp[-1].word))) {
			 ((Array *)current)->append_dim(atoi((yyvsp[-1].word)));
		     }
		     else {
			 Array *a = DDS_OBJ(arg)->get_factory()->NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi((yyvsp[-1].word)));
			 if( current ) delete current ;
			 current = a;
		     }
		 ;}
    break;

  case 50:
#line 354 "dds.y"
    {
		     id = new string((yyvsp[0].word));
		 ;}
    break;

  case 51:
#line 358 "dds.y"
    { 
		     if (!check_int32((yyvsp[0].word))) {
			 string msg = "In the dataset descriptor object:\n";
			 msg += "Expected an array subscript.\n";
			 parse_error((parser_arg *)arg, msg.c_str(), 
				 dds_line_num, (yyvsp[0].word));
			 error_exit_cleanup();
			 YYABORT;
		     }
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi((yyvsp[0].word)), *id);
		     }
		     else {
			 Array *a = DDS_OBJ(arg)->get_factory()->NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi((yyvsp[0].word)), *id);
			 if( current ) delete current ;
			 current = a;
		     }

		     delete id; id = 0;
		 ;}
    break;

  case 53:
#line 383 "dds.y"
    {
		     ostringstream msg;
		     msg << "In the dataset descriptor object:" << endl
			 << "Expected an array subscript." << endl;
		     parse_error((parser_arg *)arg, msg.str().c_str(), 
				 dds_line_num, (yyvsp[0].word));
		     YYABORT;
		 ;}
    break;

  case 54:
#line 393 "dds.y"
    { (*DDS_OBJ(arg)).set_dataset_name((yyvsp[0].word)); ;}
    break;

  case 55:
#line 394 "dds.y"
    { (*DDS_OBJ(arg)).set_dataset_name((yyvsp[0].word)); ;}
    break;

  case 56:
#line 396 "dds.y"
    {
		  ostringstream msg;
		  msg << "Error parsing the dataset name." << endl
		      << "The name may be missing or may contain an illegal character." << endl;
		     parse_error((parser_arg *)arg, msg.str().c_str(),
				 dds_line_num, (yyvsp[0].word));
		     YYABORT;
		;}
    break;


    }

/* Line 1037 of yacc.c.  */
#line 1549 "dds.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {

		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 yydestruct ("Error: popping",
                             yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yydestruct ("Error: discarding lookahead",
              yytoken, &yylval);
  yychar = YYEMPTY;
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 406 "dds.y"


/* 
   This function must be defined. However, use the error reporting code in
   parser-utils.cc.
*/

void 
ddserror(char *)
{
}

/*
  Error clean up. Call this before calling YYBORT. Don't call this on a
  normal exit.
*/

void
error_exit_cleanup()
{
    delete id; id = 0;
    delete current; current = 0;
    delete ctor; ctor = 0;
}

/*
  Invalid declaration message.
*/

void
invalid_declaration(parser_arg *arg, string semantic_err_msg, char *type, 
		    char *name)
{
  ostringstream msg;
  msg << "In the dataset descriptor object: `" << type << " " << name 
      << "'" << endl << "is not a valid declaration." << endl 
      << semantic_err_msg;
  parse_error((parser_arg *)arg, msg.str().c_str(), dds_line_num);
}

/*
  Add the variable pointed to by CURRENT to either the topmost ctor object on
  the stack CTOR or to the dataset variable table TABLE if CTOR is empty.  If
  it exists, the current ctor object is popped off the stack and assigned to
  CURRENT.

  NB: the ctor stack is popped for arrays because they are ctors which
  contain only a single variable. For other ctor types, several variables may
  be members and the parse rule (see `declaration' above) determines when to
  pop the stack.

  Returns: void 
*/

void	
add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, Part part)
{ 
    if (!*ctor)
	*ctor = new stack<BaseType *>;

    if (!(*ctor)->empty()) { /* must be parsing a ctor type */
	(*ctor)->top()->add_var(*current, part);

 	const Type &ctor_type = (*ctor)->top()->type();

	if (ctor_type == dods_array_c) {
	    if( *current ) delete *current ;
	    *current = (*ctor)->top();
	    (*ctor)->pop();

	    // Return here to avoid deleting the new value of 'current.'
	    return;
	}
    }
    else {
	table.add_var(*current);
    }

    if (*current) 
	delete *current; 
    *current = 0;
}


