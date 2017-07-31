#include "syntaxAnalisator.h"

SyntaxAnalisator::SyntaxAnalisator():
										m_in(NULL),
										m_out(NULL)
{
	m_tkn.text = NULL;
}

SyntaxAnalisator::~SyntaxAnalisator()
{
	if (m_in != stdin)	fclose(m_in);
	if (m_out != stdout)  fclose(m_out);
}

void SyntaxAnalisator::error(int errLn, int errCol, const char *fmt, ...)
{
		va_list ap;
    char buf[1000];

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    printf("%d %d error: %s\n", errLn, errCol, buf);
    exit(1);
}

void SyntaxAnalisator::expect(const char *msg, TokenType s)
{
	if (m_tkn.tok == s)
	{
		getTok();
		return ;
	}
	
	error(m_tkn.line, m_tkn.col, "%s: Expecting '%s', found '%s'\n", msg, atr[s].text, atr[m_tkn.tok].text);
}

Tree* SyntaxAnalisator::stmt_list()
{
	Tree *tree = NULL, *s;
	getTok();
	tree = stmt();
	while (m_tkn.tok != tk_EOI)
	{
		s = stmt();
		tree = makeNode(nd_Sequence, tree, s);
	}

	return tree;
}

Tree* SyntaxAnalisator::stmt()
{
	Tree *tree = NULL, *v, *e, *s, *s2;
	switch (m_tkn.tok)
	{
		case tk_If:
			getTok();
			e = paren_expr();
			s = stmt();
			s2 = NULL;
			if (m_tkn.tok == tk_Else)
			{
				getTok();
				s2 = stmt();
			}
			tree = makeNode(nd_If, e, makeNode(nd_If, s, s2));
			break;
		case tk_Putc:
			getTok();
			e = paren_expr();
			expect("Putc", tk_Semi);
			tree = makeNode(nd_Prtc, e, NULL);
			break;
		case tk_Print:
			getTok();
			expect("Print", tk_Lparen);
			tree = printList();
			expect("Print", tk_Rparen);
			expect("Print", tk_Semi);
			break;
		case tk_Semi:
			getTok();
			break;
		case tk_Ident:
			v = makeLeaf(nd_Ident, m_tkn.text);
			getTok();
			expect("assign", tk_Assign);
			e = expr();
			tree = makeNode(nd_Assign, v, e);
			expect("assign", tk_Semi);
			break;
		case tk_While:
			getTok();
			e = paren_expr();
			s = stmt();
			tree = makeNode(nd_While, e, s);
			break;
		case tk_Lbrace:
			expect("Lbrace", tk_Lbrace);
			while (m_tkn.tok != tk_Rbrace && m_tkn.tok != tk_EOI)
				tree = makeNode(nd_Sequence, tree, stmt());
			expect("Rbrace", tk_Rbrace);
			break;
	}
	return tree;
}

Tree* SyntaxAnalisator::paren_expr()
{
	expect("paren_expr", tk_Lparen);
	Tree *tree = expr();
	expect("paren_expr", tk_Rparen);

	return tree;
}

Tree* SyntaxAnalisator::printList()
{
	Tree *tree = NULL, *e;

	do
	{
		if (m_tkn.tok == tk_String)
		{
			e = makeNode(nd_Prts, makeLeaf(nd_String, m_tkn.text), NULL);
			getTok();
		} else e = makeNode(nd_Prti, expr(), NULL);

		tree = makeNode(nd_Sequence, tree, e);
		
		if (m_tkn.tok != tk_Comma)
			break;

		getTok();

	} while (true);

	return tree;
}

Tree* SyntaxAnalisator::expr()
{
	Tree *tree = NULL, *rhs;

	tree = and_expr();
	while (m_tkn.tok == tk_Or)
	{
		getTok();
		rhs = and_expr();
		tree = makeNode(nd_Or, tree, rhs);
	}

	return tree;
}

Tree* SyntaxAnalisator::and_expr()
{
	Tree *tree = NULL, *rhs;

	tree = equality_expr();
	while (m_tkn.tok == tk_And)
	{
		getTok();
		rhs = equality_expr();
		tree = makeNode(nd_And, tree, rhs);
	}

	return tree;
}

Tree* SyntaxAnalisator::equality_expr()
{
	Tree *tree = NULL, *rhs;

	tree = relation_expr();
	again1:
	switch (m_tkn.tok)
	{
		case tk_Eql:
			getTok();
			rhs = relation_expr();
			tree = makeNode(nd_Eql, tree, rhs);
			goto again1;

		case tk_Neq:
			getTok();
			rhs = relation_expr();
			tree = makeNode(nd_Neq, tree, rhs);
			goto again1;
	}

	return tree;
}

