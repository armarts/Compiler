#ifndef __SYNTAXANALISATOR__H__
#define __SYNTAXANALISATOR__H__

#include <stdio.h>
#include <stdarg.h>

#include "tree.h"

#define MAXSTRLEN 1024

class SyntaxAnalisator
{
public:
	SyntaxAnalisator();
	~SyntaxAnalisator();

	void init_in(const char *fileName);
	void init_out(const char *fileName);

	void run();
private:

	void error(int errLn, int errCol, const char *fmt, ...);
	void expect(const char *msg, TokenType s);

	/*	 Grammar   */

	Tree* stmt_list();
	Tree* stmt();
	Tree* paren_expr();
	Tree* printList();
	Tree* expr();
	Tree* and_expr();
	Tree* equality_expr();
	Tree* relation_expr();
	Tree* additional_expr();
	Tree* multiplication_expr();
	Tree* primary();

	token getTok();

	char* readLine();

	TokenType getEnum(char *name);

	void printTree(Tree *head);

	//Tree* parse();

	FILE *m_in;
	FILE *m_out;

	token m_tkn;
};

#endif
