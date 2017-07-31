#ifndef __TREE__H__
#define __TREE__H__

#include <stdlib.h>

typedef enum {
    nd_Ident, nd_String, nd_Integer, nd_Sequence, nd_If, nd_Prtc, nd_Prts, nd_Prti, nd_While,    
    nd_Assign, nd_Negate, nd_Not, nd_Mul, nd_Div, nd_Mod, nd_Add, nd_Sub, nd_Lss, nd_Leq,
    nd_Gtr, nd_Geq, nd_Eql, nd_Neq, nd_And, nd_Or
} NodeType;

struct {
    const char       *enum_text;
    NodeType   nodeType;    
} atr[] = 
{
    {"Identifier"  , nd_Ident  },  {"String"      , nd_String  },   
    {"Integer"     , nd_Integer},  {"Sequence"    , nd_Sequence},
    {"If"          , nd_If     },  {"Prtc"        , nd_Prtc    },
    {"Prts"        , nd_Prts   },  {"Prti"        , nd_Prti    },
    {"While"       , nd_While  },  {"Assign"      , nd_Assign  },
    {"Negate"      , nd_Negate },  {"Not"         , nd_Not     },
    {"Multiply"    , nd_Mul    },  {"Divide"      , nd_Div     },
    {"Mod"         , nd_Mod    },  {"Add"         , nd_Add     },
    {"Subtract"    , nd_Sub    },  {"Less"        , nd_Lss     },
    {"LessEqual"   , nd_Leq    },  {"Greater"     , nd_Gtr     },
    {"GreaterEqual", nd_Geq    },  {"Equal"       , nd_Eql     },
    {"NotEqual"    , nd_Neq    },  {"And"         , nd_And     },
    {"Or"          , nd_Or     }
};

typedef struct Tree

{
	Tree *left;
    Tree *right;
    NodeType nodeType;
    int value;
} Tree;

Tree* makeNode(NodeType nodeType, Tree *left, Tree *right);
Tree* makeLeaf(NodeType nodeType, int value);

#endif
