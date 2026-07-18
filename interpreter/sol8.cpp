#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

#define DEBUG 0

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
        LPAREN,
        RPAREN,
        SEMICOLON,
        ERROR
    };

    TOKEN_TYPE type;
    int value;

    std::string toString()
    {
        switch (type)
        {
        case TOKEN_TYPE::INTEGER:
            return std::to_string(value);
        case TOKEN_TYPE::PLUS:
            return "+";
        case TOKEN_TYPE::MINUS:
            return "-";
        case TOKEN_TYPE::MUL:
            return "*";
        case TOKEN_TYPE::DIV:
            return "/";
        case TOKEN_TYPE::LPAREN:
            return "(";
        case TOKEN_TYPE::RPAREN:
            return ")";
        case TOKEN_TYPE::SEMICOLON:
            return ";";
        case TOKEN_TYPE::ERROR:
            return "!";
        default:
            return "?";
        }
    }
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

        if (currentChar == '(')
        {
            ret.type = Token::TOKEN_TYPE::LPAREN;
            currentChar = nextChar();
            return ret;
        }

        if (currentChar == ')')
        {
            ret.type = Token::TOKEN_TYPE::RPAREN;
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

    void eat(Token::TOKEN_TYPE tokenType)
    {
        if (currentToken.type == tokenType)
        {
            currentToken = lexer.nextToken();
        }
        else
        {
            assert(false);
        }
    }

    
    int factor()
    {
        // factor = ( ( "+" | "-" ), factor ) | INTEGER | ( "(", expr, ")" ) ;
        int result;
        
        if (currentToken.type == Token::TOKEN_TYPE::PLUS){
            eat(Token::TOKEN_TYPE::PLUS);
            return factor();
        }

        if (currentToken.type == Token::TOKEN_TYPE::MINUS){
            eat(Token::TOKEN_TYPE::MINUS);
            return -factor();
        }

        if (currentToken.type == Token::TOKEN_TYPE::INTEGER)
        {
            result = currentToken.value;
            eat(Token::TOKEN_TYPE::INTEGER);
            return result;
        }

        eat(Token::TOKEN_TYPE::LPAREN);
        result = expr();
        eat(Token::TOKEN_TYPE::RPAREN);
        return result;
    }

    int term()
    {
        // term = factor, { ( "*" | "/" ), factor } ;
        int result = factor();

        while (currentToken.type == Token::TOKEN_TYPE::MUL || currentToken.type == Token::TOKEN_TYPE::DIV)
        {
            if (currentToken.type == Token::TOKEN_TYPE::MUL)
            {
                eat(Token::TOKEN_TYPE::MUL);
                result *= factor();
            }
            else
            {
                eat(Token::TOKEN_TYPE::DIV);
                result /= factor();
            }
        }

        return result;
    }

    Lexer lexer;

public:
    void setLexer(Lexer lexer)
    {
        this->lexer = lexer;
        currentToken = (this->lexer).nextToken();
    }

    int expr()
    {
        // expr = term, { ( "+" | "-" ), term } ;
        int result = term();

        while (currentToken.type == Token::TOKEN_TYPE::PLUS || currentToken.type == Token::TOKEN_TYPE::MINUS)
        {
            if (currentToken.type == Token::TOKEN_TYPE::PLUS)
            {
                eat(Token::TOKEN_TYPE::PLUS);
                result += term();
            }
            else
            {
                eat(Token::TOKEN_TYPE::MINUS);
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
    interpreter.setLexer(lexer);
    int result = interpreter.expr();
    printf("%d", result);
    return 0;
}
