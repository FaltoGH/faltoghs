#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <map>

bool verbose = false;

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

class Token
{
public:
    TOKEN_TYPE type;
    int value;
    std::string sValue;

    static const char *tokenTypeToString(TOKEN_TYPE type)
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
            ret.type = TOKEN_TYPE::VARIABLE;
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
                ret.type = TOKEN_TYPE::ASSIGN;
                return ret;
            }
        }

        if ('0' <= getCurrentChar() && getCurrentChar() <= '9')
        {
            ret.type = TOKEN_TYPE::INTEGER;
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
            ret.type = TOKEN_TYPE::PLUS;
            advance();
            return ret;
        }

        if (getCurrentChar() == '-')
        {
            ret.type = TOKEN_TYPE::MINUS;
            advance();
            return ret;
        }

        if (getCurrentChar() == '*')
        {
            ret.type = TOKEN_TYPE::MUL;
            advance();
            return ret;
        }

        if (getCurrentChar() == '/')
        {
            ret.type = TOKEN_TYPE::DIV;
            advance();
            return ret;
        }

        if (getCurrentChar() == '(')
        {
            ret.type = TOKEN_TYPE::LPAREN;
            advance();
            return ret;
        }

        if (getCurrentChar() == ')')
        {
            ret.type = TOKEN_TYPE::RPAREN;
            advance();
            return ret;
        }

        if (getCurrentChar() == ';')
        {
            ret.type = TOKEN_TYPE::SEMI;
            advance();
            return ret;
        }

        if (getCurrentChar() == '.')
        {
            ret.type = TOKEN_TYPE::DOT;
            advance();
            return ret;
        }

        ret.type = TOKEN_TYPE::ERROR;
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

enum class AST_TYPE
{
    INTEGER,
    VARIABLE,
    UNARY_OP,
    BIN_OP,
    ASSIGNMENT_STATEMENT,
    STATEMENT_LIST
};

class Ast
{
public:
    Ast(AST_TYPE astType)
    {
        this->astType = astType;
        child = nullptr;
        child2 = nullptr;
    }
    AST_TYPE astType;
    Token token;
    Ast *child;
    Ast *child2;
    std::vector<Ast *> children;
};


class DepthCounter
{
private:
    static int depth;
    std::string name;
public:
    DepthCounter(const std::string& name)
    {
        if(verbose){
            std::cout << std::string(depth*2, ' ') << name << "++\n";
        }
        ++depth;
        this->name = name;

        if(depth > 99){
            puts("ERROR");
            exit(0);
        }
    }

    ~DepthCounter()
    {
        --depth;
        if(verbose){
            std::cout << std::string(depth*2, ' ') << name << "--\n";
        }
    }
};

int DepthCounter::depth = 0;

class Parser
{
private:
    Token _currentToken;

    Token getCurrentToken()
    {
        return _currentToken;
    }

    void eat(TOKEN_TYPE tokenType)
    {
        if (getCurrentToken().type != tokenType)
        {
            fprintf(stderr, "currentToken.type: %s ; expected type: %s",
                    Token::tokenTypeToString(getCurrentToken().type), Token::tokenTypeToString(tokenType));
            exit(1);
        }

        _currentToken = lexer->nextToken();
    }

    Ast *factor()
    {
        DepthCounter dc(__func__);

        // factor = ( ( "+" | "-" ), factor ) | INTEGER | ( "(", expr, ")" ) ;
        int result;

        if (getCurrentToken().type == TOKEN_TYPE::PLUS)
        {
            eat(TOKEN_TYPE::PLUS);
            return factor();
        }

        if (getCurrentToken().type == TOKEN_TYPE::MINUS)
        {
            Ast *ret = new Ast(AST_TYPE::UNARY_OP);
            ret->token = getCurrentToken();
            eat(TOKEN_TYPE::MINUS);
            ret->child = factor();
            return ret;
        }

        if (getCurrentToken().type == TOKEN_TYPE::INTEGER)
        {
            Ast *ret = new Ast(AST_TYPE::INTEGER);
            ret->token = getCurrentToken();
            eat(TOKEN_TYPE::INTEGER);
            return ret;
        }

        if (getCurrentToken().type == TOKEN_TYPE::VARIABLE)
        {
            Ast *ret = new Ast(AST_TYPE::VARIABLE);
            ret->token = getCurrentToken();
            eat(TOKEN_TYPE::VARIABLE);
            return ret;
        }

        {
            eat(TOKEN_TYPE::LPAREN);
            Ast *ret = expr();
            eat(TOKEN_TYPE::RPAREN);
            return ret;
        }
    }

