/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "Pattern.h"

using namespace std;


#define YYSTYPE std::string
#define YYDEBUG 1


/*--- Constants -----------------------------------------------------------*/

const char* copyright =
  "/****************************************************************************\n"
  "**  SCALASCA    http://www.scalasca.org/                                   **\n"
  "*****************************************************************************\n"
  "**  Copyright (c) 1998-2016                                                **\n"
  "**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **\n"
  "**                                                                         **\n"
  "**  Copyright (c) 2009-2014                                                **\n"
  "**  German Research School for Simulation Sciences GmbH,                   **\n"
  "**  Laboratory for Parallel Programming                                    **\n"
  "**                                                                         **\n"
  "**  This software may be modified and distributed under the terms of       **\n"
  "**  a BSD-style license.  See the COPYING file in the package base         **\n"
  "**  directory for details.                                                 **\n"
  "****************************************************************************/\n"
  "\n\n";


/*--- Global variables ----------------------------------------------------*/

extern FILE* yyin;

long lineno = 1;
long depth  = 0;

string inpFilename;
string incFilename;
int    incLine;

string prolog;
string prefix("Patterns_gen");

map<string,Pattern*> id2pattern;    // pattern id |-> pattern
vector<Pattern*>     pattern;       // pattern list
Pattern*             current;       // current pattern
string               callbackgroup; // current callback group

/*--- Function prototypes -------------------------------------------------*/

int main(int argc, char** argv);

void write_header();
void write_impl();
void write_html();

string uppercase(const string& str);

void yyerror(const string& message);

extern int  yylex();
extern void include_file(const string& filename);




/* Line 268 of yacc.c  */
#line 167 "ScoutPatternParser.cc"

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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     STRING = 258,
     TEXT = 259,
     CALLBACKS = 260,
     CLASS = 261,
     CLEANUP = 262,
     CONDITION = 263,
     DATA = 264,
     DESCR = 265,
     DOCNAME = 266,
     DIAGNOSIS = 267,
     HIDDEN = 268,
     INCLUDE = 269,
     INFO = 270,
     INIT = 271,
     MODE = 272,
     NAME = 273,
     NODOCS = 274,
     PARENT = 275,
     PATTERN = 276,
     PROLOG = 277,
     STATICINIT = 278,
     TYPE = 279,
     UNIT = 280
   };
