#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <map>

#define INPUT_MAX 1024

int DEBUG = 0;
int TEST = 0;

enum class TOKEN_TYPE
{
    ASSIGN,
    DIV,
    ERROR,
    GT,
    IF,
    INT,
    LCURLY,
    LPAREN,
    LT,
    MINUS,
    MUL,
    PLUS,
    RCURLY,
    RPAREN,
    SEMI,
    VAR
};

const char *TOKEN_TYPE_STRINGS[16] = {
    "ASSIGN",
    "DIV",
    "ERROR",
    "GT",
    "IF",
    "INT",
    "LCURLY",
    "LPAREN",
    "LT",
    "MINUS",
    "MUL",
    "PLUS",
    "RCURLY",
    "RPAREN",
    "SEMI",
    "VAR"};

class Token
{

public:
    Token()
    {
        type = TOKEN_TYPE::ERROR;
        intValue = 0;
    }

    TOKEN_TYPE type;
    int intValue;
    std::string stringValue;

    std::string toString()
    {
        switch (type)
        {
        case TOKEN_TYPE::ASSIGN:
            return "=";
        case TOKEN_TYPE::DIV:
            return "/";
        case TOKEN_TYPE::ERROR:
            return std::string("ERROR(") + std::to_string(intValue) + std::string(")");
        case TOKEN_TYPE::GT:
            return ">";
        case TOKEN_TYPE::IF:
            return "if";
        case TOKEN_TYPE::INT:
            return std::to_string(intValue);
        case TOKEN_TYPE::LCURLY:
            return "{";
        case TOKEN_TYPE::LT:
            return "<";
        case TOKEN_TYPE::LPAREN:
            return "(";
        case TOKEN_TYPE::MINUS:
            return "-";
        case TOKEN_TYPE::MUL:
            return "*";
        case TOKEN_TYPE::PLUS:
            return "+";
        case TOKEN_TYPE::RCURLY:
            return "}";
        case TOKEN_TYPE::RPAREN:
            return ")";
        case TOKEN_TYPE::SEMI:
            return ";";
        case TOKEN_TYPE::VAR:
            return stringValue;
        default:
            return "UNCLASSIFIED";
        }
    }
};

TOKEN_TYPE oneShot(char x)
{
    switch (x)
    {
    case '=':
        return TOKEN_TYPE::ASSIGN;
    case '/':
        return TOKEN_TYPE::DIV;
    case '>':
        return TOKEN_TYPE::GT;
    case '{':
        return TOKEN_TYPE::LCURLY;
    case '(':
        return TOKEN_TYPE::LPAREN;
    case '<':
        return TOKEN_TYPE::LT;
    case '-':
        return TOKEN_TYPE::MINUS;
    case '*':
        return TOKEN_TYPE::MUL;
    case '+':
        return TOKEN_TYPE::PLUS;
    case '}':
        return TOKEN_TYPE::RCURLY;
    case ')':
        return TOKEN_TYPE::RPAREN;
    case ';':
        return TOKEN_TYPE::SEMI;
    default:
        return TOKEN_TYPE::ERROR;
    }
}

class Lexer
{

    void advance()
    {
        pos++;
        col++;
    }

    char getCurrentChar()
    {
        return input[pos];
    }

    char peek()
    {
        return input[pos + 1];
    }

public:
    int pos = 0;
    int ln = 1;
    int col = 1;
    char *input;

    Token nextToken()
    {
        Token ret;

        // Skip white space.
        while ((getCurrentChar() == ' ') || (getCurrentChar() == 32) || getCurrentChar() == '\r' || getCurrentChar() == '\n')
        {
            if (getCurrentChar() == '\r')
            {
                advance();
                if (getCurrentChar() == '\n')
                {
                    advance();
                }
                ln++;
                col = 1;
            }
            else if(getCurrentChar() == '\n')
            {
                advance();
                ln++;
                col = 1;
            }
            else{
                advance();
            }
        }


        if (getCurrentChar() == 'i')
        {
            if (peek() == 'f')
            {
                ret.type = TOKEN_TYPE::IF;
                advance();
                advance();
                return ret;
            }
        }

        if ('a' <= getCurrentChar() && getCurrentChar() <= 'z')
        {
            ret.type = TOKEN_TYPE::VAR;
            ret.stringValue += getCurrentChar();
            advance();
            while ('a' <= getCurrentChar() && getCurrentChar() <= 'z')
            {
                ret.stringValue += getCurrentChar();
                advance();
            }
            return ret;
        }

        if ('0' <= getCurrentChar() && getCurrentChar() <= '9')
        {
            ret.type = TOKEN_TYPE::INT;
            ret.intValue = (getCurrentChar() - '0');
            advance();
            while ('0' <= getCurrentChar() && getCurrentChar() <= '9')
            {
                ret.intValue = (ret.intValue * 10) + (getCurrentChar() - '0');
                advance();
            }
            return ret;
        }

        ret.type = oneShot(getCurrentChar());
        if(ret.type == TOKEN_TYPE::ERROR){
            ret.intValue = getCurrentChar();
        }
        advance();
        return ret;
    }

