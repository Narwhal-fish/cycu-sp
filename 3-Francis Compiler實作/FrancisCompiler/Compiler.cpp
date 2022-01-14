#include "Compiler.h"
#include <queue>
#include <sstream>
#include <iomanip>

Compiler::Compiler() {
} // Compiler()

void Compiler::Init(const vector<string> &srcCode) {
  tableManager.Init();
  tableManager.LoadTable(DELIMITER_TABLE);
  tableManager.LoadTable(RESERVED_TABLE);
  lexer.Init(srcCode, tableManager);
  generator.Init(tableManager);
  parser.Init(tableManager, generator);
  errorInfo.clear();
} // Init()

void Compiler::Compile(const vector<string> &srcCode) {
  Init(srcCode);
  queue<TokenData> statementTokens;
  int statementLine;
  int stateScope = UNDEFINED_NUMBER;  // keep to track which scope is
  ErrorInfo errInfo;
  ErrorType errorType;
  while (lexer.GetStatement(statementTokens, statementLine)) {
    if (! parser.Analyze(statementTokens, stateScope, errorType)) {  // syntax error
      errInfo.line = statementLine + 1;
      errInfo.type = errorType;
      this->errorInfo.push_back(errInfo);
    } // if
    else if (errorType == CALL_REFERENCE || errorType == GOTO_REFERENCE) {
      errInfo.line = statementLine + 1;
      errInfo.type = errorType;
      this->errorInfo.push_back(errInfo);
    } // else if

    statementTokens = queue<TokenData>();  // clear queue
  } // while

  if (stateScope != UNDEFINED_NUMBER) {
    errInfo.line = 0;
    errInfo.type = NO_ENDING_ERROR;
    this->errorInfo.push_back(errInfo);
  } // if

  tableManager.CheckCallReference(errorInfo);  // check if call function has defined
  tableManager.FillGotoAddr(errorInfo);  // fill correct position for goto statement (pass 2)
} // Compile()

void Compiler::GetIntermediateStr(vector<string> &strVec, bool humanCode) {
  vector<QuadrupleForm> intermediateCode;
  tableManager.GetQuadrupleTable(intermediateCode);
  stringstream ss;
  string resStr, str;
  QuadrupleForm qf;
  for (int i = 0; i < intermediateCode.size(); i++) {
    qf = intermediateCode.at(i);
    resStr.clear();
    resStr = resStr + "(";
    ss.str("");
    ss.clear();
    ss << "(" << qf.opr.type << "," << qf.opr.entry << ")";
    ss >> str;
    resStr = resStr + str + "\t,";
    str.clear();

    if (qf.op1 != NULL_TOKEN) {
      ss.str("");
      ss.clear();
      ss << "(" << qf.op1.type << "," << qf.op1.entry << ")";
      ss >> str;
    } // if
    resStr = resStr + str + "\t,";
    str.clear();

    if (qf.op2 != NULL_TOKEN) {
      ss.str("");
      ss.clear();
      ss << "(" << qf.op2.type << "," << qf.op2.entry << ")";
      ss >> str;
    } // if
    resStr = resStr + str + "\t,";
    str.clear();

    if (qf.res != NULL_TOKEN) {
      ss.str("");
      ss.clear();
      ss << "(" << qf.res.type << "," << qf.res.entry << ")";
      ss >> str;
    } // if
    resStr = resStr + str + "\t)";
    str.clear();
    if (humanCode) {
      resStr = resStr + "\t" + tableManager.GetHumanCode(qf);
    } // if

    strVec.push_back(resStr);
  } // for
} // GetIntermediateStr()

void Compiler::GetErrorMessageStr(vector<string> &strVec) {
  for (int i = 0; i < errorInfo.size(); i++) {
    stringstream ss;
    string str;
    ss << "Line " << errorInfo.at(i).line;
    str = ss.str();
    str = str + " - error: ";
    switch (errorInfo.at(i).type) {
      case SYNTAX_ERROR:
        str = str + "Syntax error";
        break;
      case ILLEGAL_ID_NAME_ERROR:
        str = str + "Illegal identifier name";
        break;
      case INNER_HEADING_ERROR:
        str = str + "Can't define inner routine";
        break;
      case NO_HEADING_ERROR:
        str = str + "Bad ending statement";
        break;
      case NO_ENDING_ERROR:
        str = str + "Program miss ending";
        break;
      case DUPLICATED_DEFINE_ERROR:
        str = str + "There is a duplicated identifier";
        break;
      case UNDEFINED_IDENTIFIER_ERROR:
        str = str + "The identifier was not defined";
        break;
      case NOT_APPLICABLE_IDENTIFIER_ERROR:
        str = str + "There is a not applicable identifier";
        break;
      case TOO_MANY_POINTS_ERROR:
        str = str + "Too many decimal point";
        break;
      case DIMENSION_ERROR:
        str = str + "Use error dimension in array";
        break;
      case CALL_REFERENCE:
        str = str + "The routine was not defined";
        break;
      case GOTO_REFERENCE:
        str = str + "The label was not defined";
        break;
      default:
        str = str + "Unknown error";
        break;
    } // switch

    strVec.push_back(str);
  } // for
} // GetErrorMessageStr()