#endif
/* Tokens.  */
#define STRING 258
#define TEXT 259
#define CALLBACKS 260
#define CLASS 261
#define CLEANUP 262
#define CONDITION 263
#define DATA 264
#define DESCR 265
#define DOCNAME 266
#define DIAGNOSIS 267
#define HIDDEN 268
#define INCLUDE 269
#define INFO 270
#define INIT 271
#define MODE 272
#define NAME 273
#define NODOCS 274
#define PARENT 275
#define PATTERN 276
#define PROLOG 277
#define STATICINIT 278
#define TYPE 279
#define UNIT 280




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 259 "ScoutPatternParser.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  15
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   117

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  61
/* YYNRULES -- Number of states.  */
#define YYNSTATES  118

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   280

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    31,     2,     2,     2,     2,     2,
      29,    30,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    26,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    27,     2,    28,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,    33,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    12,    14,    16,    19,
      22,    23,    31,    34,    36,    38,    40,    42,    44,    46,
      48,    50,    52,    54,    56,    58,    60,    62,    64,    66,
      68,    70,    72,    76,    80,    84,    88,    92,    94,    96,
     100,   104,   108,   112,   116,   120,   124,   128,   132,   136,
     137,   147,   148,   155,   158,   160,   164,   168,   169,   174,
     178,   181
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      35,     0,    -1,    36,    -1,    37,    -1,    36,    37,    -1,
      38,    -1,    40,    -1,    39,    -1,    14,    66,    -1,    22,
      67,    -1,    -1,    21,    66,    41,    26,    27,    42,    28,
      -1,    42,    43,    -1,    43,    -1,    44,    -1,    45,    -1,
      46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,
      51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,    -1,
      56,    -1,    57,    -1,    58,    -1,    59,    -1,    60,    -1,
      61,    -1,    18,    26,    66,    -1,     6,    26,    66,    -1,
      11,    26,    66,    -1,    20,    26,    66,    -1,    24,    26,
      66,    -1,    13,    -1,    19,    -1,    15,    26,    66,    -1,
      10,    26,    69,    -1,    12,    26,    69,    -1,    25,    26,
      66,    -1,    17,    26,    66,    -1,     8,    26,    66,    -1,
      16,    26,    67,    -1,    23,    26,    67,    -1,     7,    26,
      67,    -1,     9,    26,    67,    -1,    -1,     5,    29,    66,
      30,    62,    26,    27,    64,    28,    -1,    -1,     5,    63,
      26,    27,    64,    28,    -1,    64,    65,    -1,    65,    -1,
      66,    26,    67,    -1,    31,     3,    31,    -1,    -1,    32,
      68,    70,    33,    -1,    32,    70,    33,    -1,    70,     4,
      -1,     4,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   105,   105,   109,   110,   114,   115,   116,   120,   127,
     135,   134,   172,   173,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   198,   208,   218,   225,   242,   257,   264,   271,
     281,   291,   301,   311,   323,   333,   343,   353,   363,   374,
     373,   379,   378,   386,   387,   391,   398,   406,   405,   421,
     428,   432
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRING", "TEXT", "CALLBACKS", "CLASS",
  "CLEANUP", "CONDITION", "DATA", "DESCR", "DOCNAME", "DIAGNOSIS",
  "HIDDEN", "INCLUDE", "INFO", "INIT", "MODE", "NAME", "NODOCS", "PARENT",
  "PATTERN", "PROLOG", "STATICINIT", "TYPE", "UNIT", "'='", "'['", "']'",
  "'('", "')'", "'\"'", "'{'", "'}'", "$accept", "File", "Body",
  "BodyItem", "Include", "Prolog", "Pattern", "$@1", "PatternDef",
  "DefItem", "Name", "Classname", "Docname", "Parent", "Type", "Hidden",
  "NoDocs", "Info", "Description", "Diagnosis", "Unit", "Mode",
  "Condition", "Init", "StaticInit", "Cleanup", "Data", "Callbacks", "$@2",
  "$@3", "CbList", "CbItem", "String", "CodeBlock", "@4", "TextBlock",
  "Text", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,    61,    91,    93,    40,
      41,    34,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    34,    35,    36,    36,    37,    37,    37,    38,    39,
      41,    40,    42,    42,    43,    43,    43,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
      43,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    62,
      61,    63,    61,    64,    64,    65,    66,    68,    67,    69,
      70,    70
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     2,     2,
       0,     7,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     0,
       9,     0,     6,     2,     1,     3,     3,     0,     4,     3,
       2,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     2,     3,     5,     7,     6,
       0,     8,    10,    57,     9,     1,     4,     0,     0,     0,
      56,     0,    61,     0,     0,    60,    58,    51,     0,     0,
       0,     0,     0,     0,     0,    37,     0,     0,     0,     0,
      38,     0,     0,     0,     0,     0,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    11,    12,     0,     0,    33,    47,    44,    48,
       0,    40,    34,    41,    39,    45,    43,    32,    35,    46,
      36,    42,    49,     0,     0,     0,     0,    54,     0,    59,
       0,    52,    53,     0,     0,    55,     0,    50
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,     6,     7,     8,     9,    18,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,   105,    66,
     106,   107,   108,    14,    19,    91,    23
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -100
static const yytype_int8 yypact[] =
{
     -11,   -23,   -23,   -18,    16,   -11,  -100,  -100,  -100,  -100,
      24,  -100,  -100,  -100,  -100,  -100,  -100,     9,    17,    40,
    -100,    18,  -100,     1,    76,  -100,  -100,    19,    20,    23,
      25,    26,    27,    28,    29,  -100,    30,    31,    32,    33,
    -100,    34,    35,    36,    37,    13,  -100,  -100,  -100,  -100,
    -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,
    -100,  -100,  -100,  -100,  -100,   -23,    39,   -23,   -18,   -23,
     -18,    38,   -23,    38,   -23,   -18,   -23,   -23,   -23,   -18,
     -23,   -23,  -100,  -100,    42,    47,  -100,  -100,  -100,  -100,
      40,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,
    -100,  -100,  -100,   -23,     2,    41,   -16,  -100,    43,  -100,
      51,  -100,  -100,   -18,   -23,  -100,    11,  -100
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -100,  -100,  -100,    45,  -100,  -100,  -100,  -100,  -100,    52,
    -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,
    -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,  -100,
     -24,   -99,    -1,   -66,  -100,    44,     8
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      11,    12,    87,     1,    89,    25,    25,   112,    10,    95,
       2,     3,   111,    99,    13,    10,    15,   112,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    17,    36,    37,
      38,    39,    40,    41,    26,   109,    42,    43,    44,   117,
      20,    82,    10,    21,    22,    24,    67,   115,    65,    68,
      16,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    84,    85,    86,   110,    88,   113,
      90,    92,   102,    94,   103,    96,    97,    98,   114,   100,
     101,    27,    28,    29,    30,    31,    32,    33,    34,    35,
     116,    36,    37,    38,    39,    40,    41,    83,   104,    42,
      43,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    93
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-100))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
       1,     2,    68,    14,    70,     4,     4,   106,    31,    75,
      21,    22,    28,    79,    32,    31,     0,   116,     5,     6,
       7,     8,     9,    10,    11,    12,    13,     3,    15,    16,
      17,    18,    19,    20,    33,    33,    23,    24,    25,    28,
      31,    28,    31,    26,     4,    27,    26,   113,    29,    26,
       5,    26,    26,    26,    26,    26,    26,    26,    26,    26,
      26,    26,    26,    26,    65,    26,    67,    26,    69,    26,
      32,    72,    30,    74,    27,    76,    77,    78,    27,    80,
      81,     5,     6,     7,     8,     9,    10,    11,    12,    13,
     114,    15,    16,    17,    18,    19,    20,    45,    90,    23,
      24,    25,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    73
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,    21,    22,    35,    36,    37,    38,    39,    40,
      31,    66,    66,    32,    67,     0,    37,     3,    41,    68,
      31,    26,     4,    70,    27,     4,    33,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    15,    16,    17,    18,
      19,    20,    23,    24,    25,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    29,    63,    26,    26,    26,
      26,    26,    26,    26,    26,    26,    26,    26,    26,    26,
      26,    26,    28,    43,    66,    26,    66,    67,    66,    67,
      32,    69,    66,    69,    66,    67,    66,    66,    66,    67,
      66,    66,    30,    27,    70,    62,    64,    65,    66,    33,
      26,    28,    65,    26,    27,    67,    64,    28
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
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
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  YYUSE (yyvaluep);

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
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
        case 8:

