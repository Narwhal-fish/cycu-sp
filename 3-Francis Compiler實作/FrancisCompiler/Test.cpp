#include "Test.h"
#include "Definition.h"
#include "TableManager.h"
#include "Lexer.h"
#include "Compiler.h"
#include <assert.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <iomanip>
using namespace std;

void Test() {
  cout << "================= Test =================" << endl;
  Test_Lexer();
  Test_Compiler();
  cout << "================= Test =================" << endl;
} // Test()

void Test_Lexer() {
  Lexer lexer;
  TableManager tableManager;

  vector<string> testFileName;
  testFileName.push_back("test10.txt");
  // testFileName.push_back("lexical-test2.txt")

  vector<string> srcCode;
  fstream fin;
  int nFile = 0;
  while (nFile < testFileName.size()) {
    cout << testFileName.at(nFile) << ":" << endl;
    fin.open(testFileName.at(nFile).c_str());
    assert(fin.is_open());
    string str;
    getline(fin, str);
    while (! fin.eof()) {
      srcCode.push_back(str);
      getline(fin, str);
    } // while

    if (str.length() > 0) {
      srcCode.push_back(str);
    } // if

    tableManager.Init();
    tableManager.LoadTable(DELIMITER_TABLE);
    tableManager.LoadTable(RESERVED_TABLE);
    lexer.Init(srcCode, tableManager);

    queue<TokenData> statementTokens;
    int statementLine;
    while (lexer.GetStatement(statementTokens, statementLine)) {
      cout << "Line " << setw(4) << statementLine + 1 << ": ";
      while (statementTokens.size() > 0) {
        cout << "(" << statementTokens.front().type << "," << statementTokens.front().entry << ")";
        statementTokens.pop();
      } // while

      cout << endl;
      statementTokens = queue<TokenData>();
    } // while

    srcCode.clear();
    fin.close();
    nFile++;
  } // while
} // Test_Lexer()

void Test_Compiler() {
  Compiler compiler;

  vector<string> testFileName;
  testFileName.push_back("test10.txt");
  // testFileName.push_back("lexical-test2.txt");

  vector<string> srcCode;
  vector<string> codeStr, errStr;
  vector<string> tableInfoStr;
  fstream fin;
  int nFile = 0;
  cout << endl;
  while (nFile < testFileName.size()) {
    cout << testFileName.at(nFile) << ":" << endl;
    fin.open(testFileName.at(nFile).c_str());
    assert(fin.is_open());
    string str;
    getline(fin, str);
    while (! fin.eof()) {
      srcCode.push_back(str);
      getline(fin, str);
    } // while

    if (str.length() > 0) {
      srcCode.push_back(str);
    } // if

    compiler.Compile(srcCode);
    compiler.GetIntermediateStr(codeStr);
    compiler.GetErrorMessageStr(errStr);
/*
    for (int i = 0; i < codeStr.size(); i++) {
      cout << codeStr.at(i) << endl;
    } // for

    for (int i = 0; i < errStr.size(); i++) {
      cout << errStr.at(i) << endl;
    } // for
*/
    cout << endl;
    compiler.DumpTableInfo(tableInfoStr);
    for (int i = 0; i < tableInfoStr.size(); i++) {
      cout << tableInfoStr.at(i) << endl;
    } // for

    cout << endl << "Error Message:" << endl;
    for (int i = 0; i < errStr.size(); i++) {
      cout << errStr.at(i) << endl;
    } // for

    srcCode.clear();
    codeStr.clear();
    errStr.clear();
    tableInfoStr.clear();
    fin.close();
    nFile++;
  } // while
} // Test_Compiler()