    void test()
    {
        pos = 0;
        size_t len = strlen(input);
        while (pos < len)
        {
            Token t = nextToken();
            std::cout << t.toString() << ' ';
        }
        pos = 0;
    }
};

enum class AST_TYPE
{
    ASSIGNMENT_STATEMENT,
    BIN_OP,
    ERROR,
    IF_STATEMENT,
    INT,
    STATEMENT_LIST,
    UNARY_OP,
    VAR
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
    DepthCounter(const std::string &name)
    {
        if (DEBUG)
        {
            std::cout << std::string(depth, ' ') << name << "++\n";
        }
        ++depth;
        this->name = name;

        if (depth > 100)
        {
            fprintf(stderr, "stack overflow\r\n");
            exit(0);
        }
    }

    ~DepthCounter()
    {
        --depth;
        if (DEBUG)
        {
            std::cout << std::string(depth, ' ') << name << "--\n";
        }
    }
};

int DepthCounter::depth = 0;

class Parser
{
private:
    Token _currentToken;
    Lexer *lexer;

    Token getCurrentToken()
    {
        return _currentToken;
    }

    void eat(TOKEN_TYPE tokenType)
    {
        if (getCurrentToken().type != tokenType)
        {
            const char *currentTokenTypeString = TOKEN_TYPE_STRINGS[(int)(getCurrentToken().type)];
            const char *expectedTypeString = TOKEN_TYPE_STRINGS[(int)(tokenType)];

            fprintf(stderr, "cant eat ; Ln %d, Col %d ; currentToken.type: %s ; expected type: %s", lexer->ln, lexer->col, currentTokenTypeString, expectedTypeString);
            exit(0);
        }

        _currentToken = lexer->nextToken();
    }

