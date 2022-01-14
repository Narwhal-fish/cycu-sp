#ifndef LEXER_H
#define LEXER_H

#include "Definition.h"
#include "TableManager.h"
#include <string>
#include <vector>
#include <queue>
using namespace std;

class Lexer
{
public:
  Lexer();
  void Init();
  void Init(const vector<string> &srcCode, TableManager &tm);
  bool GetStatement(queue<TokenData> &tokens, int &statementLine);

private:
  char GetNextChar();  // read from source code, return EOF if end of file
  bool IsWhiteSpace(char ch);
  bool IsDelimiter(char ch);
  bool IsBufferEmpty();
  void AppendBuffer(char ch);
  string PopBuffer();
  void ProcessToken(string tokStr, TokenData &tokData);
  void ProcessToken(char tokChar, TokenData &tokData);

  TableManager *tableManager;
  vector<string> srcCode;
  int curColumn;
  int curLine;
  int nextColumn;
  int nextLine;
  string bufferStr;
};

#endif // LEXER_H
