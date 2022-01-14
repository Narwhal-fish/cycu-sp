#ifndef COMPILER_H
#define COMPILER_H

#include "Definition.h"
#include "TableManager.h"
#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"
#include <vector>
#include <string>
using namespace std;

class Compiler
{
public:
  Compiler();
  void Init(const vector<string> &srcCode);
  void GetIntermediateStr(vector<string> &strVec, bool humanCode = true);
  void GetErrorMessageStr(vector<string> &strVec);
  void DumpTableInfo(vector<string> &tableInfo);
  void Compile(const vector<string> &srcCode);

private:
  TableManager tableManager;
  Lexer lexer;
  Parser parser;
  Generator generator;
  vector<ErrorInfo> errorInfo;
};

#endif // COMPILER_H