    Ast *factor()
    {
        DepthCounter dc(__func__);
        // factor = INT | VAR | ( "(", cond, ")" ) | ( ( "+" | "-" ), factor ) ;

        if (getCurrentToken().type == TOKEN_TYPE::INT)
        {
            Ast *ret = new Ast(AST_TYPE::INT);
            ret->token = getCurrentToken();
            eat(TOKEN_TYPE::INT);
            return ret;
        }

        if (getCurrentToken().type == TOKEN_TYPE::VAR)
        {
            Ast *ret = new Ast(AST_TYPE::VAR);
            ret->token = getCurrentToken();
            eat(TOKEN_TYPE::VAR);
            return ret;
        }

        if (getCurrentToken().type == TOKEN_TYPE::LPAREN)
        {
            eat(TOKEN_TYPE::LPAREN);
            Ast *ret = cond();
            eat(TOKEN_TYPE::RPAREN);
            return ret;
        }

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

        fprintf(stderr, "cant factor\r\n");
        exit(0);
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

    Ast *cond()
    {
        // cond = expr, { ( ">" | "<" ) expr } ;
        Ast *ret = expr();

        while (getCurrentToken().type == TOKEN_TYPE::GT || getCurrentToken().type == TOKEN_TYPE::LT)
        {
            if (getCurrentToken().type == TOKEN_TYPE::GT)
            {
                Ast *newret = new Ast(AST_TYPE::BIN_OP);
                newret->token = getCurrentToken();
                eat(TOKEN_TYPE::GT);
                newret->child = ret;
                newret->child2 = expr();
                ret = newret;
            }
            else
            {
                Ast *newret = new Ast(AST_TYPE::BIN_OP);
                newret->token = getCurrentToken();
                eat(TOKEN_TYPE::LT);
                newret->child = ret;
                newret->child2 = expr();
                ret = newret;
            }
        }
        return ret;
    }

    Ast *assignmentStatement()
    {
        DepthCounter dc(__func__);
        Ast *variableAst = new Ast(AST_TYPE::VAR);
        variableAst->token = getCurrentToken();
        eat(TOKEN_TYPE::VAR);
        eat(TOKEN_TYPE::ASSIGN);
        Ast *exprAst = expr();
        eat(TOKEN_TYPE::SEMI);
        Ast *ret = new Ast(AST_TYPE::ASSIGNMENT_STATEMENT);
        ret->child = variableAst;
        ret->child2 = exprAst;
        return ret;
    }

    Ast *ifStatement()
    {
        DepthCounter dc(__func__);
        eat(TOKEN_TYPE::IF);
        eat(TOKEN_TYPE::LPAREN);
        Ast *condAst = cond();
        eat(TOKEN_TYPE::RPAREN);
        eat(TOKEN_TYPE::LCURLY);
        Ast *ret = new Ast(AST_TYPE::IF_STATEMENT);
        ret->children.push_back(condAst);
        while (getCurrentToken().type != TOKEN_TYPE::RCURLY)
        {
            ret->children.push_back(statement());
        }
        eat(TOKEN_TYPE::RCURLY);
        return ret;
    }

    Ast *statement()
    {
        if (getCurrentToken().type == TOKEN_TYPE::IF)
        {
            return ifStatement();
        }
        return assignmentStatement();
    }

    Ast *statementList()
    {
        Ast *ret = new Ast(AST_TYPE::STATEMENT_LIST);
        while (getCurrentToken().type == TOKEN_TYPE::VAR || getCurrentToken().type == TOKEN_TYPE::IF)
        {
            ret->children.push_back(statement());
        }
        return ret;
    }

public:
    void setLexer(Lexer *lexer)
    {
        DepthCounter dc(__func__);
        this->lexer = lexer;
        _currentToken = (this->lexer)->nextToken();
    }

    Ast *parse()
    {
        Ast *statementListAst = statementList();
        return statementListAst;
    }
};

class Interpreter
{
    int evaluate(Ast *ast)
    {
        switch (ast->astType)
        {
        case AST_TYPE::INT:
            return ast->token.intValue;
        case AST_TYPE::VAR:
            return globalScope[ast->token.stringValue];
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
                case TOKEN_TYPE::GT:
                return evaluate(ast->child) > evaluate(ast->child2);
                case TOKEN_TYPE::LT:
                return evaluate(ast->child) < evaluate(ast->child2);
            default:
                fprintf(stderr, "cant bin op\r\n");
                exit(0);
                break;
            }
        default:
            fprintf(stderr, "cant eval\r\n");
            exit(0);
            break;
        }
        return 0;
    }

public:
    std::map<std::string, int> globalScope;

    void visit(Ast *ast)
    {
        switch (ast->astType)
        {
        case AST_TYPE::STATEMENT_LIST:
            for (const auto c : ast->children)
            {
                visit(c);
            }
            break;
        case AST_TYPE::ASSIGNMENT_STATEMENT:
            globalScope[ast->child->token.stringValue] = evaluate(ast->child2);
            break;
        case AST_TYPE::IF_STATEMENT:
        {
            int result = evaluate((ast->children)[0]);
            if (result)
            {
                if(DEBUG){
                    puts("if true");
                }
                for (int i = 1; i < (ast->children).size(); i++)
                {
                    visit((ast->children)[i]);
                }
            }
            else{
                if(DEBUG){
                    puts("if false");
                }
            }
        }
            break;
        default:
            fprintf(stderr, "cant visit\r\n");
            exit(0);
            break;
        }
    }
    
};

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--debug") == 0)
        {
            DEBUG = 1;
            puts("DEBUG=1");
        }
        else if (strcmp(argv[i], "--test") == 0)
        {
            TEST = 1;
            puts("TEST=1");
        }
    }

    FILE *file = fopen("input.txt", "r");
    char buffer[INPUT_MAX];
    fread(buffer, sizeof(char), sizeof(buffer), file);

    Lexer lexer;
    lexer.input = buffer;

    if (TEST)
    {
        lexer.test();
        return 0;
    }

    Parser parser;
    parser.setLexer(&lexer);
    Ast *ast = parser.parse();

    Interpreter interpreter;
    interpreter.visit(ast);

    for (const auto &[key, value] : interpreter.globalScope)
    {
        std::cout << key << ' ' << value << '\n';
    }
    return 0;
}