/* Line 1806 of yacc.c  */
#line 121 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    include_file((yyvsp[(2) - (2)]));
                  }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 128 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    prolog += (yyvsp[(2) - (2)]);
                  }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 135 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (id2pattern.find((yyvsp[(2) - (2)])) != id2pattern.end())
                      yyerror("Pattern \"" + (yyvsp[(2) - (2)]) + "\" already defined!");

                    current = new Pattern((yyvsp[(2) - (2)]));
                    pattern.push_back(current);
                    id2pattern.insert(make_pair((yyvsp[(2) - (2)]), current));
                  }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 144 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    bool error = true;
                    string msg = "Incomplete pattern definition!\n";
                    switch(current->is_valid()) {
                      case DEF_NAME:
                        msg += "Missing NAME definition.";
                        break;
                      case DEF_CLASS:
                        msg += "Missing CLASS definition.";
                        break;
                      case DEF_INFO:
                        msg += "Missing INFO definition.";
                        break;
                      case DEF_UNIT:
                        msg += "Missing UNIT definition.";
                        break;
                      case DEF_MODE:
                        msg += "Missing MODE definition.";
                        break;
                      default:
                        error = false;
                        break;
                    }
                    if (error)
                      yyerror(msg);
                  }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 199 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_name().empty())
                      yyerror("Only one NAME definition allowed!");

                    current->set_name((yyvsp[(3) - (3)]));
                  }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 209 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_classname().empty())
                      yyerror("Only one CLASS definition allowed!");

                    current->set_classname((yyvsp[(3) - (3)]));
                  }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 219 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    current->set_docname((yyvsp[(3) - (3)]));
                  }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 226 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (current->get_parent() != "NONE")
                      yyerror("Only one PARENT definition allowed!");


                    map<string,Pattern*>::iterator it = id2pattern.find((yyvsp[(3) - (3)]));
                    if (it == id2pattern.end())
                      yyerror("Unknown pattern \"" + (yyvsp[(3) - (3)]) + "\"!");
                    if (it->second == current)
                      yyerror("A pattern cannot be its own parent!");

                    current->set_parent((yyvsp[(3) - (3)]));
                  }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 243 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if ((yyvsp[(3) - (3)]) != "MPI" &&
                        (yyvsp[(3) - (3)]) != "MPIDEP" &&
                        (yyvsp[(3) - (3)]) != "MPI_RMA" &&
                        (yyvsp[(3) - (3)]) != "OMP" &&
                        (yyvsp[(3) - (3)]) != "OMPDEP" &&
                        (yyvsp[(3) - (3)]) != "Generic")
                      yyerror("Unknown pattern type \"" + (yyvsp[(3) - (3)]) + "\"");

                    current->set_type((yyvsp[(3) - (3)]));
                  }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 258 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    current->set_hidden();
                  }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 265 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    current->set_nodocs();
                  }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 272 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_info().empty())
                      yyerror("Only one INFO definition allowed!");

                    current->set_info((yyvsp[(3) - (3)]));
                  }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 282 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_descr().empty())
                      yyerror("Only one DESCR definition allowed!");

                    current->set_descr((yyvsp[(3) - (3)]));
                  }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 292 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_diagnosis().empty())
                      yyerror("Only one DIAGNOSIS definition allowed!");

                    current->set_diagnosis((yyvsp[(3) - (3)]));
                  }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 302 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_unit().empty())
                      yyerror("Only one UNIT definition allowed!");

                    current->set_unit((yyvsp[(3) - (3)]));
                  }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 312 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_mode().empty())
                      yyerror("Only one MODE definition allowed!");
                    if ((yyvsp[(3) - (3)]) != "inclusive" &&
                        (yyvsp[(3) - (3)]) != "exclusive")
                      yyerror("Unknown pattern mode \"" + (yyvsp[(3) - (3)]) + "\"");

                    current->set_mode((yyvsp[(3) - (3)]));
                  }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 324 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_condition().empty())
                      yyerror("Only one CONDITION definition allowed!");

                    current->set_condition((yyvsp[(3) - (3)]));
                  }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 334 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_init().empty())
                      yyerror("Only one INIT definition allowed!");

                    current->set_init((yyvsp[(3) - (3)]));
                  }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 344 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_staticinit().empty())
                      yyerror("Only one STATICINIT definition allowed!");

                    current->set_staticinit((yyvsp[(3) - (3)]));
                  }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 354 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_cleanup().empty())
                      yyerror("Only one CLEANUP definition allowed!");

                    current->set_cleanup((yyvsp[(3) - (3)]));
                  }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 364 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->get_data().empty())
                      yyerror("Only one DATA definition allowed!");

                    current->set_data((yyvsp[(3) - (3)]));
                  }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 374 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    callbackgroup = (yyvsp[(3) - (4)]);
                  }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 379 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    callbackgroup = "";
                  }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 392 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    if (!current->add_callback(callbackgroup, (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])))
                      yyerror("Callback \"" + (yyvsp[(1) - (3)]) + "\" already defined!");
                  }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 399 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    (yyval) = (yyvsp[(2) - (3)]);
                  }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 406 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    char line_str[32];
                    snprintf(line_str, 32, "\n#line %ld \"", lineno);

                    (yyval) = line_str +
                         (incFilename.empty() ? inpFilename : incFilename) +
                         "\"\n";
                  }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 415 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    (yyval) = (yyvsp[(2) - (4)]) + (yyvsp[(3) - (4)]);
                  }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 422 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    (yyval) = (yyvsp[(2) - (3)]);
                  }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 429 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    (yyval) = (yyvsp[(1) - (2)]) + (yyvsp[(2) - (2)]);
                  }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 433 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"
    {
                    (yyval) = (yyvsp[(1) - (1)]);
                  }
    break;



