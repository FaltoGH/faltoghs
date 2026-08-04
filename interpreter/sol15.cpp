#include <assert.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <map>
#include <vector>

#define INPUT_MAX 1024
#define stacktrace() DepthCounter __dc(__func__)

enum class TOKEN_TYPE
{
    ASSIGN,
    DIV,
    eof,
    ERROR,
    FOR,
    GT,
    ID,
    IF,
    INT,
    LCURLY,
    LPAREN,
    LT,
    MINUS,
    MUL,
    PLUS,
    PRINT,
    RCURLY,
    RPAREN,
    SEMI,
    WHILE
};
const char *TOKEN_TYPE_STRINGS[20] = {
    "ASSIGN",
    "DIV",
    "eof",
    "ERROR",
    "FOR",
    "GT",
    "ID",
    "IF",
    "INT",
    "LCURLY",
    "LPAREN",
    "LT",
    "MINUS",
    "MUL",
    "PLUS",
    "PRINT",
    "RCURLY",
    "RPAREN",
    "SEMI",
    "WHILE"};
enum class AST_TYPE
{
    ASSIGN,
    DIV,
    ERROR,
    FOR,
    GT,
    IF,
    INT,
    LT,
    MINUS,
    MUL,
    PLUS,
    PRINT,
    STMT_LIST,
    VAR,
    WHILE
};
const char *AST_TYPE_STRINGS[15] = {
    "ASSIGN",
    "DIV",
    "ERROR",
    "FOR",
    "GT",
    "IF",
    "INT",
    "LT",
    "MINUS",
    "MUL",
    "PLUS",
    "PRINT",
    "STMT_LIST",
    "VAR",
    "WHILE"};

int DEBUG = 0;
int TEST = 0;

class DepthCounter
{
private:
    static int depth;
    static std::vector<std::string> __stackTrace;
    std::string name;

public:
    DepthCounter(const std::string &name)
    {
        // if (DEBUG)
        // {
        //     std::cout << std::string(depth, ' ') << name << "++\n";
        // }
        ++depth;
        this->name = name;
        __stackTrace.push_back(name);

        if (depth > 100)
        {
            fprintf(stderr, "stack overflow\r\n");
            exit(0);
        }
    }
    ~DepthCounter()
    {
        --depth;
        // if (DEBUG)
        // {
        //     std::cout << std::string(depth, ' ') << name << "--\n";
        // }
        __stackTrace.pop_back();
    }
    static void print()
    {
        std::cout << "stacktrace: ";
        for (auto s : __stackTrace)
        {
            std::cout << s << ' ';
        }
        std::cout << "\r\n";
    }
};
int DepthCounter::depth = 0;
std::vector<std::string> DepthCounter::__stackTrace;

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

    int line;
    int column;

    std::string toString()
    {
        switch (type)
        {
        case TOKEN_TYPE::ASSIGN:
            return "=";
        case TOKEN_TYPE::DIV:
            return "/";
        case TOKEN_TYPE::eof:
            return "eof";
        case TOKEN_TYPE::ERROR:
            return std::string("ERROR(") + std::to_string(intValue) + std::string(")");
        case TOKEN_TYPE::FOR:
            return "for";
        case TOKEN_TYPE::GT:
            return ">";
        case TOKEN_TYPE::ID:
            return std::string("ID(") + stringValue + std::string(")");
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
        case TOKEN_TYPE::PRINT:
            return "print";
        case TOKEN_TYPE::RCURLY:
            return "}";
        case TOKEN_TYPE::RPAREN:
            return ")";
        case TOKEN_TYPE::SEMI:
            return ";";
        case TOKEN_TYPE::WHILE:
            return "while";
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
        column++;
    }

    char getCurrentChar()
    {
        return input[pos];
    }

    int pos = 0;
    int line = 1;
    int column = 1;

    Token nextToken()
    {
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
                line++;
                column = 1;
            }
            else if (getCurrentChar() == '\n')
            {
                advance();
                line++;
                column = 1;
            }
            else
            {
                advance();
            }
        }

        Token ret;
        ret.line = line;
        ret.column = column;

        if (getCurrentChar() == '\0')
        {
            ret.type = TOKEN_TYPE::eof;
        }
        else if ('a' <= getCurrentChar() && getCurrentChar() <= 'z')
        {
            ret.type = TOKEN_TYPE::ID;
            ret.stringValue += getCurrentChar();
            advance();
            while ('a' <= getCurrentChar() && getCurrentChar() <= 'z')
            {
                ret.stringValue += getCurrentChar();
                advance();
            }

            if (ret.stringValue == "for")
            {
                ret.type = TOKEN_TYPE::FOR;
            }
            else if (ret.stringValue == "if")
            {
                ret.type = TOKEN_TYPE::IF;
            }
            else if (ret.stringValue == "print")
            {
                ret.type = TOKEN_TYPE::PRINT;
            }
            else if (ret.stringValue == "while")
            {
                ret.type = TOKEN_TYPE::WHILE;
            }
        }
        else if ('0' <= getCurrentChar() && getCurrentChar() <= '9')
        {
            ret.type = TOKEN_TYPE::INT;
            ret.intValue = (getCurrentChar() - '0');
            advance();
            while ('0' <= getCurrentChar() && getCurrentChar() <= '9')
            {
                ret.intValue = (ret.intValue * 10) + (getCurrentChar() - '0');
                advance();
            }
        }
        else
        {
            ret.type = oneShot(getCurrentChar());
            if (ret.type == TOKEN_TYPE::ERROR)
            {
                ret.intValue = getCurrentChar();
            }
            advance();
        }
        return ret;
    }

