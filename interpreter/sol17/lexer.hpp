#include <vector>
#include <string>
#include "token.hpp"

typedef struct _lexer
{
    std::string _input;
    int _pos = 0;
    int _line = 1;
    int _column = 1;

    void advance()
    {
        _pos++;
        _column++;
    }

    char getCurrentChar()
    {
        if(_pos < (int)_input.size()){
            return _input[_pos];
        }
        return '\0';
    }

    void skipWhiteSpace()
    {
        while ((getCurrentChar() == ' ') || (getCurrentChar() == 32) || getCurrentChar() == '\r' || getCurrentChar() == '\n')
        {
            if (getCurrentChar() == '\r')
            {
                advance();
                if (getCurrentChar() == '\n')
                {
                    advance();
                }
                _line++;
                _column = 1;
            }
            else if (getCurrentChar() == '\n')
            {
                advance();
                _line++;
                _column = 1;
            }
            else
            {
                advance();
            }
        }
    }

    
    token read()
    {
        skipWhiteSpace();

        token ret;
        ret._type = ttype::error;
        ret._line = _line;
        ret._column = _column;
        ret._intValue = 0;

        if (getCurrentChar() == '\0')
        {
            ret._type = ttype::eof;
        }
        else if ('a' <= getCurrentChar() && getCurrentChar() <= 'z')
        {
            int posBegin = _pos;
            advance();
            while ('a' <= getCurrentChar() && getCurrentChar() <= 'z')
            {
                advance();
            }
            int posEnd = _pos;
            ret._stringValue = _input.substr(posBegin, posEnd - posBegin);
            ret._type = token::stringToType(ret._stringValue);
        }
        else if ('0' <= getCurrentChar() && getCurrentChar() <= '9')
        {
            ret._type = ttype::iint;
            ret._intValue = (getCurrentChar() - '0');
            advance();
            while ('0' <= getCurrentChar() && getCurrentChar() <= '9')
            {
                ret._intValue = (ret._intValue * 10) + (getCurrentChar() - '0');
                advance();
            }
        }
        else
        {
            ret._type = token::charToType(getCurrentChar());
            if (ret._type == ttype::error)
            {
                ret._intValue = getCurrentChar();
            }
            advance();
        }

        return ret;
    }

    std::vector<token> lex()
    {
        std::vector<token> ret;
        _pos = 0;
        while (1)
        {
            token t = read();
            ret.push_back(t);
            if(t._type == ttype::eof){
                break;
            }
        }
        return ret;
    }
} lexer;
