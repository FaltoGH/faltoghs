#include <stdio.h>
#include <vector>

#include "token.hpp"

typedef struct _parser {
  int _pos = 0;
  int _callcount=0;
  std::vector<token> _tokens;
  std::vector<std::string> calstk;
  bool debug=false;

  token *assign();
  token *cond();
  token *cpycrt() { return new token(crttok()); }
  token crttok();
  ttype crttyp() { return crttok()._type; }
  void eat(ttype);
  token *expr();
  token *factor();
  token *forStmt();
  token *ifStmt();
  token nxttok();
  ttype nxttyp() { return nxttok()._type; }
  void pop() { calstk.pop_back(); }
  token *printStmt();
  void prtstk() {
    printf("\r\nstack: ");
    for (auto s : calstk) {
      std::cout << s << ' ';
    }
    puts("");
  }
  void push(std::string s) { calstk.push_back(s); }
  token *stmt();
  token *term();
  token *whileStmt();

} parser;

token *parser::assign() {
  _callcount++;
  push("assign");
  token *ret;
  if ((crttyp() == ttype::iid) && (nxttyp() == ttype::assign)) {

    token *a = cpycrt();
    eat(ttype::iid);
    ret = cpycrt();
    eat(ttype::assign);
    ret->addc(a);
    ret->addc(cond());
  } else {
    ret = cond();
  }
  pop();
  return ret;
}

token *parser::cond() {
  _callcount++;
  push("cond");
  token *ret = expr();
  while (crttyp() == ttype::gt || crttyp() == ttype::lt) {
    token *p = cpycrt();
    eat(crttyp());
    p->addc(ret);
    p->addc(expr());
    ret = p;
  }
  pop();
  return ret;
}

token parser::crttok() {
  if (_pos < (int)(_tokens.size())) {
    return _tokens[_pos];
  }
  fprintf(stderr, "error: index out of range");
  exit(0);
}

void parser::eat(ttype expectedTokenType) {
  _callcount++;
  if (crttyp() != expectedTokenType) {
    printf("error: cannot eat. expected token type is %s. actual token is:\r\n",
           typetostr(expectedTokenType));
    crttok().prtdbg();
    prtstk();
    exit(0);
  }
  _pos++;
}

token *parser::expr() {
  _callcount++;
  token *ret = term();
  while (crttyp() == ttype::plus || crttyp() == ttype::minus) {
    token *p = cpycrt();
    eat(crttyp());
    p->addc(ret);
    p->addc(term());
    ret = p;
  }
  return ret;
}

token *parser::factor() {
  _callcount++;
  push("factor");
  token *ret;
  switch (crttyp()) {
  case ttype::iint: {
    ret = cpycrt();
    eat(ttype::iint);
    break;
  }
  case ttype::iid: {
    ret = cpycrt();
    eat(ttype::iid);
    break;
  }
  case ttype::lparen: {
    eat(ttype::lparen);
    ret = assign();
    eat(ttype::rparen);
    break;
  }
  case ttype::plus: {
    eat(ttype::plus);
    ret = factor();
    break;
  }
  case ttype::minus: {
    ret = cpycrt();
    eat(ttype::minus);
    token *zero = new token();
    zero->_type = ttype::iint;
    zero->_intValue = 0;
    ret->addc(zero);
    ret->addc(factor());
    break;
  }
  default:
    // printst();
    fprintf(stderr, "error: %d:%d No factor starts with %d\r\n", crttok()._line,
            crttok()._column, (int)crttyp());
    exit(0);
    break;
  }
  pop();
  return ret;
}

token *parser::forStmt() {
  _callcount++;
  token *ret = cpycrt();
  eat(ttype::ffor);
  eat(ttype::lparen);
  ret->addc(assign());
  eat(ttype::semi);
  ret->addc(assign());
  eat(ttype::semi);
  ret->addc(assign());
  eat(ttype::rparen);
  ret->addc(stmt());
  return ret;
}

token *parser::ifStmt() {
  _callcount++;
  token *ret = cpycrt();
  eat(ttype::iif);
  eat(ttype::lparen);
  ret->addc(assign());
  eat(ttype::rparen);
  ret->addc(stmt());
  if(crttyp()==ttype::eelse){
    eat(ttype::eelse);
    ret->addc(stmt());
  }
  else{
    token* zero=new token();
    zero->_type = ttype::iint;
    zero->_intValue = 0;
    ret->addc(zero);
  }
  return ret;
}

token parser::nxttok() {
  if (_pos + 1 < (int)(_tokens.size())) {
    return _tokens[_pos + 1];
  }

  fprintf(stderr, "error: index out of range");
  exit(0);
}

token *parser::printStmt() {
  _callcount++;
  token *ret = cpycrt();
  eat(ttype::pprint);
  eat(ttype::lparen);
  ret->addc(assign());
  eat(ttype::rparen);
  return ret;
}

token *parser::stmt() {
  _callcount++;
  push("stmt");
  token *ret;
  switch (crttyp()) {
  case ttype::bbreak:
    ret = cpycrt();
    eat(ttype::bbreak);
    eat(ttype::semi);
    break;
  case ttype::ccontinue:
    ret = cpycrt();
    eat(ttype::ccontinue);
    eat(ttype::semi);
    break;
  case ttype::ffor:
    ret = forStmt();
    break;
  case ttype::iif:
    ret = ifStmt();
    break;
  case ttype::pprint:
    ret = printStmt();
    eat(ttype::semi);
    break;
  case ttype::wwhile:
    ret = whileStmt();
    break;
  case ttype::lcurly:
    push("stmt.lcurly");
    ret = cpycrt();
    eat(ttype::lcurly);
    while (crttyp() != ttype::rcurly) {
      ret->addc(stmt());
    }
    eat(ttype::rcurly);
    ret->_type = ttype::stmtls;
    pop();
    break;
  default:
    push("stmt.assign");
    ret = assign();
    eat(ttype::semi);
    pop();
    break;
  }
  if(debug){
    printf("stmt() returned ");
    ret->prtdbg();
  }
  pop();
  return ret;
}

token *parser::term() {
  _callcount++;
  token *ret = factor();
  while (crttyp() == ttype::mul || crttyp() == ttype::div) {
    token *p = cpycrt();
    eat(crttyp());
    p->addc(ret);
    p->addc(factor());
    ret = p;
  }
  return ret;
}

token *parser::whileStmt() {
  _callcount++;
  token *ret = cpycrt();
  eat(ttype::wwhile);
  eat(ttype::lparen);
  ret->addc(assign());
  eat(ttype::rparen);
  ret->addc(stmt());
  return ret;
}
