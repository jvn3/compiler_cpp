/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>

#include <map>

#include <string>

#include "parse.h"

#include "val.h"

using std::vector;
using std::map;
using std::string;

// NodeType represents all possible types
enum NodeType {
  ERRTYPE,
  INTTYPE,
  STRTYPE
};

// a "forward declaration" for a class to hold values
class Val;
extern map < string, Val > symbol;
extern void RunTimeError(int, string);
static map < string, bool > declared_ID;

class ParseTree {

  public:
          int linenum;
          ParseTree * lt;
          ParseTree * rt;

  ParseTree(int linenum, ParseTree * l = 0, ParseTree * r = 0): linenum(linenum), lt(l), rt(r) {}

  virtual~ParseTree() {
    delete lt;
    delete rt;
  }

  int GetLineNumber() const {
    return linenum;
  }

  int traverse(int input, int(ParseTree:: * func)(void)) {
    if (lt) input += lt -> traverse(input, func);
    if (rt) input += rt -> traverse(input, func);
    return Visit(input, func);
  }

  virtual int Visit(int input, int(ParseTree:: * func)(void)) {
    return input;
  }

  virtual bool IsIdent() const {
    return false;
  }
  virtual bool IsVar() const {
    return false;
  }
  virtual string GetId() const {
    return "";
  }
  virtual int IsBang() const {
    return 0;
  }
  virtual bool IsLet() const {
    return false;
  }
  virtual Val EVAL(map < string, Val > & symbol) = 0;

};

class StmtList: public ParseTree {
  public: StmtList(ParseTree * l, ParseTree * r): ParseTree(0, l, r) {}
  virtual Val EVAL(map < string, Val > & symbol) {
    lt -> EVAL(symbol);
    if (rt) {
      rt -> EVAL(symbol);
    }
    return Val();
  }

};
class Let: public ParseTree {
  string id;
  public:
    Let(Lex & t, ParseTree * e): ParseTree(t.GetLinenum(), e), id(t.GetLexeme()) {
      declared_ID.insert(std::pair < string, bool > (id, IsLet()));
    }
  string GetId() const {
    return id;
  }
  bool IsLet() const {
    return true;
  }
    virtual Val EVAL(map < string, Val > & symbol) {
    Val result = lt -> EVAL(symbol);

    if (symbol.count(id) && !symbol.empty()) {
      symbol[id] = result;
    } else {
      symbol.insert(std::pair < string, Val > (id, result));
    }
    return Val();
  }
};

class Print: public ParseTree {
  public: Print(int line, ParseTree * e): ParseTree(line, e) {}
  virtual Val EVAL(map < string, Val > & symbol) {
  
  cout << lt -> EVAL(symbol);
  return Val();
  }
};

class Loop: public ParseTree {

  public: Loop(int line, ParseTree * e, ParseTree * s): ParseTree(line, e, s) {}
  virtual Val EVAL(map < string, Val > & symbol) {
    Val results = lt -> EVAL(symbol);

    if (results.isInt() && results.ValInt() != 0) {
      for (int i = results.ValInt(); i != 0; i = lt -> EVAL(symbol).ValInt()) {
        lt -> EVAL(symbol);
        rt -> EVAL(symbol);
      }

    }

    return Val();
  }
};

class If: public ParseTree {

  public: If(int line, ParseTree * e, ParseTree * s): ParseTree(line, e, s) {}

  virtual Val EVAL(map < string, Val > & symbol) {
    Val results = lt -> EVAL(symbol);
    if (results.isInt() && results.ValInt() != 0) {
      return rt -> EVAL(symbol);
    } else if (!results.isInt()) {
      RunTimeError(linenum, "Expression is not an integer");
    }
    return Val();
  }
};