public:
    char *input;
    std::vector<Token> lex()
    {
        std::vector<Token> ret;
        pos = 0;
        int len = strlen(input);

        while (pos < len)
        {
            Token t = nextToken();
            ret.push_back(t);
        }

        // Add EOF if it does not exists.
        if (ret.size() > 0)
        {
            if (ret[ret.size() - 1].type != TOKEN_TYPE::eof)
            {
                Token eof;
                eof.type = TOKEN_TYPE::eof;
                ret.push_back(eof);
            }
        }

        return ret;
    }
};

class Ast
{
public:
    AST_TYPE astType;
    std::vector<Ast *> children;
    std::string stringValue;
    int intValue;

    Ast(AST_TYPE astType)
    {
        this->astType = astType;
        intValue = 0;
    }

    void addChild(Ast *child)
    {
        children.push_back(child);
    }
};

class Parser
{
private:
    int pos = 0;

    Token getCurrentToken()
    {
        if ((size_t)pos < tokens.size())
        {
            return tokens[pos];
        }
        fprintf(stderr, "error: index out of range");
        exit(0);
    }

    Token getNextToken()
    {
        if ((size_t)pos + 1 < tokens.size())
        {
            return tokens[pos + 1];
        }

        fprintf(stderr, "error: index out of range");
        exit(0);
    }

    void eat(TOKEN_TYPE tokenType)
    {
        Token currentToken = getCurrentToken();

        if (currentToken.type != tokenType)
        {
            const char *currentTokenTypeString = TOKEN_TYPE_STRINGS[(int)(currentToken.type)];
            const char *expectedTypeString = TOKEN_TYPE_STRINGS[(int)(tokenType)];

            fprintf(stderr, "cant eat ; Ln %d, Col %d ; currentToken.type: %s ; expected type: %s", currentToken.line, currentToken.column, currentTokenTypeString, expectedTypeString);
            exit(0);
        }

        // if (DEBUG)
        // {
        //     printf("debug: eat %d:%d\r\n", currentToken.line, currentToken.column);
        // }

        pos++;
    }

