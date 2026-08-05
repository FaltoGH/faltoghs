#include <assert.h>
#include <fstream> // std::ifstream
#include <iostream>
#include <map>
#include <stdbool.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sstream> // std::ostringstream
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "ttype.h"
#include "token.hpp"
#include "interpreter.hpp"

int main(int argc, char **argv)
{
    bool debug = false;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--debug") == 0)
        {
            debug=true;
            puts("debug=true");
        }
    }

    std::ifstream ifs("input.txt");
    std::ostringstream oss;
    oss << ifs.rdbuf();
    std::string entireFile = oss.str();
    lexer l;
    l._input=entireFile;
    token::debug=debug;
    std::vector<token> tokens=l.lex();

    if (debug)
    {
        for (auto t : tokens)
        {
            std::cout << t.toString() << ' ';
        }
        puts("");
    }

    parser p;
    p._tokens=tokens;
    p.debug = debug;
    token* s=p.stmt();
    
    if(debug){
      printf("-----END PARSE-----\r\nTop level statements are %d.\r\n",s->nc());
      for(auto t : s->children){
        t->prtdbg();
      }
    }

    interpreter inte;
    inte.debug = debug;
    inte.visit(s);

    if (debug)
    {
        for (const auto &[key, value] : inte.globalScope)
        {
            std::cout << key << ' ' << value << '\n';
        }
    }


    return 0;
}