class PlusExpr: public ParseTree {
  public: PlusExpr(int line, ParseTree * l, ParseTree * r): ParseTree(line, l, r) {}
  Val EVAL(map < string, Val > & symbol) {
    auto L = lt -> EVAL(symbol);
    if (L.isErr())
      RunTimeError(linenum, L.GetErrMsg());
    auto R = rt -> EVAL(symbol);
    if (R.isErr())
      RunTimeError(linenum, R.GetErrMsg());
    auto answer = L + R;
    if (answer.isErr())
      RunTimeError(linenum, answer.GetErrMsg());
    return answer;
  }
};

class MinusExpr: public ParseTree {
  public: MinusExpr(int line, ParseTree * l, ParseTree * r): ParseTree(line, l, r) {}
  Val EVAL(map < string, Val > & symbol) {
    auto L = lt -> EVAL(symbol);
    if (L.isErr())
      RunTimeError(linenum, L.GetErrMsg());
    auto R = rt -> EVAL(symbol);
    if (R.isErr())
      RunTimeError(linenum, R.GetErrMsg());
    auto answer = L - R;
    if (answer.isErr())
      RunTimeError(linenum, answer.GetErrMsg());
    return answer;
  }

};

class TimesExpr: public ParseTree {
  public: TimesExpr(int line, ParseTree * l, ParseTree * r): ParseTree(line, l, r) {}
  Val EVAL(map < string, Val > & symbol) {
    auto L = lt -> EVAL(symbol);
    if (L.isErr())
      RunTimeError(linenum, L.GetErrMsg());
    auto R = rt -> EVAL(symbol);
    if (R.isErr())
      RunTimeError(linenum, R.GetErrMsg());
    auto answer = L * R;
    if (answer.isErr())
      RunTimeError(linenum, answer.GetErrMsg());
    return answer;
  }
};

class DivideExpr: public ParseTree {
  public: DivideExpr(int line, ParseTree * l, ParseTree * r): ParseTree(line, l, r) {}

  Val EVAL(map < string, Val > & symbol) {
    auto L = lt -> EVAL(symbol);
    if (L.isErr())
      RunTimeError(linenum, L.GetErrMsg());
    auto R = rt -> EVAL(symbol);
    if (R.isErr())
      RunTimeError(linenum, R.GetErrMsg());
    auto answer = L / R;
    if (answer.isErr())
      RunTimeError(linenum, answer.GetErrMsg());
    return answer;
  }
};

class BangExpr: public ParseTree {
  public: BangExpr(int line, ParseTree * l): ParseTree(line, l) {}
  virtual int IsBang() const {
    return 1;
  }
  Val EVAL(map < string, Val > & symbol) {
    auto L = lt -> EVAL(symbol);
    if (L.isErr())
      RunTimeError(linenum, L.GetErrMsg());
    auto answer = !L;
    if (answer.isErr())
      RunTimeError(linenum, answer.GetErrMsg());
    return answer;
  }
};

class IConst: public ParseTree {
  int val;
  public:
    IConst(Lex & t): ParseTree(t.GetLinenum()) {
      val = stoi(t.GetLexeme());
    }
  virtual Val EVAL(map < string, Val > & symbol) {
    return Val(val);
  }
};

class SConst: public ParseTree {
  string val;
  public:
    SConst(Lex & t): ParseTree(t.GetLinenum()) {
      val = t.GetLexeme();
    }
  virtual Val EVAL(map < string, Val > & symbol) {
    return Val(val);
  }
};

class Ident: public ParseTree {
  string id;
  public:
    Ident(Lex & t): ParseTree(t.GetLinenum()), id(t.GetLexeme()) {
      CheckLetBeforeUse(declared_ID);
    }
  bool is_Ident() const {
    return true;
  }
  string get_ID() const {
    return id;
  }

  virtual void CheckLetBeforeUse(map < string, bool > & declared_ID) {
    if (declared_ID.end() == declared_ID.find(id)) {
      cout << "UNDECLARED VARIABLE " << id << endl;
      exit(1);
    }
  }
  virtual Val EVAL(map < string, Val > & symbol) {
    return symbol[id];
  }
};

#endif /* PARSETREE_H_ */