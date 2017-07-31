#include "syntaxAnalisator.h"

using namespace std;

int main(int argc, char **argv)
{
	SyntaxAnalisator sa;
	sa.init_in(argc > 1 ? argv[1] : "");
	sa.init_out(argc > 2 ? argv[2] : "");
	sa.run();
	return 0;
}
