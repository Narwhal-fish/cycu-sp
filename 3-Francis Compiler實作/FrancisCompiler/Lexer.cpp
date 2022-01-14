#include "Lexer.h"

Lexer::Lexer() {
  Init();
} // Lexer()

void Lexer::Init() {
  srcCode.clear();
  tableManager = nullptr;
  curLine = UNDEFINED_NUMBER;
  curColumn = UNDEFINED_NUMBER;
  nextColumn = UNDEFINED_NUMBER;
  nextLine = UNDEFINED_NUMBER;
} // Init()

void Lexer::Init(const vector<string> &srcCode, TableManager &tm) {
  this->srcCode = srcCode;
  this->tableManager = &tm;
  curLine = UNDEFINED_NUMBER;
  curColumn = UNDEFINED_NUMBER;
  nextColumn = 0;
  nextLine = 0;
  for (unsigned int i = 0; i < this->srcCode.size(); i++) {
    this->srcCode.at(i) = this->srcCode.at(i) + "\n";
  } // for
} // Init()

bool Lexer::GetStatement(queue<TokenData> &tokens, int &statementLine) {
  tokens = queue<TokenData>();      // initial token queue
  statementLine = UNDEFINED_NUMBER; // initial line
  TokenData newTok;
  string tokStr;
  char curChar;
  curChar = GetNextChar();
  while (curChar != EOF) {
    if (IsWhiteSpace(curChar)) {  // process 0 or 1 token
      if (! IsBufferEmpty()) { // 1 token case
        tokStr = PopBuffer();
        ProcessToken(tokStr, newTok);
        tokens.push(newTok);
      } // if
    } // if
    else if (IsDelimiter(curChar)) {  // process 1 or 2 tokens
      if (statementLine == UNDEFINED_NUMBER) { // set first valid char line
        statementLine = curLine;
      } // if

      if (! IsBufferEmpty()) {  // process first of two tokens
        tokStr = PopBuffer();
        ProcessToken(tokStr, newTok);
        tokens.push(newTok);
      } // else if

      ProcessToken(curChar, newTok);
      tokens.push(newTok);
      if (newTok == COLON_TOKEN) {
        return true;
      } // if
    } // else if
    else {
      if (statementLine == UNDEFINED_NUMBER) {  // set first valid char line
        statementLine = curLine;
      } // if

      AppendBuffer(curChar);
    } // else

    curChar = GetNextChar();
  } // while

  if (tokens.empty()) {
    return false;
  } // if

  return true;
} // GetStatement()

char Lexer::GetNextChar() {
  if (srcCode.empty() || nextColumn == UNDEFINED_NUMBER || nextLine == UNDEFINED_NUMBER) {
    return EOF;
  } // if

  string str;
  curLine = nextLine;
  curColumn = nextColumn;
  str = srcCode.at(curLine);
  char ch = str[curColumn];
  if (nextColumn + 1 < str.length()) {
    nextColumn = nextColumn + 1;
  } // if
  else {
    if (nextLine + 1 < srcCode.size()) {
      nextLine = nextLine + 1;
      nextColumn = 0;
    } // if

    if (nextLine == curLine && nextColumn == curColumn) {
      nextLine = UNDEFINED_NUMBER;
      nextColumn = UNDEFINED_NUMBER;
    } // if
  } // else

  return ch;
} // GetNextChar()

bool Lexer::IsWhiteSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n';
} // IsWhiteSpace()

bool Lexer::IsDelimiter(char ch) {
  string key;
  key = key + ch;
  return tableManager->Find(DELIMITER_TABLE, key) != UNDEFINED_NUMBER;
} // IsDelimiter()

bool Lexer::IsBufferEmpty() {
  return bufferStr.empty();
} // IsBufferEmpty()

void Lexer::AppendBuffer(char ch) {
  bufferStr = bufferStr + ch;
} // AppendBuffer()

string Lexer::PopBuffer() {
  string str = bufferStr;
  bufferStr.clear();
  return str;
} // PopBuffer()

void Lexer::ProcessToken(string tokStr, TokenData &tokData) {
  int pos;
  pos = tableManager->Find(DELIMITER_TABLE, tokStr);
  if (pos != UNDEFINED_NUMBER) {
    tokData.type = DELIMITER_TABLE;
    tokData.entry = pos;
    return;
  } // if

  pos = tableManager->Find(RESERVED_TABLE, tokStr);
  if (pos != UNDEFINED_NUMBER) {
    tokData.type = RESERVED_TABLE;
    tokData.entry = pos;
    return;
  } // if

  bool isNumber = true;
  bool hasPoint = false;
  for (unsigned int i = 0; i < tokStr.length(); i++) {
    if (!(tokStr[i] >= '0' && tokStr[i] <= '9') && tokStr[i] != '.') {
      isNumber = false;
    } // if
    else if (tokStr[i] == '.') {
      hasPoint = true;
    } // else if
  } // for

  if (isNumber) {
    if (hasPoint) {
      tokData.type = REAL_TABLE;
      tokData.entry = tableManager->Insert(REAL_TABLE, tokStr);
    } // if
    else {
      tokData.type = INTEGER_TABLE;
      tokData.entry = tableManager->Insert(INTEGER_TABLE, tokStr);
    } // else
  } // if
  else {
    IdentifierEntry idEntry = NULL_ID_ENTRY;
    idEntry.id = tokStr;
    tokData.type = IDENTIFIER_TABLE;
    tokData.entry = tableManager->Insert(IDENTIFIER_TABLE, idEntry);
  } // else
} // ProcessToken()

void Lexer::ProcessToken(char tokChar, TokenData &tokData) {
  string tokStr;
  tokStr = tokStr + tokChar;
  ProcessToken(tokStr, tokData);
} // ProcessToken()
