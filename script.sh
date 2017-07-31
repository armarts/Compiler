#!/bin/bash

./lexer/lex $1 |./Syntax\ Analisator/result | ./AST\ Interpitator/result
