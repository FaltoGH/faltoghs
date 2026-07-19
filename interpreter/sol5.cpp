#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>

#define PRINT_LEXER_RESULT 0

class Token
{
public:
    enum class TOKEN_TYPE
    {
        NUMBER,
        OP,
        SEMI
    };

    TOKEN_TYPE type;
    int value;

    std::string toString()
    {
        if (type == TOKEN_TYPE::NUMBER)
        {
            return std::to_string(value);
        }

        if (type == TOKEN_TYPE::OP)
        {
            char cs[2];
            cs[0] = value;
            cs[1] = '\0';
            return std::string(cs);
        }

        if (type == TOKEN_TYPE::SEMI)
        {
            return ";";
        }

        return "?";
    }
};

class Lexer
{
    char nextChar()
    {
        return getc(stdin);
    }

    Token nextToken(char *currentChar)
    {
        Token ret;

        while (*currentChar == ' ')
        {
            *currentChar = nextChar();
        }

        if ('0' <= (*currentChar) && (*currentChar) <= '9')
        {
            ret.type = Token::TOKEN_TYPE::NUMBER;
            ret.value = ((*currentChar) - '0');
            (*currentChar) = nextChar();
            while ('0' <= (*currentChar) && (*currentChar) <= '9')
            {
                ret.value = (ret.value * 10) + ((*currentChar) - '0');
                (*currentChar) = nextChar();
            }
            return ret;
        }

        if ((*currentChar) == '+' || (*currentChar) == '-')
        {

            ret.type = Token::TOKEN_TYPE::OP;
            ret.value = (*currentChar);
            (*currentChar) = nextChar();
            return ret;
        }

        if ((*currentChar) == ';')
        {
            ret.type = Token::TOKEN_TYPE::SEMI;
            return ret;
        }

        throw 1;
    }

public:
    std::vector<Token> lex()
    {
        char currentChar = ' ';
        std::vector<Token> tokens;
        tokens.push_back(nextToken(&currentChar));

        while (tokens.back().type != Token::TOKEN_TYPE::SEMI)
        {
            tokens.push_back(nextToken(&currentChar));
        }

        if (PRINT_LEXER_RESULT)
        {
            printf("Lexer result: ");
            for (int i = 0; i < tokens.size(); i++)
            {
                std::cout << tokens[i].toString() << ' ';
            }
            puts("");
        }

        return tokens;
    }
};

class Interpreter
{
public:
    int interprete(const std::vector<Token> &tokens)
    {
        if (tokens[0].type != Token::TOKEN_TYPE::NUMBER)
        {
            throw 2;
        }

        int result = tokens[0].value;

        for (int i = 1; i < tokens.size(); i++)
        {
            if (tokens[i].type == Token::TOKEN_TYPE::SEMI)
            {
                return result;
            }

            if (tokens[i].type == Token::TOKEN_TYPE::OP)
            {
                if (tokens[i].value == '+')
                {
                    result += tokens[i + 1].value;
                }
                else
                {
                    result -= tokens[i + 1].value;
                }
                i++;
                continue;
            }
        }

        return result;
    }
};

int main(int argc, char **argv)
{
    try
    {
        Lexer lexer;
        std::vector<Token> tokens = lexer.lex();
        Interpreter interpreter;
        int result = interpreter.interprete(tokens);
        printf("%d", result);
    }
    catch (const int x)
    {
        puts("ERROR");
    }
    return 0;
}
