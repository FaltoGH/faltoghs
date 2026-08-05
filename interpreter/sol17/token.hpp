#pragma once

#include <string>
#include <vector>

#include "ttype.h"

typedef struct _token
{
    static bool debug;
    static int total;
    _token(){
      _id=total;
      total++;
    }
    
    int _id;
    ttype _type;
    int _line;
    int _column;
    int _intValue;
    std::string _stringValue;
    std::vector<struct _token*> children;
    struct _token* parent;
    
    void addc(struct _token* child);
    static ttype charToType(char x);
    void prtdbg();
    static ttype stringToType(std::string s);
    std::string toString();
    
    int nc(){return children.size();}
    struct _token* c0(){ return children[0];}
    struct _token* c1(){ return children[1];}
    struct _token* c2(){ return children[2];}
    struct _token* c3(){ return children[3];}
    
} token;

int token::total=0;
bool token::debug=false;

void token::addc(struct _token* child)
{
    if(_id == child->_id){
      printf("error: tried to add itself as child\r\n");
      exit(0);
    }
    
    
    if(nc()>0){
      if((c0()->_id) == (child->_id)){
        printf("error: child duplicate\r\n");
        exit(0);
      }
    }
    
    children.push_back(child);
    child->parent=this;
}

ttype token::charToType(char x)
{
    switch (x)
    {
    case '=':
        return ttype::assign;
    case '/':
        return ttype::div;
    case '>':
        return ttype::gt;
    case '{':
        return ttype::lcurly;
    case '(':
        return ttype::lparen;
    case '<':
        return ttype::lt;
    case '-':
        return ttype::minus;
    case '*':
        return ttype::mul;
    case '+':
        return ttype::plus;
    case '}':
        return ttype::rcurly;
    case ')':
        return ttype::rparen;
    case ';':
        return ttype::semi;
    default:
        return ttype::error;
    }
}

void token::prtdbg(){
    std::cout<<"\r\ntoken Ln: "<<_line<<" Col: "<<_column<<" str: "<<toString()<<" id: "<<_id<<"\r\n";
}

ttype token::stringToType(std::string s)
{
    if (s == "break")
        return ttype::bbreak;
    if (s == "continue")
        return ttype::ccontinue;
    if (s == "else")
        return ttype::eelse;
    if (s == "for")
        return ttype::ffor;
    if (s == "if")
        return ttype::iif;
    if (s == "print")
        return ttype::pprint;
    if (s == "while")
        return ttype::wwhile;
    return ttype::iid;
}

std::string token::toString()
{
    switch (_type)
    {
    case ttype::assign:
        return "=";
    case ttype::bbreak:
        return "break";
    case ttype::ccontinue:
        return "continue";
    case ttype::div:
        return "/";
    case ttype::eelse:
        return "else";
    case ttype::eof:
        return "eof";
    case ttype::error:
        return std::string("error:") + std::to_string(_intValue);
    case ttype::ffor:
        return "for";
    case ttype::gt:
        return ">";
    case ttype::iid:
        return std::string("ID(") + _stringValue + std::string(")");
    case ttype::iif:
        return "if";
    case ttype::iint:
        return std::to_string(_intValue);
    case ttype::lcurly:
        return "{";
    case ttype::lt:
        return "<";
    case ttype::lparen:
        return "(";
    case ttype::minus:
        return "-";
    case ttype::mul:
        return "*";
    case ttype::plus:
        return "+";
    case ttype::pprint:
        return "print";
    case ttype::rcurly:
        return "}";
    case ttype::rparen:
        return ")";
    case ttype::semi:
        return ";";
    case ttype::stmtls:
        return "stmtls";
    case ttype::wwhile:
        return "while";
    default:
        return "UNCLASSIFIED";
    }
}