    Ast *factor()
    {
        stacktrace();

        // factor = INT | ID | ( "(", assign, ")" ) | ( ( "+" | "-" ), factor ) ;

        Token currentToken = getCurrentToken();
        TOKEN_TYPE t = currentToken.type;
        Ast *ret = NULL;

        switch (t)
        {
        case TOKEN_TYPE::INT:
        {
            ret = new Ast(AST_TYPE::INT);
            ret->intValue = getCurrentToken().intValue;
            eat(TOKEN_TYPE::INT);
            break;
        }

        case TOKEN_TYPE::ID:
        {
            ret = new Ast(AST_TYPE::VAR);
            ret->stringValue = getCurrentToken().stringValue;
            eat(TOKEN_TYPE::ID);
            break;
        }

        case TOKEN_TYPE::LPAREN:
        {
            eat(TOKEN_TYPE::LPAREN);
            ret = assign();
            eat(TOKEN_TYPE::RPAREN);
            break;
        }

        case TOKEN_TYPE::PLUS:
        {
            eat(TOKEN_TYPE::PLUS);
            ret = factor();
            break;
        }

        case TOKEN_TYPE::MINUS:
        {
            eat(TOKEN_TYPE::MINUS);
            ret = new Ast(AST_TYPE::MINUS);
            Ast *zero = new Ast(AST_TYPE::INT);
            zero->intValue = 0;
            ret->addChild(zero);
            ret->addChild(factor());
            break;
        }

        default:
            DepthCounter::print();
            fprintf(stderr, "error: %d:%d No factor starts with %s\r\n", currentToken.line, currentToken.column, TOKEN_TYPE_STRINGS[(int)t]);
            exit(0);
            break;
        }

        return ret;
    }

    Ast *term()
    {
        stacktrace();

        // term = factor, { ( "*" | "/" ), factor } ;
        Ast *ret = factor();

        while (getCurrentToken().type == TOKEN_TYPE::MUL || getCurrentToken().type == TOKEN_TYPE::DIV)
        {
            if (getCurrentToken().type == TOKEN_TYPE::MUL)
            {
                eat(TOKEN_TYPE::MUL);
                Ast *mul = new Ast(AST_TYPE::MUL);
                mul->addChild(ret);
                mul->addChild(factor());
                ret = mul;
            }
            else
            {
                eat(TOKEN_TYPE::DIV);
                Ast *div = new Ast(AST_TYPE::DIV);
                div->addChild(ret);
                div->addChild(factor());
                ret = div;
            }
        }

        return ret;
    }

    Ast *expr()
    {
        stacktrace();

        // expr = term, { ( "+" | "-" ), term } ;
        Ast *ret = term();

        while (getCurrentToken().type == TOKEN_TYPE::PLUS || getCurrentToken().type == TOKEN_TYPE::MINUS)
        {
            if (getCurrentToken().type == TOKEN_TYPE::PLUS)
            {
                eat(TOKEN_TYPE::PLUS);
                Ast *plus = new Ast(AST_TYPE::PLUS);
                plus->addChild(ret);
                plus->addChild(term());
                ret = plus;
            }
            else
            {
                eat(TOKEN_TYPE::MINUS);
                Ast *minus = new Ast(AST_TYPE::MINUS);
                minus->addChild(ret);
                minus->addChild(term());
                ret = minus;
            }
        }
        return ret;
    }

    Ast *cond()
    {
        stacktrace();

        // cond = expr, { ( ">" | "<" ) expr } ;
        Ast *ret = expr();

        while (getCurrentToken().type == TOKEN_TYPE::GT || getCurrentToken().type == TOKEN_TYPE::LT)
        {
            if (getCurrentToken().type == TOKEN_TYPE::GT)
            {
                eat(TOKEN_TYPE::GT);
                Ast *gt = new Ast(AST_TYPE::GT);
                gt->addChild(ret);
                gt->addChild(expr());
                ret = gt;
            }
            else
            {
                eat(TOKEN_TYPE::LT);
                Ast *lt = new Ast(AST_TYPE::LT);
                lt->addChild(ret);
                lt->addChild(expr());
                ret = lt;
            }
        }
        return ret;
    }

