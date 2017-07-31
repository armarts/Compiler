#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define strarg(str) #str

#define TRUE 1
#define FALSE 0

#define MAXIDENT 100
#define MAXSTRING 1000

typedef enum {
    tk_EOI, tk_Mul, tk_Div, tk_Mod, tk_Add, tk_Sub, tk_Negate, tk_Not, tk_Lss, tk_Leq,
    tk_Gtr, tk_Geq, tk_Eq, tk_Neq, tk_Assign, tk_And, tk_Or, tk_If, tk_Else, tk_While,
    tk_Print, tk_Putc, tk_Lparen, tk_Rparen, tk_Lbrace, tk_Rbrace, tk_Semi, tk_Comma,
    tk_Ident, tk_Integer, tk_String
} TokenType;

typedef struct {
    TokenType tok;
    int indLine, indCol;
    int n;      /* value for integer */
    char *text; /* value for ident name */
} token;

static FILE *in, *out;
static int line = 1, col = 0;
static char ch = ' ';

static char* keyWords[] =
{
    "if",
    "else",
    "while",
    "print",
    "putc"
};

token getTok();

static void error(int errLine, int errCol, const char *fmt, ... ) {
    char buf[1000];
    va_list ap;
         
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    printf("(%d,%d) error: %s\n", errLine, errCol, buf);
    
    exit(1);
}

static void next_ch()
{
    ch = getc(in);
    ++col;
    
    if (ch == '\n')
    {
        ++line;
        col = 0;
    }
}

static token charLit(int n, int indLine, int indCol)
{
    if (ch == '\'')
        error(indLine, indCol, "empty literal");

    if (ch == '\\')
    {
        next_ch();
        if (ch == 'n')
            n = '\n';
        else
        if (ch == '\\')
            n = '\\';
        else
            error(indLine, indCol, "unknow esacape char");
    } 
    next_ch();

    if (ch != '\'')
        error(indLine, indCol, "it is not character literal");
    
    next_ch();

    return (token){tk_Integer, indLine, indCol, n, NULL};
}

static token divOrCmt(int indLine, int indCol)
{
    if (ch == '/')
    {
        next_ch();
        while (ch != EOF && ch != '\n')
            next_ch();

        if (ch == EOF)
            return (token){tk_EOI, indLine, indCol, 0, NULL};

        if (ch == '\n')
        {
            next_ch();
            return getTok();
        }
    }

    if (ch != '*')
        return (token){tk_Div, indLine, indCol, 0, NULL};

    /* comment detected */
    while (TRUE)
    {
        switch (ch)
        {
            case '*':
                next_ch();
                if (ch == '/')
                {
                    next_ch();
                    return getTok();
                }
                break;
            case EOF:
                error(indLine, indCol, "EOF in comment");
        }
        next_ch();
    }
}

static token stringLit(char start, int indLine, int indCol)
{
    static char buf[MAXSTRING];
    int position = 0;

    next_ch();
    while (ch != start)
    {
        if (ch == '\n') error(indLine, indCol, "EOL in string");
        if (ch == EOF)  error(indLine, indCol, "EOF in string");
        buf[position++] = ch;
        next_ch();
    }
    buf[position++] = '\0';
    
    next_ch();
    return (token){tk_String, indLine, indCol, 0, buf};
}

static TokenType getIdentType(char *ident, int indLine, int indCol)
{
    int i;
    for (i = 0; i < 5; i++)
        if (strcmp(ident, keyWords[i]) == 0)
            return tk_If + i;

    return tk_Ident;
}

static token identOrInt(int indLine, int indCol)
{
    int n;
    int position = 0;
    int isNumber = TRUE;

    static char id[MAXIDENT];

    while (isalnum(ch) || ch == '_')
    {
        id[position++] = ch;
        if (!isdigit(ch))
            isNumber = FALSE;
        next_ch();
    }

    if (position == 0)
        error(indLine, indCol, "unrecognized character (%d) '%c'\n", ch, ch);
    id[position++] = '\0';

    if (isdigit(id[0]))
    {
        if (!isNumber)
            error(indLine, indCol, "invalid number: %s\n", id);
        n = strtol(id, NULL, 0);
        if (n == LONG_MAX && errno == ERANGE)
            error(indLine, indCol, "Number out of limits");
        return (token){tk_Integer, indLine, indCol, n, NULL};
    }
    return (token){getIdentType(id, indLine, indCol), indLine, indCol, 0, id};

}