/* Line 1806 of yacc.c  */
#line 1947 "ScoutPatternParser.cc"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
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
      if (!yypact_value_is_default (yyn))
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
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
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 439 "../../build-backend/../src/scout/generator/ScoutPatternParser.yy"



/*
 *---------------------------------------------------------------------------
 *
 * gen_pattern <description file>
 *
 *---------------------------------------------------------------------------
 */

int main(int argc, char** argv)
{
  /* Check command line arguments */
  if (argc != 2) {
    fprintf(stderr, "Usage: gen_patterns <description file>\n");
    exit(EXIT_FAILURE);
  }

  /* Store input filename */
  inpFilename = argv[1];

  /* Open input file */
  yyin = fopen(inpFilename.c_str(), "r");
  if (!yyin) {
    fprintf(stderr, "Could not open file \"%s\".\n", inpFilename.c_str());
    exit(EXIT_FAILURE);
  }

  /* Parse input file */
  yyparse();

  /* Close input file */
  fclose(yyin);

  /* Write output */
  write_header();
  write_impl();
  write_html();

  /* Clean up */
  vector<Pattern*>::iterator it = pattern.begin();
  while (it != pattern.end()) {
    delete *it;
    ++it;
  }

  return EXIT_SUCCESS;
}


void write_header()
{
  /* Open header file */
  string filename = prefix + ".h";
  FILE* fp = fopen(filename.c_str(), "w");
  if (!fp) {
    fprintf(stderr, "Could not open output file \"%s\".\n", filename.c_str());
    exit(EXIT_FAILURE);
  }

  /* Temporary strings */
  string prefix_upper = uppercase(prefix);

  /* Write copyright notice */
  fprintf(fp, "%s", copyright);

  /* Open include guard */
  fprintf(fp, "#ifndef SCOUT_%s_H\n", prefix_upper.c_str());
  fprintf(fp, "#define SCOUT_%s_H\n", prefix_upper.c_str());
  fprintf(fp, "\n\n");

  /* Open namespace */
  fprintf(fp, "namespace scout\n"
              "{\n"
              "\n");

  /* Write pattern constants */
  int num = -1;
  fprintf(fp, "//--- Constants -------------------------------------------------------------\n\n");
  fprintf(fp, "const long PAT_%-30s = %3d;\n", "NONE", num++);
  vector<Pattern*>::const_iterator it = pattern.begin();
  while (it != pattern.end()) {
    fprintf(fp, "const long PAT_%-30s = %3d;\n", (*it)->get_id().c_str(), num++);
    ++it;
  }
  fprintf(fp, "\n\n");

  /* Write forward declarations */
  fprintf(fp, "//--- Forward declarations --------------------------------------------------\n"
              "\n"
              "class AnalyzeTask;\n"
              "\n\n");

  /* Write function prototypes */
  fprintf(fp, "//--- Function prototypes ---------------------------------------------------\n"
              "\n"
              "void create_patterns(AnalyzeTask* analyzer);\n"
              "\n\n");

  /* Close namespace */
  fprintf(fp, "}   /* namespace scout */\n"
              "\n\n");

  /* Close include guard */
  fprintf(fp, "#endif   /* !SCOUT_%s_H */\n", prefix_upper.c_str());

  /* Close file */
  fclose(fp);
}