Tree* SyntaxAnalisator::relation_expr()
{
	Tree *tree = NULL, *rhs;

	tree = additional_expr();
	again2:
	switch (m_tkn.tok)
	{
		case tk_Lss:
			getTok();
			rhs = additional_expr();
			tree = makeNode(nd_Lss, tree, rhs);
			goto again2;

		case tk_Leq:
			getTok();
			rhs = additional_expr();
			tree = makeNode(nd_Leq, tree, rhs);
			goto again2;

		case tk_Gtr:
			getTok();
			rhs = additional_expr();
			tree = makeNode(nd_Gtr, tree, rhs);
			goto again2;

		case tk_Geq:
			getTok();
			rhs = additional_expr();
			tree = makeNode(nd_Geq, tree, rhs);
			goto again2;
	}

	return tree;
}

Tree* SyntaxAnalisator::additional_expr()
{
	Tree *tree = NULL, *rhs;

	tree = multiplication_expr();
	again3:
	switch (m_tkn.tok)
	{
		case tk_Add:
			getTok();
			rhs = multiplication_expr();
			tree = makeNode(nd_Add, tree, rhs);
			goto again3;

		case tk_Sub:
			getTok();
			rhs = multiplication_expr();
			tree = makeNode(nd_Sub, tree, rhs);
			goto again3;

		default:
			break;
	}

	return tree;
}

Tree* SyntaxAnalisator::multiplication_expr()
{
	Tree *tree = NULL, *rhs;
	tree = primary();

	again4:
	switch (m_tkn.tok)
	{
		case tk_Mul:
			getTok();
			rhs = primary();
			tree = makeNode(nd_Mul, tree, rhs);
			goto again4;

		case tk_Div:
			getTok();
			rhs = primary();
			tree = makeNode(nd_Div, tree, rhs);
			goto again4;

		case tk_Mod:
			getTok();
			rhs = primary();
			tree = makeNode(nd_Mod, tree, rhs);
			goto again4;

		default:
			break;
	}

	return tree;
}

Tree* SyntaxAnalisator::primary()
{
	Tree *tree = NULL, *e;

	switch (m_tkn.tok)
	{
		case tk_Ident:
			tree = makeLeaf(nd_Ident, m_tkn.text);
			getTok();
			break;

		case tk_Integer:
			tree = makeLeaf(nd_Integer, m_tkn.text);
			getTok();
			break;

		case tk_Lparen:
			tree = paren_expr();
			break;

		case tk_Add:
			getTok();
			e = primary();
			tree = makeNode(nd_Add, e, NULL);
			break;

		case tk_Negate:
			getTok();
			e = primary();
			tree = makeNode(nd_Negate, e, NULL);
			break;

		case tk_Not:
			getTok();
			e = primary();
			tree = makeNode(nd_Not, e, NULL);
			break;
	}

	return tree;
}

char* SyntaxAnalisator::readLine()
{
	char c;
	char *buf = (char*) malloc(sizeof(char) * MAXSTRLEN);
	int pos = 0;

	while (true)
	{
		c = fgetc(m_in);
		if (c == '\n' || c == EOF)
		{
			buf[pos++] = '\0';
			return buf;
		}

		buf[pos++] = c;
	}
}

TokenType SyntaxAnalisator::getEnum(char *name)
{
	for (size_t i = 0; i < 31; i++)
		if (strcmp(atr[i].enum_text, name) == 0)
			return atr[i].tok;

	error(0, 0, "Unknown token %s\n", name);
}


token SyntaxAnalisator::getTok()
{
	// free memory after last token
	//free(m_tkn.text);

	char *str = readLine();

	// get line & column of token
	m_tkn.line = atoi(strtok(str, " "));
	m_tkn.col  = atoi(strtok(NULL, " "));

	// get token name
	char *name = strtok(NULL, " ");
	m_tkn.tok = getEnum(name);

	// get value of token if  exist
	if (m_tkn.tok == tk_String || m_tkn.tok == tk_Integer || m_tkn.tok == tk_Ident)
	{
		char *val = name + strlen(name);
		val++;

		while (*val == ' ') val++;

		m_tkn.text = strdup(val);
	}

	free(str);
}

void SyntaxAnalisator::init_in(const char *fileName)
{
	if (fileName[0] == '\0')
		m_in = stdin;
	else	if ((m_in = fopen(fileName, "r")) == NULL)
		error(0, 0, "Can't open file");
}

void SyntaxAnalisator::init_out(const char *fileName)
{
	if (fileName[0] == '\0')
		m_out = stdout;
	else	if ((m_out = fopen(fileName, "w")) == NULL)
		error(0, 0, "Can't open file");
}

void SyntaxAnalisator::printTree(Tree *head)
{
	if (head == NULL)
	{
		fprintf(m_out, ";\n");
		return ;
	}

	fprintf(m_out, "%s", Display_nodes[head -> nodeType]);

	if (head -> nodeType == nd_Integer || head -> nodeType == nd_String || head -> nodeType == nd_Ident)
		fprintf(m_out, " %s\n", head -> value);
	else {
		fprintf(m_out, "\n");
		printTree(head -> left);
		printTree(head -> right);
	}
}

void SyntaxAnalisator::run()
{
	printTree( stmt_list() );
}
