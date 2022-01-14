#ifndef GENERATOR_H
#define GENERATOR_H

#include "Definition.h"
#include "TableManager.h"
#include <string>
#include <vector>
using namespace std;

class Generator
{
public:
  Generator();
  void Init();
  void Init(TableManager &tm);
  bool ProgramHeading(string id);                         // PROGRAM, SUBROUTINE
  void EndOfProgram(bool isEnp);                          // ENP, ENS
  bool VariableDeclare(string id, int scope,              // VARIABLE
                       DataType type);
  bool ArrayDeclare(string id, int scope, DataType type,  // DIMENSION
                    const vector<int> &dim);
  bool LabelDeclare(string id, int scope);                // LABEL
  bool LabelDefine(string id, int scope, int quaPos,      // <label>
                   ErrorType &et);
  bool GotoStatement(string id, int scope,                // GTO
                     ErrorType &et);
  bool CallStatement(string id, int scope,                // CALL
                     const vector<string> &arg);
  bool IoStatement(string IoWord, string id, int scope);  // INPUT, OUTPUT
  bool Assign(string assignId, string expRes, int scope); // assignment
  bool Expression(string opr1, string opr2,               // expression
                  string optor, string res, int scope);
  bool IfStatement(string condStr, int scope,             // IF
                   int ifCondPos, int elsePos,
                   int nextStatementPos);

private:
  bool IsTemporary(string operand, int &tempNum);
  bool IsValidOperand(string operand, int scope, TokenData &tokData);
  bool IsArray(string operand, int scope, TokenData &idTok, TokenData &indexTok);

  TableManager *tableManager;
};

#endif // GENERATOR_H