    Ast *assign()
    {
        stacktrace();

        // assign = cond | ( ID, "=", cond ) ;
        if (getCurrentToken().type == TOKEN_TYPE::ID && getNextToken().type == TOKEN_TYPE::ASSIGN)
        {
            if (DEBUG)
            {
                printf("debug: The assign is ( ID, \"=\", cond ).\r\n");
            }

            Ast *assignAst = new Ast(AST_TYPE::ASSIGN);
            assignAst->stringValue = getCurrentToken().stringValue;
            eat(TOKEN_TYPE::ID);
            eat(TOKEN_TYPE::ASSIGN);
            assignAst->addChild(cond());
            return assignAst;
        }
        else
        {
            if (DEBUG)
            {
                printf("debug: The assign is cond.\r\n");
            }
            return cond();
        }
    }

    Ast *forStmt()
    {
        stacktrace();
        Ast *forAst = new Ast(AST_TYPE::FOR);
        eat(TOKEN_TYPE::FOR);
        eat(TOKEN_TYPE::LPAREN);
        forAst->addChild(assign());
        eat(TOKEN_TYPE::SEMI);
        forAst->addChild(assign());
        eat(TOKEN_TYPE::SEMI);
        forAst->addChild(assign());
        eat(TOKEN_TYPE::RPAREN);
        eat(TOKEN_TYPE::LCURLY);
        forAst->addChild(stmtList());
        eat(TOKEN_TYPE::RCURLY);
        return forAst;
    }

    Ast *ifStmt()
    {
        stacktrace();
        Ast *ifAst = new Ast(AST_TYPE::IF);
        eat(TOKEN_TYPE::IF);
        eat(TOKEN_TYPE::LPAREN);
        ifAst->addChild(assign());
        eat(TOKEN_TYPE::RPAREN);
        eat(TOKEN_TYPE::LCURLY);
        ifAst->addChild(stmtList());
        eat(TOKEN_TYPE::RCURLY);
        return ifAst;
    }

    Ast *printStmt()
    {
        stacktrace();
        Ast *printAst = new Ast(AST_TYPE::PRINT);
        eat(TOKEN_TYPE::PRINT);
        eat(TOKEN_TYPE::LPAREN);
        printAst->addChild(assign());
        eat(TOKEN_TYPE::RPAREN);
        return printAst;
    }

    Ast *whileStmt()
    {
        stacktrace();
        Ast *whileAst = new Ast(AST_TYPE::WHILE);
        eat(TOKEN_TYPE::WHILE);
        eat(TOKEN_TYPE::LPAREN);
        whileAst->addChild(assign());
        eat(TOKEN_TYPE::RPAREN);
        eat(TOKEN_TYPE::LCURLY);
        whileAst->addChild(stmtList());
        eat(TOKEN_TYPE::RCURLY);
        return whileAst;
    }

    Ast *stmtList()
    {
        stacktrace();

        // stmt_list = { ( assign, ";" ) | for_stmt | if_stmt | ( print_stmt, ";" ) | while_stmt }
        Ast *ret = new Ast(AST_TYPE::STMT_LIST);
        Token currentToken = getCurrentToken();
        TOKEN_TYPE t = currentToken.type;
        while ((t == TOKEN_TYPE::INT) || (t == TOKEN_TYPE::ID) || (t == TOKEN_TYPE::FOR) || (t == TOKEN_TYPE::IF) || (t == TOKEN_TYPE::PRINT) || (t == TOKEN_TYPE::WHILE))
        {
            if (DEBUG)
            {
                printf("debug: stmtList(): The statement starts with %d:%d ", currentToken.line, currentToken.column);
                std::cout << currentToken.toString() << ".\r\n";
            }

            switch (t)
            {
            case TOKEN_TYPE::INT:
            case TOKEN_TYPE::ID:
                ret->addChild(assign());
                eat(TOKEN_TYPE::SEMI);
                break;

            case TOKEN_TYPE::FOR:
                ret->children.push_back(forStmt());
                break;

            case TOKEN_TYPE::IF:
                if (DEBUG)
                {
                    printf("debug: stmtList(): getting if statement...\r\n");
                }
                ret->children.push_back(ifStmt());
                break;

            case TOKEN_TYPE::PRINT:
                ret->children.push_back(printStmt());
                eat(TOKEN_TYPE::SEMI);
                break;

            case TOKEN_TYPE::WHILE:
                ret->children.push_back(whileStmt());
                break;

            default:
                fprintf(stderr, "error: %s is not a statement.\r\n", TOKEN_TYPE_STRINGS[(int)t]);
                exit(0);
                break;
            }

            currentToken = getCurrentToken();
            t = currentToken.type;
        }
        return ret;
    }

