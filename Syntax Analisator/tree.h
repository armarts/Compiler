#ifndef __TREE__H__
#define __TREE__H__

#include <stdlib.h>
#include <string.h>

typedef enum {
    tk_EOI, tk_Mul, tk_Div, tk_Mod, tk_Add, tk_Sub, tk_Negate, tk_Not, tk_Lss, tk_Leq, tk_Gtr,
    tk_Geq, tk_Eql, tk_Neq, tk_Assign, tk_And, tk_Or, tk_If, tk_Else, tk_While, tk_Print,
    tk_Putc, tk_Lparen, tk_Rparen, tk_Lbrace, tk_Rbrace, tk_Semi, tk_Comma, tk_Ident,
    tk_Integer, tk_String
} TokenType;

typedef enum {
    nd_Ident, nd_String, nd_Integer, nd_Sequence, nd_If, nd_Prtc, nd_Prts, nd_Prti, nd_While,
    nd_Assign, nd_Negate, nd_Not, nd_Mul, nd_Div, nd_Mod, nd_Add, nd_Sub, nd_Lss, nd_Leq,
    nd_Gtr, nd_Geq, nd_Eql, nd_Neq, nd_And, nd_Or, NOTHING
} NodeType;

typedef struct {
    TokenType tok;
    int line;
    int col;
    char *text;             /* ident or string literal or integer value */
} token;

struct {
    const char       *text, *enum_text;
    TokenType   tok;
    NodeType    nodeType;
} atr[] =
{
    {"EOI",             "End_of_input"   , tk_EOI,     NOTHING    },
    {"*",               "Op_multiply"    , tk_Mul,     nd_Mul     },
    {"/",               "Op_divide"      , tk_Div,     nd_Div     },
    {"%",               "Op_mod"         , tk_Mod,     nd_Mod     },
    {"+",               "Op_add"         , tk_Add,     nd_Add     },
    {"-",               "Op_subtract"    , tk_Sub,     nd_Sub     },
    {"-",               "Op_negate"      , tk_Negate,  nd_Negate  },
    {"!",               "Op_not"         , tk_Not,     nd_Not     },
    {"<",               "Op_less"        , tk_Lss,     nd_Lss     },
    {"<=",              "Op_lessequal"   , tk_Leq,     nd_Leq     },
    {">",               "Op_greater"     , tk_Gtr,     nd_Gtr     },
    {">=",              "Op_greaterequal", tk_Geq,     nd_Geq     },
    {"==",              "Op_equal"       , tk_Eql,     nd_Eql     },
    {"!=",              "Op_notequal"    , tk_Neq,     nd_Neq     },
    {"=",               "Op_assign"      , tk_Assign,  nd_Assign  },
    {"&&",              "Op_and"         , tk_And,     nd_And     },
    {"||",              "Op_or"          , tk_Or,      nd_Or      },
    {"if",              "Keyword_if"     , tk_If,      nd_If      },
    {"else",            "Keyword_else"   , tk_Else,    NOTHING    },
    {"while",           "Keyword_while"  , tk_While,   nd_While   },
    {"print",           "Keyword_print"  , tk_Print,   NOTHING    },
    {"putc",            "Keyword_putc"   , tk_Putc,    NOTHING    },
    {"(",               "LeftParen"      , tk_Lparen,  NOTHING    },
    {")",               "RightParen"     , tk_Rparen,  NOTHING    },
    {"{",               "LeftBrace"      , tk_Lbrace,  NOTHING    },
    {"}",               "RightBrace"     , tk_Rbrace,  NOTHING    },
    {";",               "Semicolon"      , tk_Semi,    NOTHING    },
    {",",               "Comma"          , tk_Comma,   NOTHING    },
    {"Ident",           "Identifier"     , tk_Ident,   nd_Ident   },
    {"Integer literal", "Integer"        , tk_Integer, nd_Integer },
    {"String literal",  "String"         , tk_String,  nd_String  },
};

static const char* Display_nodes[] = {"Identifier", "String", "Integer", "Sequence", "If", "Prtc",
    "Prts", "Prti", "While", "Assign", "Negate", "Not", "Multiply", "Divide", "Mod",
    "Add", "Subtract", "Less", "LessEqual", "Greater", "GreaterEqual", "Equal",
    "NotEqual", "And", "Or"};

typedef struct Tree
{
	Tree *left;
    Tree *right;
    NodeType nodeType;
    char *value; 		/* value for String or Integer */
} Tree;

Tree* makeNode(NodeType nodeType, Tree *left, Tree *right);
Tree* makeLeaf(NodeType nodeType, char *value);

#endif
