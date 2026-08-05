#pragma once
enum struct ttype
{
    assign,
    bbreak,
    ccontinue,
    div,
    eelse,
    eof,
    error,
    ffor,
    gt,
    iid,
    iif,
    iint,
    lcurly,
    lparen,
    lt,
    minus,
    mul,
    plus,
    pprint,
    rcurly,
    rparen,
    semi,
    stmtls,
    wwhile
};
const char* _stype[]
{
    "assign",
    "bbreak",
    "ccontinue",
    "div",
    "eelse",
    "eof",
    "error",
    "ffor",
    "gt",
    "iid",
    "iif",
    "iint",
    "lcurly",
    "lparen",
    "lt",
    "minus",
    "mul",
    "plus",
    "pprint",
    "rcurly",
    "rparen",
    "semi",
    "stmtls",
    "wwhile"
};
const char* typetostr(ttype x){return _stype[(int)x];}

