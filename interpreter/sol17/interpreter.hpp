typedef struct _interpreter {
  bool breakRequired = false;
  bool continueRequired = false;
  bool debug=false;
  std::map<std::string, int> globalScope;

  int visit(token *t) {
    if(t == NULL){
      printf("cannot visit null\r\n");
      exit(0);
    }
    if(debug){
      //printf("visit ");
      //t->prtdbg();
    }
    
    int ret=0;
    
    switch (t->_type) {
    case ttype::assign: {
      int rvalue = visit(t->c1());
      globalScope[t->c0()->_stringValue] = rvalue;
      ret=rvalue;
      if(debug){
        std::cout<< "debug: " << t->c0()->_stringValue << '=' << rvalue << ";\r\n";
      }
      break;
    }

    case ttype::bbreak: {
      breakRequired = true;
      break;
    }

    case ttype::ccontinue: {
      continueRequired = true;
      break;
    }

    case ttype::div:
    {
      int denom=visit(t->c1());
      if(denom==0){
        printf("error: division by zero\r\n");
        exit(0);
      }
      ret= visit(t->c0()) / denom;
      break;
    }

    case ttype::ffor: {
      visit(t->c0());
      bool conditionMet = visit(t->c1());
      while (conditionMet) {
        visit(t->c3());
        if (breakRequired) {
          breakRequired = false;
          break;
        }

        if (continueRequired) {
          continueRequired = false;
        }

        visit(t->c2());
        conditionMet = visit(t->c1());
      }
      break;
    }

    case ttype::gt:
      ret = visit(t->c0()) > visit(t->c1());break;

    case ttype::iif: {
      if (visit(t->c0())) {
        visit(t->c1());
      }
      else{
        visit(t->c2());
      }
      break;
    }
    
    case ttype::iint:
      ret= t->_intValue;
      break;

    case ttype::lt:
      ret = visit(t->c0()) < visit(t->c1());break;

    case ttype::minus:
      ret = visit(t->c0()) - visit(t->c1());break;

    case ttype::mul:
      ret = visit(t->c0()) * visit(t->c1());break;

    case ttype::plus:
      ret = visit(t->c0()) + visit(t->c1());break;;

    case ttype::pprint: {
      int value = visit(t->c0());
      printf("%d\r\n", value);
      break;
    }

    case ttype::stmtls:
    {
      int count=0;
      for (const auto c : t->children) {
        visit(c);
        count++;
        if (breakRequired || continueRequired) {
          break;
        }
      }
      if(debug){
        printf("visited %d/%d statement in stmtls\r\n", count, t->nc());
      }
      break;
    }

    case ttype::iid:
      ret=globalScope[t->_stringValue];
      break;

    case ttype::wwhile: {
      int count = 0;
      while (visit(t->c0())) {
        visit(t->c1());
        if (breakRequired) {
          breakRequired = false;
          break;
        }

        if (continueRequired) {
          continueRequired = false;
        }

        count++;
        if (count > 100) {
          fprintf(stderr, "error: too much while loop\r\n");
          exit(0);
        }
      }
      break;
    }

    default:
      fprintf(stderr, "error: cannot visit\r\n");
      exit(0);
      break;
    }

    return ret;
  }
  
} interpreter;