void write_impl()
{
  /* Open implementation file */
  string filename = prefix + ".cpp";
  FILE* fp = fopen(filename.c_str(), "w");
  if (!fp) {
    fprintf(stderr, "Could not open output file \"%s\".\n", filename.c_str());
    exit(EXIT_FAILURE);
  }

  /* Write copyright notice */
  fprintf(fp, "%s", copyright);

  /* Write common includes */
  fprintf(fp, "#define __STDC_FORMAT_MACROS\n"
              "#define __STDC_CONSTANT_MACROS\n\n"
              "#include <config.h>\n"
              "#include \"Patterns_gen.h\"\n\n"
              "#include <cassert>\n"
              "#include <inttypes.h>\n\n"
              "#include <pearl/CallbackManager.h>\n"
              "#include <pearl/Callpath.h>\n"
              "#include <pearl/GlobalDefs.h>\n\n");
  fprintf(fp, "#include \"AnalyzeTask.h\"\n"
              "#include \"CbData.h\"\n"
              "#include \"MpiPattern.h\"\n"
              "#ifdef _OPENMP\n"
              "#  include \"OmpPattern.h\"\n"
              "#endif\n\n");

  /* Write "using" directives */
  fprintf(fp, "using namespace std;\n"
              "using namespace pearl;\n"
              "using namespace scout;\n"
              "\n\n");

  /* Write prolog */
  fprintf(fp, "%s\n\n", prolog.c_str());

  /* Write class implementation */
  vector<Pattern*>::const_iterator it = pattern.begin();
  while (it != pattern.end()) {
    (*it)->write_impl(fp);
    ++it;
  }

  /* Write function implementation */
  fprintf(fp, "//--- Implementation --------------------------------------------------------\n"
              "\n"
              "void scout::create_patterns(AnalyzeTask* analyzer)\n"
              "{\n");
  it = pattern.begin();
  while (it != pattern.end()) {
    if ((*it)->skip_impl()) {
      ++it;
      continue;
    }

    const struct guard {
      const char *type, *begin, *end;
    } guards[] = {
      { "MPI",    
        "#if defined(_MPI)\n", 
        "#endif   /* _MPI */\n" 
      },
      { "MPIDEP", 
        "#if defined(_MPI)\n", 
        "#endif   /* _MPI */\n" 
      },
      { "MPI_RMA",
        "#if defined(_MPI) && defined(HAS_MPI2_1SIDED)\n", 
        "#endif   /* _MPI && HAS_MPI2_1SIDED */\n"
      },
      { "MPI_RMADEP",
        "#if defined(_MPI) && defined(HAS_MPI2_1SIDED)\n", 
        "#endif   /* _MPI && HAS_MPI2_1SIDED */\n"
      },
      { "OMP",
        "#if defined(_OPENMP)\n",
        "#endif   /* _OPENMP */\n"        
      },
      { "OMPDEP",
        "#if defined(_OPENMP)\n",
        "#endif   /* _OPENMP */\n"        
      },
      { "ARMCI",
        "#if defined(HAS_ARMCI)\n",
        "#endif   /* HAS_ARMCI */\n"
      },
      { "ARMCIDEP",
        "#if defined(HAS_ARMCI)\n",
        "#endif   /* HAS_ARMCI */\n"
      },

      { 0, 0, 0 }
    };

    const struct guard* g;

    for (g = guards; g->type && (*it)->get_type() != g->type; ++g)
      ;

    if (g->begin)
      fprintf(fp, "%s", g->begin);

    if (!(*it)->get_condition().empty())
      fprintf(fp, "  if (%s)\n  ", (*it)->get_condition().c_str());

    fprintf(fp, "  analyzer->addPattern(new %s());\n",
                (*it)->get_classname().c_str());

    if (g->end)
      fprintf(fp, "%s", g->end);

    ++it;
  }
  fprintf(fp, "}\n\n\n");

  /* Close file */
  fclose(fp);
}


