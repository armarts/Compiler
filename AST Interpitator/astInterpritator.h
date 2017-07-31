#ifndef __ASTINTERPRITATOR__H__
#define __ASTINTERPRITATOR__H__

#include <vector>
#include <string>
#include <string.h>
#include <stdarg.h>

#include "tree.h"

#define MAXSTRLEN 100

using std::string;
using std::vector;

class astInterp
{
public:
    astInterp(const char *fileName);
    ~astInterp();
    
	void error(const char *fmt, ...);
    NodeType getEnum(string str);

	void printTree(Tree *head);

private:

    void deleteAST(Tree *head);

    Tree* loadAST();
    int run(Tree *x);
	int defGlobalVar(string str);
	int defString(string str);

    FILE *m_source;

    Tree  *m_head;

    vector<string> m_stringPool;
    vector<string> m_globalNames;
    vector<int>    m_globalValues;

};

#endif
