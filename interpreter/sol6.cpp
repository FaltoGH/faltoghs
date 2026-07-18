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
        INTEGER,
        PLUS,
        MINUS,
        MUL,
        DIV,
        SEMICOLON,
        ERROR
    };

    TOKEN_TYPE type;
    int value;
};

class Lexer
{
    char currentChar = ' ';

    char nextChar()
    {
        return getc(stdin);
    }

public:
    Token nextToken()
    {
        Token ret;

        while (currentChar == ' ')
        {
            currentChar = nextChar();
        }

        if ('0' <= currentChar && currentChar <= '9')
        {
            ret.type = Token::TOKEN_TYPE::INTEGER;
            ret.value = (currentChar - '0');
            currentChar = nextChar();
            while ('0' <= currentChar && currentChar <= '9')
            {
                ret.value = (ret.value * 10) + (currentChar - '0');
                currentChar = nextChar();
            }
            return ret;
        }

        if (currentChar == '+')
        {
            ret.type = Token::TOKEN_TYPE::PLUS;
            currentChar = nextChar();
            return ret;
        }

        if (currentChar == '-')
        {
            ret.type = Token::TOKEN_TYPE::MINUS;
            currentChar = nextChar();
            return ret;
        }

        if (currentChar == '*')
        {
            ret.type = Token::TOKEN_TYPE::MUL;
            currentChar = nextChar();
            return ret;
        }

        if (currentChar == '/')
        {
            ret.type = Token::TOKEN_TYPE::DIV;
            currentChar = nextChar();
            return ret;
        }

        if (currentChar == ';')
        {
            ret.type = Token::TOKEN_TYPE::SEMICOLON;
            return ret;
        }

        ret.type = Token::TOKEN_TYPE::ERROR;
        return ret;
    }
};

class Interpreter
{
    Token currentToken;

    int term(){
        // term = INTEGER, { ( "*" | "/" ), INTEGER } ;
        int INTEGER = currentToken.value;
        currentToken = lexer.nextToken();

        while(currentToken.type == Token::TOKEN_TYPE::MUL || currentToken.type == Token::TOKEN_TYPE::DIV){
            if(currentToken.type == Token::TOKEN_TYPE::MUL){
                currentToken = lexer.nextToken();
                INTEGER *= currentToken.value;
                currentToken = lexer.nextToken();
            }
            else{
                currentToken = lexer.nextToken();
                INTEGER /= currentToken.value;
                currentToken = lexer.nextToken();
            }
        }

        return INTEGER;
    }

public:
    Lexer lexer;

    int expr()
    {
        // expr = term, { ( "+" | "-" ), term } ;
        currentToken = lexer.nextToken();
        int result = term();

        while(currentToken.type == Token::TOKEN_TYPE::PLUS || currentToken.type == Token::TOKEN_TYPE::MINUS)
        {
            if (currentToken.type == Token::TOKEN_TYPE::PLUS)
            {
                currentToken = lexer.nextToken();
                result += term();
            }
            else{
                currentToken = lexer.nextToken();
                result -= term();
            }
        }

        return result;
    }
};

int main(int argc, char **argv)
{
    Lexer lexer;
    Interpreter interpreter;
    interpreter.lexer = lexer;
    int result = interpreter.expr();
    printf("%d", result);
    return 0;
}
