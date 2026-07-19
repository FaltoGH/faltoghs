#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

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
        SEMI,
        DOT,
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
        case TOKEN_TYPE::SEMI:
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
    int pos = 0;

    void advance(){
        pos++;
    }

    char getCurrentChar(){
        return inp[pos];
    }

    char peek(){
        return inp[pos+1];
    }

public:
    std::string inp;

    Token nextToken()
    {
        Token ret;

        while (getCurrentChar() == ' ')
        {
            advance();
        }

        if ('0' <= getCurrentChar() && getCurrentChar() <= '9')
        {
            ret.type = Token::TOKEN_TYPE::INTEGER;
            ret.value = (getCurrentChar() - '0');
            advance();
            while ('0' <= getCurrentChar() && getCurrentChar() <= '9')
            {
                ret.value = (ret.value * 10) + (getCurrentChar() - '0');
                advance();
            }
            return ret;
        }

        if (getCurrentChar() == '+')
        {
            ret.type = Token::TOKEN_TYPE::PLUS;
            advance();
            return ret;
        }

        if (getCurrentChar() == '-')
        {
            ret.type = Token::TOKEN_TYPE::MINUS;
            advance();
            return ret;
        }

        if (getCurrentChar() == '*')
        {
            ret.type = Token::TOKEN_TYPE::MUL;
            advance();
            return ret;
        }

        if (getCurrentChar() == '/')
        {
            ret.type = Token::TOKEN_TYPE::DIV;
            advance();
            return ret;
        }

        if (getCurrentChar() == '(')
        {
            ret.type = Token::TOKEN_TYPE::LPAREN;
            advance();
            return ret;
        }

        if (getCurrentChar() == ')')
        {
            ret.type = Token::TOKEN_TYPE::RPAREN;
            advance();
            return ret;
        }

        if (getCurrentChar() == ';')
        {
            ret.type = Token::TOKEN_TYPE::SEMI;
            advance();
            return ret;
        }

        if (getCurrentChar() == '.')
        {
            ret.type = Token::TOKEN_TYPE::DOT;
            advance();
            return ret;
        }

        ret.type = Token::TOKEN_TYPE::ERROR;
        advance();
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
            currentToken = lexer->nextToken();
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

    Lexer* lexer;

public:
    void setLexer(Lexer* lexer)
    {
        this->lexer = lexer;
        currentToken = (this->lexer)->nextToken();
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

    void interprete(){
        printf("%d ", expr());
        eat(Token::TOKEN_TYPE::SEMI);
        while(currentToken.type != Token::TOKEN_TYPE::DOT){
            printf("%d ", expr());
            eat(Token::TOKEN_TYPE::SEMI);
        }
    }
};




int main(int argc, char **argv)
{
    std::string s;
    std::getline(std::cin, s);
    Lexer lexer;
    lexer.inp = s;
    Interpreter interpreter;
    interpreter.setLexer(&lexer);
    interpreter.interprete();
    return 0;
}



