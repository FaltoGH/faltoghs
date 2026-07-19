#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <map>

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
        VARIABLE,
        ASSIGN,
        SEMI,
        DOT,
        ERROR
    };

    TOKEN_TYPE type;
    int value;
    std::string sValue;

    static const char* tokenTypeToString(TOKEN_TYPE type)
    {
        switch (type)
        {
        case TOKEN_TYPE::INTEGER:
            return "INTEGER";
        case TOKEN_TYPE::PLUS:
            return "PLUS";
        case TOKEN_TYPE::MINUS:
            return "MINUS";
        case TOKEN_TYPE::MUL:
            return "MUL";
        case TOKEN_TYPE::DIV:
            return "DIV";
        case TOKEN_TYPE::LPAREN:
            return "LPAREN";
        case TOKEN_TYPE::RPAREN:
            return "RPAREN";
        case TOKEN_TYPE::VARIABLE:
            return "VARIABLE";
        case TOKEN_TYPE::ASSIGN:
            return "ASSIGN";
        case TOKEN_TYPE::SEMI:
            return "SEMI";
        case TOKEN_TYPE::DOT:
            return "DOT";
        case TOKEN_TYPE::ERROR:
            return "ERROR";
        default:
            return "?";
        }
    }

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
        case TOKEN_TYPE::VARIABLE:
            return sValue;
        case TOKEN_TYPE::ASSIGN:
            return ":=";
        case TOKEN_TYPE::SEMI:
            return ";";
        case TOKEN_TYPE::DOT:
            return ".";
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

    void advance()
    {
        pos++;
    }

    char getCurrentChar()
    {
        return inp[pos];
    }

    char peek()
    {
        return inp[pos + 1];
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

        if ('a' <= getCurrentChar() && getCurrentChar() <= 'z')
        {
            ret.type = Token::TOKEN_TYPE::VARIABLE;
            ret.sValue.clear();
            ret.sValue += getCurrentChar();
            advance();
            while ('a' <= getCurrentChar() && getCurrentChar() <= 'z')
            {
                ret.sValue += getCurrentChar();
                advance();
            }
            return ret;
        }

        if (getCurrentChar() == ':')
        {
            if (peek() == '=')
            {
                advance();
                advance();
                ret.type = Token::TOKEN_TYPE::ASSIGN;
                return ret;
            }
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

    void test()
    {
        pos = 0;
        while (pos < inp.size())
        {
            Token t = nextToken();
            std::cout << t.toString() << ' ';
        }
        pos = 0;
    }
};

class Interpreter
{
private:
    Token _currentToken;

    Token getCurrentToken()
    {
        return _currentToken;
    }

    void eat(Token::TOKEN_TYPE tokenType)
    {
        if (getCurrentToken().type != tokenType)
        {
            fprintf(stderr, "currentToken.type: %s ; expected type: %s",
                    Token::tokenTypeToString(getCurrentToken().type), Token::tokenTypeToString(tokenType));
            exit(1);
        }

        _currentToken = lexer->nextToken();
    }

    int factor()
    {
        // factor = ( ( "+" | "-" ), factor ) | INTEGER | ( "(", expr, ")" ) ;
        int result;

        if (getCurrentToken().type == Token::TOKEN_TYPE::PLUS)
        {
            eat(Token::TOKEN_TYPE::PLUS);
            return factor();
        }

        if (getCurrentToken().type == Token::TOKEN_TYPE::MINUS)
        {
            eat(Token::TOKEN_TYPE::MINUS);
            return -factor();
        }

        if (getCurrentToken().type == Token::TOKEN_TYPE::INTEGER)
        {
            result = getCurrentToken().value;
            eat(Token::TOKEN_TYPE::INTEGER);
            return result;
        }

        if (getCurrentToken().type == Token::TOKEN_TYPE::VARIABLE)
        {
            result = globalScope[getCurrentToken().sValue];
            eat(Token::TOKEN_TYPE::VARIABLE);
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

        while (getCurrentToken().type == Token::TOKEN_TYPE::MUL || getCurrentToken().type == Token::TOKEN_TYPE::DIV)
        {
            if (getCurrentToken().type == Token::TOKEN_TYPE::MUL)
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

    Lexer *lexer;
    
    public:
    std::map<std::string, int> globalScope;
    void setLexer(Lexer *lexer)
    {
        this->lexer = lexer;
        _currentToken = (this->lexer)->nextToken();
    }

    int expr()
    {
        // expr = term, { ( "+" | "-" ), term } ;
        int result = term();

        while (getCurrentToken().type == Token::TOKEN_TYPE::PLUS || getCurrentToken().type == Token::TOKEN_TYPE::MINUS)
        {
            if (getCurrentToken().type == Token::TOKEN_TYPE::PLUS)
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

    void assignmentStatement()
    {
        std::string s = getCurrentToken().sValue;
        eat(Token::TOKEN_TYPE::VARIABLE);
        eat(Token::TOKEN_TYPE::ASSIGN);
        int result = expr();
        globalScope[s] = result;
    }

    void statementList(){
        assignmentStatement();
        eat(Token::TOKEN_TYPE::SEMI);
        while(getCurrentToken().type == Token::TOKEN_TYPE::VARIABLE){
            assignmentStatement();
            eat(Token::TOKEN_TYPE::SEMI);
        }
    }

    void interprete()
    {
        statementList();
        eat(Token::TOKEN_TYPE::DOT);
    }
};

int main(int argc, char **argv)
{
    std::string s;
    std::getline(std::cin, s);
    Lexer lexer;
    lexer.inp = s;

    // lexer.test();
    // return 0;

    Interpreter interpreter;
    interpreter.setLexer(&lexer);
    interpreter.interprete();

    for(const auto& [key, value] : interpreter.globalScope){
        std::cout<<key << ' ' << value<<'\n';
    }
    return 0;
}
