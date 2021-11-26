/*
 * project name: SIC Lexical Analysis
 *
 * id:     10827216
 * author: ¤ý¥ßµ¾
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "TokenTable.h"
using namespace std;

#define HASH_TABLE_NUM 3
#define HASH_TABLE_SIZE 100
#define SYMBOL_TABLE_INDEX 0
#define INTEGER_TABLE_INDEX 1
#define STRING_TABLE_INDEX 2

Table *table[4]; // instruction, pseudo, register, delimiter, type 1 ~ 4
HashTable *hashTable[HASH_TABLE_NUM]; // symbol, integer, string, type 5 ~ 7

struct Token {
  int type;
  int pos;
};

struct SourceLine {
  string srcLine;
  vector<Token> tokens;
};

void BuildTable();
void DestroyTable();
void WriteResult(string outputFileName, vector<SourceLine> &resVec);

Token ProcessToken(string tokenStr);
Token ProcessLiteral(string literalStr, bool isString);
bool ReadAndAnalyzeSource(string sourceName, vector<SourceLine> &anlsRes);
char ReadNextChar(ifstream &fin);
char ReadComment(ifstream &fin, string &commentStr);
char ReadLiteral(ifstream &fin, string &literalStr);
bool IsWhiteSpace(char ch);
bool IsLiteralPrefix(string bufferStr);
bool IsIntegerString(string bufferStr);
string CharToString(char ch);

int main() {
  BuildTable();
  vector<SourceLine> resVec;
  string fileName;
  cout << "Input a source name: ";
  cin >> fileName;
  while (! ReadAndAnalyzeSource(fileName, resVec)) {
    cout << "File name doesn't exist!" << endl;
    cout << "Input a source name: ";
    cin >> fileName;
  } // while

  string outputFileName;
  cout << "Analysis completely!" << endl;
  cout << "Input output file name: ";
  cin >> outputFileName;
  WriteResult(outputFileName, resVec);
  DestroyTable();
} // main()

void BuildTable() {
  vector<TokenStr> vt[4];
  vt[0] = Table::ReadTableFile("Table1.table");
  vt[1] = Table::ReadTableFile("Table2.table");
  vt[2] = Table::ReadTableFile("Table3.table");
  vt[3] = Table::ReadTableFile("Table4.table");

  for (int i = 0; i < 4; i++) {
    table[i] = new Table(vt[i]);
  } // for

  for (int i = 0; i < HASH_TABLE_NUM; i++) {
    hashTable[i] = new HashTable(HASH_TABLE_SIZE);
  } // for
} // BuildTable()

void DestroyTable() {
  for (int i = 0; i < 4; i++) {
    delete table[i];
  } // for

  for (int i = 0; i < HASH_TABLE_NUM; i++) {
    delete hashTable[i];
  } // for
} // DestroyTable()

void WriteResult(string outputFileName, vector<SourceLine> &resVec) {
  ofstream fout;
  fout.open(outputFileName.c_str());

  for (int i = 0; i < resVec.size(); i++) {
    fout << resVec.at(i).srcLine << endl;
    vector<Token> &tokenVec = resVec.at(i).tokens;
    for (int ti = 0; ti < tokenVec.size(); ti++) {
      fout << "(" << tokenVec.at(ti).type << "," << tokenVec.at(ti).pos << ")";
    } // for

    fout << endl;
  } // for

  fout.close();
} // WriteResult()

Token ProcessToken(string tokenStr) {
  Token token = {-1, -1};
  for (int i = 0; i < 4; i++) {
    int pos = table[i]->Find(tokenStr);
    if (pos != NOT_FOUND) {
      token.type = i + 1;
      token.pos = pos;
    } // if
  } // for

  if (token.type == -1) { // not type 1 ~ 4
    if (IsIntegerString(tokenStr)) { // integer, type 6
      int pos = hashTable[INTEGER_TABLE_INDEX]->Find(tokenStr);
      if (pos == NOT_FOUND) {
        pos = hashTable[INTEGER_TABLE_INDEX]->Insert(tokenStr);
      } // if

      token.type = 5 + INTEGER_TABLE_INDEX;
      token.pos = pos;
    } // if
    else { // symbol, type 5
      int pos = hashTable[SYMBOL_TABLE_INDEX]->Find(tokenStr);
      if (pos == NOT_FOUND) {
        pos = hashTable[SYMBOL_TABLE_INDEX]->Insert(tokenStr);
      } // if

      token.type = 5 + SYMBOL_TABLE_INDEX;
      token.pos = pos;
    } // else
  } // if

  return token;
} // ProcessToken()

Token ProcessLiteral(string literalStr, bool isString) {
  Token token;
  if (isString) { // string
    int pos = hashTable[STRING_TABLE_INDEX]->Find(literalStr);
    if (pos == NOT_FOUND) {
      pos = hashTable[STRING_TABLE_INDEX]->Insert(literalStr);
    } // if

    token.type = 5 + STRING_TABLE_INDEX;
    token.pos = pos;
  } // if
  else { // hex integer (upper case)
    // convert to upper case
    for (int i = 0; i < literalStr.length(); i++) {
      if (islower(literalStr[i]) != 0) {
        literalStr[i] = toupper(literalStr[i]);
      } // if
    } // for

    int pos = hashTable[INTEGER_TABLE_INDEX]->Find(literalStr);
    if (pos == NOT_FOUND) {
      pos = hashTable[INTEGER_TABLE_INDEX]->Insert(literalStr);
    } // if

    token.type = 5 + INTEGER_TABLE_INDEX;
    token.pos = pos;
  } // else

  return token;
} // ProcessLiteral()

bool ReadAndAnalyzeSource(string sourceName, vector<SourceLine> &anlsRes) {
  ifstream fin;
  fin.open(sourceName.c_str());
  if (! fin.is_open()) {
    fin.open((sourceName + ".txt").c_str());
    if (! fin.is_open()) {
      return false;
    } // if
  } // if

  SourceLine srcLine;
  string srcLineStr;
  vector<Token> lineTokens;

  string bufferStr;
  bool finish = false;
  char ch = ReadNextChar(fin);
  while (! finish) {
    if (ch != '\n' && ch != EOF) {
      srcLineStr = srcLineStr + ch;
    } // if

    if (IsWhiteSpace(ch) || ch == EOF) { // current char is white-space or EOF
      if (bufferStr.length() > 0) {
        Token newToken;
        newToken = ProcessToken(bufferStr);
        lineTokens.push_back(newToken);
        bufferStr.clear();
      } // if

      if (ch == '\n' || ch == EOF) { // end of line or end of file
        if (lineTokens.size() > 0) {
          srcLine.srcLine = srcLineStr;
          srcLine.tokens = lineTokens;
          anlsRes.push_back(srcLine);
          srcLineStr.clear();
          lineTokens.clear();
        } // if

        if (ch == EOF) {
          finish = true;
        } // if
      } // if
    } // if
    else if (table[3]->Find(CharToString(ch)) != NOT_FOUND) { // current char is delimiter char
      Token newToken;
      if (ch == '\'' && IsLiteralPrefix(bufferStr)) { // process literal token
        newToken = ProcessToken(CharToString(ch));
        lineTokens.push_back(newToken);

        string literalStr;
        ch = ReadLiteral(fin, literalStr);
        srcLineStr = srcLineStr + literalStr + ch;
        if (bufferStr.compare("C") == 0 || bufferStr.compare("c") == 0) {
          newToken = ProcessLiteral(literalStr, true);
        } // if
        else { // hex integer
          newToken = ProcessLiteral(literalStr, false);
        } // else

        lineTokens.push_back(newToken);
      } // if
      else if (bufferStr.length() > 0) { // process first of two tokens
        newToken = ProcessToken(bufferStr);
        lineTokens.push_back(newToken);
      } // if

      bufferStr.clear();
      newToken = ProcessToken(CharToString(ch));
      lineTokens.push_back(newToken);
    } // else if
    else {
      bufferStr = bufferStr + ch;
    } // else

    if (ch == '.') {
      string commentStr;
      ch = ReadComment(fin, commentStr);
      srcLineStr = srcLineStr + commentStr;
    } // if
    else if (! finish) {  // still not finish
      ch = ReadNextChar(fin);
    } // else
  } // while

  fin.close();
  return true;
} // ReadAndAnalyzeSource()

char ReadNextChar(ifstream &fin) {
  char ch = fin.get();
  return ch;
} // ReadNextChar()

char ReadComment(ifstream &fin, string &commentStr) {
  char ch = fin.get();
  while (ch != '\n' && ch != EOF) {
    commentStr = commentStr + ch;
    ch = fin.get();
  } // while

  return ch;
} // ReadComment()

char ReadLiteral(ifstream &fin, string &literalStr) {
  char ch = fin.get();
  while (ch != '\'' && ch != EOF) {
    literalStr = literalStr + ch;
    ch = fin.get();
  } // while

  return ch;
} // ReadLiteral()

bool IsWhiteSpace(char ch) {
  return ch == ' ' || ch == '\n' || ch == '\t';
} // IsWhiteSpace()

bool IsLiteralPrefix(string bufferStr) {
  if (bufferStr.compare("C") == 0 || bufferStr.compare("c") == 0 ||
      bufferStr.compare("X") == 0 || bufferStr.compare("x") == 0) {
    return true;
  } // if

  return false;
} // IsLiteralPrefix()

bool IsIntegerString(string bufferStr) {
  for (int i = 0; i < bufferStr.length(); i++) {
    if (! (bufferStr[i] >= '0' && bufferStr[i] <= '9')) {
      return false;
    } // if
  } // for

  return true;
} // IsIntegerString()

string CharToString(char ch) {
  string s;
  s = s + ch;
  return s;
} // CharToString()
