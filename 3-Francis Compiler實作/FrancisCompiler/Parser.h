#ifndef PARSER_H
#define PARSER_H

#include "Definition.h"
#include "TableManager.h"
#include "Generator.h"
#include <queue>
using namespace std;

class Parser
{
public:
  Parser();
  void Init();
  void Init(TableManager &tm, Generator &gen);
  bool Analyze(queue<TokenData> &tokens, int &curScope, ErrorType &errorType);

private:
  bool ProgramHeading(queue<TokenData> &tokens);
  bool ProgramEnding(queue<TokenData> &tokens);
  bool ProgramEndingStatement(queue<TokenData> &tokens);
  bool VariableDeclarationStatement(queue<TokenData> &tokens);
  bool ArrayDeclarationStatement(queue<TokenData> &tokens);
  bool LabelDeclarationStatement(queue<TokenData> &tokens);
  bool NonDeclaredStatement(queue<TokenData> &tokens);

  bool VariableDeclaration(queue<TokenData> &tokens);
  bool ArrayDeclaration(queue<TokenData> &tokens);
  bool SubscriptedVariable(queue<TokenData> &tokens, DataType dataType);
  bool IntegerSequence(queue<TokenData> &tokens, vector<int> &intSeq);

  bool UnlabeledStatement(queue<TokenData> &tokens);
  bool IfStatement(queue<TokenData> &tokens);
  bool Condition(queue<TokenData> &tokens, string &resStr);

  bool StatementI(queue<TokenData> &tokens);
  bool IoStatement(queue<TokenData> &tokens);
  bool GotoStatement(queue<TokenData> &tokens);
  bool CallStatement(queue<TokenData> &tokens);
  bool AssignStatement(queue<TokenData> &tokens);
  bool Expression(queue<TokenData> &tokens, string &resStr);
  bool Term(queue<TokenData> &tokens, string &resStr);
  bool Factor(queue<TokenData> &tokens, string &resStr);
  bool Number(queue<TokenData> &tokens, string &resStr);
  bool Variable(queue<TokenData> &tokens, string &resStr);
  bool AssignVariable(queue<TokenData> &tokens, string &resStr);
  bool ArrayElement(queue<TokenData> &tokens, string &resStr, bool tempResult = true);

  bool Identifier(queue<TokenData> &tokens, string &id);
  bool SpecificReservedWord(queue<TokenData> &tokens, string word);
  bool SpecificReservedWord(queue<TokenData> &tokens, string word, string &getWord);
  bool SpecificDelimiter(queue<TokenData> &tokens, string delimiter);
  bool SpecificDelimiter(queue<TokenData> &tokens, string delimiter, string &getDelimiter);
  string GetTemporaryString();
  int StringToInt(string str);

  TableManager *tableManager;
  Generator *generator;
  ErrorType curErrorType;
  int curScope;
  int tempNum;
};

#endif // PARSER_H
