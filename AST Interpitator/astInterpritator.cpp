#include "astInterpritator.h"

astInterp::astInterp(const char *fileName)
{   
    if (fileName[0] == '\0')
        m_source = stdin;
    else if ((m_source = fopen(fileName, "r")) == NULL)
        error("Can't open file %s", fileName);

    m_head = loadAST();
    run(m_head);
}

astInterp::~astInterp()
{
    if (m_source != stdin)
        fclose(m_source);

    deleteAST(m_head);
}

void astInterp::error(const char *fmt, ... ) {
    va_list ap;
    char buf[1000];
 
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    printf("error: %s\n", buf);
    exit(1);
}

void astInterp::printTree(Tree* head)
{
	static int space = 0;
	if (head == NULL)
	{
		return ;
	}
	++space;
	
	printf("%s\n", atr[head->nodeType].enum_text);

	printTree(head->left);
	printTree(head->right);

	--space;
}

void astInterp::deleteAST(Tree *head)
{
    if (head == NULL)
        return ;

    deleteAST(head->left);
    deleteAST(head->right);
    
    head->left = head->right = NULL;
    free(head);
}

NodeType astInterp::getEnum(string str)
{
    for (int i = 0; i < 25; i++)
            if (strcmp(str.c_str(), atr[i].enum_text) == 0)
                return atr[i].nodeType;

    error("Unknow Token %s\n", str.c_str());
}

int astInterp::defString(string str)
{
    /* erase first & last " */
    str.erase(str.begin());
    str.erase(str.end() - 1);

    for (size_t i = 0; i < str.size(); i++)
		if (str[i] == '\\')
		{
			if (str[i + 1] == '\\')
				str[i] = '\\';
			else if (str[i + 1] == 'n')
				str[i] = '\n';

			str.erase(i + 1);
		}
			
    for (size_t i = 0; i < m_stringPool.size(); i++)
        if (m_stringPool[i] == str)
            return i;

    m_stringPool.push_back(str);
    
    return m_stringPool.size() - 1;
}

int astInterp::defGlobalVar(string str)
{
    for (size_t i = 0; i < m_globalNames.size(); i++)
        if (m_globalNames[i] == str)
            return i;

    m_globalNames.push_back(str);
    m_globalValues.push_back(0);

    return m_globalValues.size() - 1;
}

Tree* astInterp::loadAST()
{
	char tmp[MAXSTRLEN];
    string firstWord, lastWord;

    if (!fgets(tmp, MAXSTRLEN, m_source))
        error("Can't read string from source file \n");

	string str(tmp);
	if (str.back() == '\n')
		str.erase(str.end() - 1);

	int pos = str.find(" ");
	if (pos != string::npos)
	{
		firstWord = str.substr(0, pos);
		while (pos < str.size())
		{
			if (str[pos] != ' ') break;
			pos++;
		}

		lastWord = str.substr(pos);
	}
	else
		firstWord = str;

    if (firstWord[0] == ';')
        return NULL;

    NodeType nodeType = getEnum(firstWord);
	
    int val;
    switch (nodeType)    
    {
	    case nd_Integer:    val = strtol(lastWord.c_str(), NULL, 0);
	                        return makeLeaf(nodeType, val);

        case nd_String:     val = defString(lastWord);
                            return makeLeaf(nodeType, val);

        case nd_Ident: 		val = defGlobalVar(lastWord);
                            return makeLeaf(nodeType, val);

        default:
							Tree *left = loadAST();
							Tree *right = loadAST();
                            return makeNode(nodeType, left, right);
    }
}

int astInterp::run(Tree *x)
{
    if (x == NULL)
        return 0;

    switch (x -> nodeType)
    {
        case nd_Integer: return x->value;
        case nd_Ident:   return m_globalValues[x->value];
        case nd_String:  return x->value;

        case nd_Assign:  return m_globalValues[x->left->value] = run(x->right);
        case nd_Add:     return run(x->left) + run(x->right);
        case nd_Sub:     return run(x->left) - run(x->right);
        case nd_Mul:     return run(x->left) * run(x->right);
        case nd_Div:     return run(x->left) / run(x->right);
        case nd_Mod:     return run(x->left) % run(x->right);
        case nd_Lss:     return run(x->left) < run(x->right);
        case nd_Gtr:     return run(x->left) > run(x->right);
        case nd_Leq:     return run(x->left) <= run(x->right);
        case nd_Eql:     return run(x->left) == run(x->right);
        case nd_Neq:     return run(x->left) != run(x->right);
        case nd_And:     return run(x->left) && run(x->right);
		case nd_Negate:  return -run(x->left);
        case nd_Not:     return !run(x->left);

        case nd_If:      if (run(x->left))
                            run(x->right->left);
                         else
                            run(x->right->right);
						 break;

        case nd_While:   while (run(x->left))
                            run(x->right);
						 break;

        case nd_Prtc:    printf("%c", run(x->left));
						 break;

        case nd_Prti:    printf("%d", run(x->left));
						 break;

        case nd_Prts:    printf("%s", m_stringPool[run(x->left)].c_str());
						 break;

        case nd_Sequence: 
                         run(x->left);
                         run(x->right);
						 break;
        default:
                          error("interp: unknown tree type %d\n", x->nodeType);
    }

    return 0;
}