    Ast *term()
    {
        DepthCounter dc(__func__);

        // term = factor, { ( "*" | "/" ), factor } ;
        Ast *ret = factor();

        while (getCurrentToken().type == TOKEN_TYPE::MUL || getCurrentToken().type == TOKEN_TYPE::DIV)
        {
            if (getCurrentToken().type == TOKEN_TYPE::MUL)
            {
                Ast *newret = new Ast(AST_TYPE::BIN_OP);
                newret->token = getCurrentToken();
                eat(TOKEN_TYPE::MUL);
                newret->child = ret;
                newret->child2 = factor();
                ret = newret;
            }
            else
            {
                Ast *newret = new Ast(AST_TYPE::BIN_OP);
                newret->token = getCurrentToken();
                eat(TOKEN_TYPE::DIV);
                newret->child = ret;
                newret->child2 = factor();
                ret = newret;
            }
        }

        return ret;
    }

    Lexer *lexer;

public:
    void setLexer(Lexer *lexer)
    {
        this->lexer = lexer;
        _currentToken = (this->lexer)->nextToken();
    }

    Ast *expr()
    {
        DepthCounter dc(__func__);

        // expr = term, { ( "+" | "-" ), term } ;
        Ast *ret = term();

        while (getCurrentToken().type == TOKEN_TYPE::PLUS || getCurrentToken().type == TOKEN_TYPE::MINUS)
        {
            if (getCurrentToken().type == TOKEN_TYPE::PLUS)
            {
                Ast *newret = new Ast(AST_TYPE::BIN_OP);
                newret->token = getCurrentToken();
                eat(TOKEN_TYPE::PLUS);
                newret->child = ret;
                newret->child2 = term();
                ret = newret;
            }
            else
            {
                Ast *newret = new Ast(AST_TYPE::BIN_OP);
                newret->token = getCurrentToken();
                eat(TOKEN_TYPE::MINUS);
                newret->child = ret;
                newret->child2 = term();
                ret = newret;
            }
        }
        return ret;
    }

    Ast *assignmentStatement()
    {
        Ast *ret = new Ast(AST_TYPE::ASSIGNMENT_STATEMENT);
        Ast *variableAst = new Ast(AST_TYPE::VARIABLE);
        ret->child = variableAst;
        variableAst->token = getCurrentToken();
        eat(TOKEN_TYPE::VARIABLE);
        ret->token = getCurrentToken();
        eat(TOKEN_TYPE::ASSIGN);
        Ast *exprAst = expr();
        ret->child2 = exprAst;
        return ret;
    }

    Ast *statementList()
    {
        if (verbose)
        {
            puts("statementList()++");
        }
        Ast *ret = new Ast(AST_TYPE::STATEMENT_LIST);
        Ast *assignmentStatementAst = assignmentStatement();
        ret->children.push_back(assignmentStatementAst);
        eat(TOKEN_TYPE::SEMI);
        while (getCurrentToken().type == TOKEN_TYPE::VARIABLE)
        {
            assignmentStatementAst = assignmentStatement();
            ret->children.push_back(assignmentStatementAst);
            eat(TOKEN_TYPE::SEMI);
        }
        return ret;
    }

    Ast *parse()
    {
        Ast *statementListAst = statementList();
        eat(TOKEN_TYPE::DOT);
        return statementListAst;
    }
};

class Interpreter
{
    int evaluate(Ast *ast)
    {
        switch (ast->astType)
        {
        case AST_TYPE::INTEGER:
            return ast->token.value;
        case AST_TYPE::VARIABLE:
            return globalScope[ast->token.sValue];
        case AST_TYPE::UNARY_OP:
            if (ast->token.type == TOKEN_TYPE::MINUS)
            {
                return -evaluate(ast->child);
            }
        case AST_TYPE::BIN_OP:
            switch (ast->token.type)
            {
            case TOKEN_TYPE::PLUS:
                return evaluate(ast->child) + evaluate(ast->child2);
            case TOKEN_TYPE::MINUS:
                return evaluate(ast->child) - evaluate(ast->child2);
            case TOKEN_TYPE::MUL:
                return evaluate(ast->child) * evaluate(ast->child2);
            case TOKEN_TYPE::DIV:
                return evaluate(ast->child) / evaluate(ast->child2);
            default:
                break;
            }
        default:
            break;
        }
        return 0;
    }

public:
    std::map<std::string, int> globalScope;

    void visit(Ast *ast)
    {
        if (ast->astType == AST_TYPE::STATEMENT_LIST)
        {
            for (const auto c : ast->children)
            {
                visit(c);
            }
        }
        else if (ast->astType == AST_TYPE::ASSIGNMENT_STATEMENT)
        {
            globalScope[ast->child->token.sValue] = evaluate(ast->child2);
        }
    }
};

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        verbose = true;
        freopen("input.txt", "r", stdin);
    }

    std::string s;
    std::getline(std::cin, s);
    Lexer lexer;
    lexer.inp = s;

    // lexer.test();
    // return 0;

    Parser parser;
    parser.setLexer(&lexer);
    Ast *ast = parser.parse();

    if (verbose)
    {
        puts("-----END PARSE-----");
    }

    Interpreter interpreter;
    interpreter.visit(ast);

    for (const auto &[key, value] : interpreter.globalScope)
    {
        std::cout << key << ' ' << value << '\n';
    }
    return 0;
}