void write_html()
{
  /* Open documentation file */
  string filename = prefix + ".html";
  FILE* fp = fopen(filename.c_str(), "w");
  if (!fp) {
    fprintf(stderr, "Could not open output file \"%s\".\n", filename.c_str());
    exit(EXIT_FAILURE);
  }

  /* Write file header */
  fprintf(fp, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
              "<html>\n"
              "<head>\n"
              "<title>Performance properties</title>\n"
              "</head>\n"
              "<body>\n");

  /* Write page header */
  fprintf(fp, "<h2>Performance properties</h2>\n\n");

  /* Write class documentation */
  bool isFirst = true;
  vector<Pattern*>::const_iterator it = pattern.begin();
  while (it != pattern.end()) {
    (*it)->write_html(fp, isFirst);
    isFirst = false;
    ++it;
  }

  /* Close file */
  fclose(fp);
}


void yyerror(const string& message)
{
  if (!incFilename.empty())
    fprintf(stderr, "In included file \"%s\":\n   ", incFilename.c_str());

  fprintf(stderr, "Line %ld: %s\n", lineno, message.c_str());
  exit(EXIT_FAILURE);
}


struct fo_toupper : public std::unary_function<int,int> {
  int operator()(int x) const {
    return std::toupper(x);
  }
};


string uppercase(const string& str)
{
  string result(str);

  transform(str.begin(), str.end(), result.begin(), fo_toupper());

  return result;
}