void Compiler::DumpTableInfo(vector<string> &tableInfo) {
  vector<string> integerTable, realTable;
  vector<IdentifierEntry> identifierTable;
  vector<int> informationTable;
  tableManager.GetIntegerTable(integerTable);
  tableManager.GetRealTable(realTable);
  tableManager.GetIdentifierTable(identifierTable);
  tableManager.GetInformationTable(informationTable);

  stringstream ss;
  string nStr;
  tableInfo.push_back("Quadruple Table:");
  vector<string> strVec;
  GetIntermediateStr(strVec);
  for (int i = 0; i < strVec.size(); i++) {
    ss.clear();
    ss.str("");
    ss << i + 1;
    nStr.clear();
    ss >> nStr;
    tableInfo.push_back(nStr + "\t" + strVec.at(i));
  } // for

  tableInfo.push_back("");
  tableInfo.push_back("Integer Table:");
  for (int i = 0; i < integerTable.size(); i++) {
    ss.clear();
    ss.str("");
    ss << i + 1;
    nStr.clear();
    ss >> nStr;
    tableInfo.push_back(nStr + "\t" + integerTable.at(i));
  } // for

  tableInfo.push_back("");
  tableInfo.push_back("Real Table:");
  for (int i = 0; i < realTable.size(); i++) {
    ss.clear();
    ss.str("");
    ss << i + 1;
    nStr.clear();
    ss >> nStr;
    tableInfo.push_back(nStr + "\t" + realTable.at(i));
  } // for

  tableInfo.push_back("");
  tableInfo.push_back("Identifier Table:");
  tableInfo.push_back("\tID\tScope\tType\tPointer");
  for (int i = 0; i < identifierTable.size(); i++) {
    if (identifierTable.at(i) != NULL_ID_ENTRY) {
      string id, scopeStr, typeStr, pointerStr;
      id = identifierTable.at(i).id;
      ss.clear();
      ss.str("");
      ss << i;
      nStr.clear();
      ss >> nStr;
      if (identifierTable.at(i).scope != UNDEFINED_NUMBER) {
        ss.clear();
        ss.str("");
        ss << identifierTable.at(i).scope;
        ss >> scopeStr;
      } // if
      else {
        scopeStr = "NULL";
      } // else

      if (identifierTable.at(i).pointer != UNDEFINED_NUMBER) {
        ss.clear();
        ss.str("");
        ss << identifierTable.at(i).pointer;
        ss >> pointerStr;
      } // if
      else {
        pointerStr = "NULL";
      } // else

      if (identifierTable.at(i).type == ARRAY) {
        typeStr = "Array";
      } // if
      else if (identifierTable.at(i).type == BOOLEAN) {
        typeStr = "Boolean";
      } // else if
      else if (identifierTable.at(i).type == CHARACTER) {
        typeStr = "Character";
      } // else if
      else if (identifierTable.at(i).type == INTEGER) {
        typeStr = "Integer";
      } // else if
      else if (identifierTable.at(i).type == LABEL) {
        typeStr = "Label";
      } // else if
      else if (identifierTable.at(i).type == REAL) {
        typeStr = "Real";
      } // else if
      else if (identifierTable.at(i).type == CALLEE) {
        typeStr = "Callee";
      } // else if
      else {
        typeStr = "NULL";
      } // else

      tableInfo.push_back(nStr + "\t" + id + "\t" + scopeStr + "\t" + typeStr + "\t" + pointerStr);
    } // if
  } // for

  tableInfo.push_back("");
  tableInfo.push_back("Information Table:");
  for (int i = 0; i < informationTable.size(); i++) {
    string intStr;
    ss.clear();
    ss.str("");
    ss << i + 1;
    nStr.clear();
    ss >> nStr;
    ss.clear();
    ss.str("");
    ss << informationTable.at(i);
    ss >> intStr;
    tableInfo.push_back(nStr + "\t" + intStr);
  } // for
} // DumpTableInfo()