static token follow(int expect, TokenType ifyes, TokenType ifno, int indLine, int indCol)
{
    if (ch == expect)
    {
        next_ch();
        return (token){ifyes, indLine, indCol, 0, NULL};
    }
    if (ifno == tk_EOI)
        error(indLine, indCol, "follow: unrecognized character '%c' (%d)\n", ch, ch);
    
    return (token){ifno, indLine, indCol, 0, NULL};
}


token getTok()
{
    /* skip white space */
    while (isspace(ch))
        next_ch();

    int indLine = line, indCol = col;
    switch (ch)
    {
        case '{':  next_ch(); return (token){tk_Lbrace, indLine, indCol,  0, NULL};
        case '}':  next_ch(); return (token){tk_Rbrace, indLine, indCol, 0, NULL};
        case '(':  next_ch(); return (token){tk_Lparen, indLine, indCol,  0, NULL};
        case ')':  next_ch(); return (token){tk_Rparen,  indLine, indCol, 0, NULL};
        case '+':  next_ch(); return (token){tk_Add,  indLine, indCol, 0, NULL};
        case '-':  next_ch(); return (token){tk_Sub,  indLine, indCol, 0, NULL};
        case '*':  next_ch(); return (token){tk_Mul,  indLine, indCol, 0, NULL};
        case '%':  next_ch(); return (token){tk_Mod,  indLine, indCol, 0, NULL};
        case ';':  next_ch(); return (token){tk_Semi,  indLine, indCol, 0, NULL};
        case ',':  next_ch(); return (token){tk_Comma,  indLine, indCol, 0, NULL};
        case '/':  next_ch(); return divOrCmt(indLine, indCol);
        case '\'': next_ch(); return charLit(ch, indLine, indCol);
        case '<':  next_ch(); return follow('=', tk_Leq, tk_Lss,    indLine, indCol);
        case '>':  next_ch(); return follow('=', tk_Geq, tk_Gtr,    indLine, indCol);
        case '=':  next_ch(); return follow('=', tk_Eq,  tk_Assign, indLine, indCol);
        case '!':  next_ch(); return follow('=', tk_Neq, tk_Not,    indLine, indCol);
        case '&':  next_ch(); return follow('&', tk_And, tk_EOI,    indLine, indCol);
        case '|':  next_ch(); return follow('|', tk_Or,  tk_EOI,    indLine, indCol);
        case '"' : return stringLit(ch, indLine, indCol);
        case EOF:  return (token){tk_EOI,  indLine, indCol, 0, NULL};
        default:   return identOrInt(indLine, indCol);
    }
}


void runLexer()
{
    token tkn;
    do
    {
        tkn = getTok();
		fprintf(out, "%5d  %5d %.15s",
            tkn.indLine, tkn.indCol,
            &"End_of_input    Op_multiply     Op_divide       Op_mod          Op_add          "
             "Op_subtract     Op_negate       Op_not          Op_less         Op_lessequal    "
             "Op_greater      Op_greaterequal Op_equal        Op_notequal     Op_assign       "
             "Op_and          Op_or           Keyword_if      Keyword_else    Keyword_while   "
             "Keyword_print   Keyword_putc    LeftParen       RightParen      LeftBrace       "
             "RightBrace      Semicolon       Comma           Identifier      Integer         "
             "String          "
            [tkn.tok * 16]);
        if (tkn.tok == tk_Integer) fprintf(out, " %6d", tkn.n);
        if (tkn.tok == tk_String)  fprintf(out, " \"%s\"", tkn.text);
        if (tkn.tok == tk_Ident)   fprintf(out, " %s", tkn.text);
        fprintf(out, "\n");
    } while (tkn.tok != tk_EOI); 

    if (out != stdout)
        fclose(out);
}

void init_io(FILE **filePtr, FILE *std, const char *mode, const char *fileName)
{
    if (fileName[0] == '\0')
        *filePtr = std;
    else 
    if ((*filePtr = fopen(fileName, mode)) == NULL)
        error(0, 0, "Can't open %s\n", fileName);
}

int main(int argc, char **argv)
{
    init_io(&in, stdin, "r", argc > 1 ? argv[1] : "");
    init_io(&out, stdout, "w", argc > 2 ? argv[2] : "");

    runLexer();
    
    return 0;
}