    std::vector<Token> tokens;

public:
    Parser(std::vector<Token> tokens)
    {
        this->tokens = tokens;
    }

    Ast *parse()
    {
        return stmtList();
    }
};

class Interpreter
{

public:
    std::map<std::string, int> globalScope;

    int visit(Ast *ast)
    {
        if (ast == nullptr)
        {
            fputs("cannot visit null", stderr);
            exit(0);
        }

        switch (ast->astType)
        {
        case AST_TYPE::ASSIGN:
        {
            int rvalue = visit(ast->children[0]);
            globalScope[ast->stringValue] = rvalue;
            return rvalue;
        }
        case AST_TYPE::DIV:
            return visit(ast->children[0]) / visit(ast->children[1]);

        case AST_TYPE::FOR:
        {
            visit(ast->children[0]);
            bool conditionMet = visit(ast->children[1]);
            if (DEBUG)
            {
                printf("visit for. Initial conditionMet = %d. ast->children[1] type is %s\r\n", conditionMet, AST_TYPE_STRINGS[(int)ast->children[1]->astType]);
            }
            while (conditionMet)
            {
                visit(ast->children[3]);
                visit(ast->children[2]);
                conditionMet = visit(ast->children[1]);
            }
            break;
        }

        case AST_TYPE::GT:
            return visit(ast->children[0]) > visit(ast->children[1]);

        case AST_TYPE::IF:
        {
            if (visit(ast->children[0]))
            {
                visit(ast->children[1]);
            }
            break;
        }
        case AST_TYPE::INT:
            return ast->intValue;

        case AST_TYPE::LT:
            return visit(ast->children[0]) < visit(ast->children[1]);

        case AST_TYPE::MINUS:
            return visit(ast->children[0]) - visit(ast->children[1]);

        case AST_TYPE::MUL:
            return visit(ast->children[0]) * visit(ast->children[1]);

        case AST_TYPE::PLUS:
            return visit(ast->children[0]) + visit(ast->children[1]);

        case AST_TYPE::PRINT:
        {
            int value = visit(ast->children[0]);
            printf("%d\r\n", value);
            break;
        }

        case AST_TYPE::STMT_LIST:
            for (const auto c : ast->children)
            {
                visit(c);
            }
            break;

        case AST_TYPE::VAR:
            return globalScope[ast->stringValue];

        case AST_TYPE::WHILE:
        {
            while (visit(ast->children[0]))
            {
                visit(ast->children[1]);
            }
            break;
        }

        default:
            fprintf(stderr, "error: cannot visit\r\n");
            exit(0);
            break;
        }

        return 0;
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
    std::vector<Token> tokens = lexer.lex();

    if (TEST)
    {
        for (auto t : tokens)
        {
            std::cout << t.toString() << ' ';
        }
        return 0;
    }

    Parser parser(tokens);
    Ast *ast = parser.parse();

    Interpreter interpreter;
    interpreter.visit(ast);

    if (DEBUG)
    {
        for (const auto &[key, value] : interpreter.globalScope)
        {
            std::cout << key << ' ' << value << '\n';
        }
    }

    return 0;
}
