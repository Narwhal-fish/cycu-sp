#include "Parser.h"
#include <sstream>
#include <iostream>
#include <assert.h>

Parser::Parser() {
  Init();
} // Parser()

void Parser::Init() {
  tableManager = nullptr;
  generator = nullptr;
} // Init()

void Parser::Init(TableManager &tm, Generator &gen) {
  this->tableManager = &tm;
  this->generator = &gen;
} // Init()

bool Parser::Analyze(queue<TokenData> &tokens, int &scope, ErrorType &errorType) {
  curErrorType = UNDEFINED_ERROR;
  curScope = scope;
  tempNum = 1;
  bool isCorrectSyntax;
  if (ProgramHeading(tokens) || ProgramEnding(tokens) ||
      VariableDeclarationStatement(tokens) || ArrayDeclarationStatement(tokens) ||
      LabelDeclarationStatement(tokens) || NonDeclaredStatement(tokens)) {
    scope = curScope;
    isCorrectSyntax = true;
  } // if
  else {
    if (curErrorType == UNDEFINED_ERROR) {
      curErrorType = SYNTAX_ERROR;
    } // if

    isCorrectSyntax = false;
  } // else

  errorType = curErrorType;
  tableManager->ClearInvalidIdentifier();  // clear invalid identifier entry
  return isCorrectSyntax;
} // Analyze()

bool Parser::ProgramHeading(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string id;
  if (SpecificReservedWord(temp, "PROGRAM") && Identifier(temp, id) && SpecificDelimiter(temp, ";")) {
    if (curScope == UNDEFINED_NUMBER) {
      curScope = tableManager->Find(IDENTIFIER_TABLE, id, UNDEFINED_NUMBER);
      if (! generator->ProgramHeading(id)) {
        curErrorType = DUPLICATED_DEFINE_ERROR;
        return false;
      } // if

      tokens = temp;
      return true;
    } // if
    else {
      curErrorType = INNER_HEADING_ERROR;
      return false;
    } // else
  } // if
  else if (SpecificReservedWord(temp, "SUBROUTINE") && Identifier(temp, id) && SpecificDelimiter(temp, "(")) {
    if (curScope == UNDEFINED_NUMBER) {
      curScope = tableManager->Find(IDENTIFIER_TABLE, id, UNDEFINED_NUMBER);
      if (! generator->ProgramHeading(id)) {
        curErrorType = DUPLICATED_DEFINE_ERROR;
        return false;
      } // if

      if (VariableDeclaration(temp)) {
        queue<TokenData> tempTemp = temp;
        while (SpecificDelimiter(temp, ",") && VariableDeclaration(temp)) {
          tempTemp = temp;
        } // while

        temp = tempTemp;  // recover queue
        if (SpecificDelimiter(temp, ")") && SpecificDelimiter(temp, ";")) {
          tokens = temp;
          return true;
        } // if
      } // if
    } // if
    else {
      curErrorType = INNER_HEADING_ERROR;
      return false;
    } // else
  } // else if

  return false;
} // ProgramHeading()

bool Parser::ProgramEnding(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string endStr;
  if (ProgramEndingStatement(temp) && SpecificDelimiter(temp, ";")) {
    tokens = temp;
    return true;
  } // if

  return false;
} // ProgramEnding()

bool Parser::ProgramEndingStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string endStr;
  if (SpecificReservedWord(temp, "ENP", endStr) || SpecificReservedWord(temp, "ENS", endStr)) {
    if (curScope != UNDEFINED_NUMBER) {
      tokens = temp;
      generator->EndOfProgram(endStr.compare("ENP") == 0);
      curScope = UNDEFINED_NUMBER;
      return true;
    } // if
    else {
      curErrorType = NO_HEADING_ERROR;
      return false;
    } // else
  } // if

  return false;
} // ProgramEndingStatement()

bool Parser::VariableDeclarationStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  if (SpecificReservedWord(temp, "VARIABLE") && VariableDeclaration(temp) &&
      SpecificDelimiter(temp, ";")) {
    tokens = temp;
    return true;
  } // if

  return false;
} // VariableDeclarationStatement()

bool Parser::VariableDeclaration(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string typeStr, idStr;
  if (! SpecificReservedWord(temp, "INTEGER", typeStr) &&
      ! SpecificReservedWord(temp, "REAL", typeStr) &&
      ! SpecificReservedWord(temp, "BOOLEAN", typeStr)) {
    return false;
  } // if
  else if (SpecificDelimiter(temp, ":") && Identifier(temp, idStr)) {
    tokens = temp;
    DataType dType;
    if (typeStr.compare("INTEGER") == 0) {
      dType = INTEGER;
    } // if
    else if (typeStr.compare("REAL") == 0) {
      dType = REAL;
    } // else if
    else {
      dType = BOOLEAN;
    } // else

    if (! generator->VariableDeclare(idStr, curScope, dType)) {
      curErrorType = DUPLICATED_DEFINE_ERROR;
      return false;
    } // if

    while (SpecificDelimiter(temp, ",") && Identifier(temp, idStr)) {
      tokens = temp;
      if (! generator->VariableDeclare(idStr, curScope, dType)) {
        curErrorType = DUPLICATED_DEFINE_ERROR;
        return false;
      } // if
    } // while

    return true;
  } // if

  return false;
} // VariableDeclaration()

bool Parser::ArrayDeclarationStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  if (SpecificReservedWord(temp, "DIMENSION") && ArrayDeclaration(temp) &&
      SpecificDelimiter(temp, ";")) {
    tokens = temp;
    return true;
  } // if

  return false;
} // ArrayDeclarationStatement()

bool Parser::ArrayDeclaration(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string typeStr;
  if (! SpecificReservedWord(temp, "INTEGER", typeStr) &&
      ! SpecificReservedWord(temp, "REAL", typeStr) &&
      ! SpecificReservedWord(temp, "BOOLEAN", typeStr)) {
    return false;
  } // if
  else if (SpecificDelimiter(temp, ":")) {
    DataType dType;
    if (typeStr.compare("INTEGER") == 0) {
      dType = INTEGER;
    } // if
    else if (typeStr.compare("REAL") == 0) {
      dType = REAL;
    } // else if
    else if (typeStr.compare("BOOLEAN") == 0) {
      dType = BOOLEAN;
    } // else

    if (SubscriptedVariable(temp, dType)) {
      tokens = temp;
      while (SpecificDelimiter(temp, ",") && SubscriptedVariable(temp, dType)) {
        tokens = temp;
      } // while

      return true;
    } // if
  } // if

  return false;
} // ArrayDeclaration()

bool Parser::SubscriptedVariable(queue<TokenData> &tokens, DataType dataType) {
  queue<TokenData> temp = tokens;
  string idStr;
  vector<int> dim;
  if (Identifier(temp, idStr) && SpecificDelimiter(temp, "(") &&
      IntegerSequence(temp, dim) && SpecificDelimiter(temp, ")")) {
    tokens = temp;
    if (! generator->ArrayDeclare(idStr, curScope, dataType, dim)) {
      curErrorType = DUPLICATED_DEFINE_ERROR;
      return false;
    } // if

    return true;
  } // if

  return false;
} // SubscriptedVariable()

bool Parser::IntegerSequence(queue<TokenData> &tokens, vector<int> &intSeq) {
  queue<TokenData> temp = tokens;
  string nStr;
  if (temp.front().type == INTEGER_TABLE) {
    nStr = tableManager->GetKeyContent(INTEGER_TABLE, temp.front().entry);
    intSeq.push_back(StringToInt(nStr));
    temp.pop();
    tokens = temp;
    while (SpecificDelimiter(temp, ",") && temp.front().type == INTEGER_TABLE) {
      nStr = tableManager->GetKeyContent(INTEGER_TABLE, temp.front().entry);
      intSeq.push_back(StringToInt(nStr));
      temp.pop();
      tokens = temp;
    } // while

    return true;
  } // if

  return false;
} // IntegerSequence()

bool Parser::LabelDeclarationStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string idStr;
  if (SpecificReservedWord(temp, "LABEL") && Identifier(temp, idStr)) {
    if (! generator->LabelDeclare(idStr, curScope)) {
      curErrorType = DUPLICATED_DEFINE_ERROR;
      return false;
    } // if

    queue<TokenData> tempTemp = temp;
    while (SpecificDelimiter(temp, ",") && Identifier(temp, idStr)) {
      tempTemp = temp;
      if (! generator->LabelDeclare(idStr, curScope)) {
        curErrorType = DUPLICATED_DEFINE_ERROR;
        return false;
      } // if
    } // while

    temp = tempTemp;  // recover queue
    if (SpecificDelimiter(temp, ";")) {
      tokens = temp;
      return true;
    } // if
  } // if

  return false;
} // LabelDeclarationStatement()

bool Parser::NonDeclaredStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  if (UnlabeledStatement(temp) && SpecificDelimiter(temp, ";")) {  // unlabeled statement
    tokens = temp;
    return true;
  } // if

  string labelStr;
  int labelScope = curScope;
  int labelPos = tableManager->GetTableSize(QUADRUPLE_TABLE) + 1;
  temp = tokens;
  if (Identifier(temp, labelStr) && UnlabeledStatement(temp) && SpecificDelimiter(temp, ";")) {  // labeled statement
    if (! generator->LabelDefine(labelStr, labelScope, labelPos, curErrorType)) {  // label define failed
      return false;
    } // if

    tokens = temp;
    return true;
  } // if

  return false;
} // NonDeclaredStatement()

bool Parser::UnlabeledStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  if (ProgramEndingStatement(temp) || IfStatement(temp) || StatementI(temp)) {
    tokens = temp;
    return true;
  } // if

  return false;
} // UnlabeledStatement()

bool Parser::IfStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string condStr;
  int ifCondPos = UNDEFINED_NUMBER;
  int elsePos = UNDEFINED_NUMBER;
  int newQuaPos = UNDEFINED_NUMBER;
  if (SpecificReservedWord(temp, "IF") && Condition(temp, condStr)) {
    ifCondPos = tableManager->GetTableSize(QUADRUPLE_TABLE) + 1;
    if (! SpecificReservedWord(temp, "THEN") || ! StatementI(temp)) {
      return false;
    } // if

    elsePos = tableManager->GetTableSize(QUADRUPLE_TABLE) + 1;
    queue<TokenData> tempTemp = temp;
    if (SpecificReservedWord(temp, "ELSE") && StatementI(temp)) {
      tempTemp = temp;
    } // if

    temp = tempTemp;
    newQuaPos = tableManager->GetTableSize(QUADRUPLE_TABLE) + 1;
    if (! generator->IfStatement(condStr, curScope, ifCondPos, elsePos, newQuaPos)) {
      curErrorType = UNDEFINED_IDENTIFIER_ERROR;
      return false;
    } // if

    tokens = temp;
    return true;
  } // if

  return false;
} // IfStatement()

bool Parser::Condition(queue<TokenData> &tokens, string &resStr) {
  queue<TokenData> temp = tokens;
  string oprStr1;
  if (Expression(temp, oprStr1)) {
    resStr = oprStr1;
    string relStr, oprStr2;
    queue<TokenData> tempTemp = temp;
    while ((SpecificReservedWord(temp, "OR", relStr) || SpecificReservedWord(temp, "AND", relStr) ||
            SpecificReservedWord(temp, "EQ", relStr) || SpecificReservedWord(temp, "NE", relStr) ||
            SpecificReservedWord(temp, "GT", relStr) || SpecificReservedWord(temp, "GE", relStr) ||
            SpecificReservedWord(temp, "LT", relStr) || SpecificReservedWord(temp, "LE", relStr)) &&
           Expression(temp, oprStr2)) {
      tempTemp = temp;
      resStr = GetTemporaryString();
      if (! generator->Expression(oprStr1, oprStr2, relStr, resStr, curScope)) {
        curErrorType = UNDEFINED_IDENTIFIER_ERROR;
        return false;
      } // if

      oprStr1 = resStr;
    } // while

    temp = tempTemp;
    tokens = temp;
    return true;
  } // if

  return false;
} // Condition()

/**
 * Must return true because it can be empty statement.
 * If it meet the IO, CALL, GOTO, or Assign syntax, it will pop tokens from queue
 */
bool Parser::StatementI(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  if (IoStatement(temp) || CallStatement(temp) ||
      GotoStatement(temp) || AssignStatement(temp)) {
    tokens = temp;
  } // if

  return true;
} // StatementI()

bool Parser::IoStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string wordStr;
  string varStr;
  if ((SpecificReservedWord(temp, "INPUT", wordStr) || SpecificReservedWord(temp, "OUTPUT", wordStr)) &&
       AssignVariable(temp, varStr)) {
    if (! generator->IoStatement(wordStr, varStr, curScope)) {
      curErrorType = UNDEFINED_IDENTIFIER_ERROR;
      return false;
    } // if

    tokens = temp;
    return true;
  } // if

  return false;
} // IoStatement()

bool Parser::GotoStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string labelStr;
  if (SpecificReservedWord(temp, "GTO") && Identifier(temp, labelStr)) {
    if (! generator->GotoStatement(labelStr, curScope, curErrorType)) {
      return false;
    } // if

    curErrorType = GOTO_REFERENCE;
    tokens = temp;
    return true;
  } // if

  return false;
} // GotoStatement()

bool Parser::CallStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string funId;
  if (SpecificReservedWord(temp, "CALL") && Identifier(temp, funId) && SpecificDelimiter(temp, "(")) {
    vector<string> argVec;
    string str;
    if (Expression(temp, str)) {
      argVec.push_back(str);
      queue<TokenData> tempTemp = temp;
      while (SpecificDelimiter(temp, ",") && Expression(temp, str)) {
        tempTemp = temp;
        argVec.push_back(str);
      } // while

      temp = tempTemp;
      if (SpecificDelimiter(temp, ")")) {
        if (! generator->CallStatement(funId, curScope, argVec)) {
          curErrorType = UNDEFINED_IDENTIFIER_ERROR;
          return false;
        } // if

        curErrorType = CALL_REFERENCE;
        tokens = temp;
        return true;
      } // if
    } // if
  } // if

  return false;
} // CallStatement()

bool Parser::AssignStatement(queue<TokenData> &tokens) {
  queue<TokenData> temp = tokens;
  string assignVarStr;
  string expResStr;
  if (AssignVariable(temp, assignVarStr) && SpecificDelimiter(temp, "=") && Expression(temp, expResStr)) {
    if (! generator->Assign(assignVarStr, expResStr, curScope)) {
      curErrorType = UNDEFINED_IDENTIFIER_ERROR;
      return false;
    } // if

    tokens = temp;
    return true;
  } // if

  return false;
} // AssignStatement()

bool Parser::Expression(queue<TokenData> &tokens, string &resStr) {
  queue<TokenData> temp = tokens;
  string signStr;
  bool sign = false;
  ErrorType oriErrorType = curErrorType;
  if (SpecificDelimiter(temp, "+", signStr) || SpecificDelimiter(temp, "-", signStr)) {
    tokens = temp;
    sign = true;
  } // if
  curErrorType = oriErrorType;  // sign is dispensable, so don't have to set error type

  string oprStr1;
  if (Term(temp, oprStr1)) {
    tokens = temp;
    resStr = oprStr1;
    if (sign) {
      resStr = GetTemporaryString();
      if (! generator->Expression("0", oprStr1, signStr, resStr, curScope)) {
        curErrorType = UNDEFINED_IDENTIFIER_ERROR;
        return false;
      } // if

      oprStr1 = resStr;
    } // if

    string optorStr, oprStr2;
    while ((SpecificDelimiter(temp, "+", optorStr) || SpecificDelimiter(temp, "-", optorStr)) &&
           Term(temp, oprStr2)) {
      tokens = temp;
      resStr = GetTemporaryString();
      if (! generator->Expression(oprStr1, oprStr2, optorStr, resStr, curScope)) {
        curErrorType = UNDEFINED_IDENTIFIER_ERROR;
        return false;
      } // if

      oprStr1 = resStr;
    } // while

    return true;
  } // if

  return false;
} // Expression()

bool Parser::Term(queue<TokenData> &tokens, string &resStr) {
  queue<TokenData> temp = tokens;
  string oprStr1;
  if (Factor(temp, oprStr1)) {
    tokens = temp;
    resStr = oprStr1;
    string optorStr, oprStr2;
    while ((SpecificDelimiter(temp, "*", optorStr) || SpecificDelimiter(temp, "/", optorStr)) &&
           Factor(temp, oprStr2)) {
      tokens = temp;
      resStr = GetTemporaryString();
      if (! generator->Expression(oprStr1, oprStr2, optorStr, resStr, curScope)) {
        curErrorType = UNDEFINED_IDENTIFIER_ERROR;
        return false;
      } // if

      oprStr1 = resStr;
    } // while

    return true;
  } // if

  return false;
} // Term()

bool Parser::Factor(queue<TokenData> &tokens, string &resStr) {
  queue<TokenData> temp = tokens;
  string oprStr1;
  if (Variable(temp, oprStr1) || Number(temp, oprStr1) ||
      SpecificDelimiter(temp, "(") && Expression(temp, oprStr1) && SpecificDelimiter(temp, ")")) {
    tokens = temp;
    resStr = oprStr1;
    string oprStr2;
    while (SpecificDelimiter(temp, "^") && Factor(temp, oprStr2)) {
      tokens = temp;
      resStr = GetTemporaryString();
      if (! generator->Expression(oprStr1, oprStr2, "^", resStr, curScope)) {
        curErrorType = UNDEFINED_IDENTIFIER_ERROR;
        return false;
      } // if

      oprStr1 = resStr;
    } // while

    return true;
  } // if

  return false;
} // Factor()

bool Parser::Number(queue<TokenData> &tokens, string &resStr) {
  if (tokens.front().type == INTEGER_TABLE) {
    resStr = tableManager->GetKeyContent(INTEGER_TABLE, tokens.front().entry);
    tokens.pop();
    return true;
  } // if
  else if (tokens.front().type == REAL_TABLE) {
    resStr = tableManager->GetKeyContent(REAL_TABLE, tokens.front().entry);
    int pointNum = 0;
    for (int i = 0; i < resStr.length(); i++) {
      if (resStr[i] == '.') {
        pointNum++;
      } // if
    } // for

    if (pointNum != 1) {
      curErrorType = TOO_MANY_POINTS_ERROR;
      return false;
    } // if

    tokens.pop();
    return true;
  } // else if

  return false;
} // Number()

bool Parser::Variable(queue<TokenData> &tokens, string &resStr) {
  queue<TokenData> temp = tokens;
  if (ArrayElement(temp, resStr) || Identifier(temp, resStr)) {
    tokens = temp;
    return true;
  } // if

  return false;
} // Variable()

bool Parser::AssignVariable(queue<TokenData> &tokens, string &resStr) {
  queue<TokenData> temp = tokens;
  if (ArrayElement(temp, resStr, false) || Identifier(temp, resStr)) {
    tokens = temp;
    return true;
  } // if

  return false;
} // AssignVariable()

/**
 * tempResult, Specify whether to store the result as a temporary. Default true.
 * e.g. if true, result is [1]. However is x[17].
 */
bool Parser::ArrayElement(queue<TokenData> &tokens, string &resStr, bool tempResult) {
  queue<TokenData> temp = tokens;
  string idStr, indexStr;
  if (Identifier(temp, idStr) && SpecificDelimiter(temp, "(") && Expression(temp, indexStr)) {
    tokens = temp;
    vector<int> dim;
    if (! tableManager->GetArrayDimension(idStr, curScope, dim)) {
      curErrorType = NOT_APPLICABLE_IDENTIFIER_ERROR;
      return false;
    } // if

    int curDim = 1;  // record current dimension
    string indexResStr = indexStr;
    while (SpecificDelimiter(temp, ",") && Expression(temp, indexStr)) {
      tokens = temp;
      string tmpStr = GetTemporaryString();
      generator->Expression(indexStr, "1", "-", tmpStr, curScope);
      indexStr = tmpStr;
      for (int i = 0; i < curDim && i < dim.size(); i++) {
        stringstream ss;  // integer to string
        string numStr;
        ss << dim.at(i);
        ss >> numStr;
        tmpStr = GetTemporaryString();
        generator->Expression(indexStr, numStr, "*", tmpStr, curScope);
        indexStr = tmpStr;
      } // for

      tmpStr = GetTemporaryString();
      generator->Expression(indexResStr, indexStr, "+", tmpStr, curScope);
      indexResStr = tmpStr;
      curDim++;
    } // while

    if (curDim != dim.size()) {
      curErrorType = DIMENSION_ERROR;
      return false;
    } // if

    temp = tokens;
    if (SpecificDelimiter(temp, ")")) {
      tokens = temp;
      if (tempResult) {
        resStr = GetTemporaryString();
        generator->Assign(resStr, idStr + "[" + indexResStr + "]", curScope);
      } // if
      else {
        resStr = idStr + "[" + indexResStr + "]";
      } // else

      return true;
    } // if
  } // if

  return false;
} // ArrayElement()

bool Parser::Identifier(queue<TokenData> &tokens, string &id) {
  if (tokens.empty() || tokens.front().type != IDENTIFIER_TABLE) {
    return false;
  } // if

  id = tableManager->GetKeyContent(IDENTIFIER_TABLE, tokens.front().entry);
  if (id.length() <= 0) {
    return false;
  } // if

  for (int i = 0; i < id.length(); i++) {
    if (id[i] != '_' &&
        !(id[i] >= 'a' && id[i] <= 'z') &&
        !(id[i] >= 'A' && id[i] <= 'Z') &&
        !(i != 0 && id[i] >= '0' && id[i] <= '9')) {
      curErrorType = ILLEGAL_ID_NAME_ERROR;
      return false;
    } // if
  } // for

  tokens.pop();
  return true;
} // Identifier()

bool Parser::SpecificReservedWord(queue<TokenData> &tokens, string word) {
  string tStr;
  return SpecificReservedWord(tokens, word, tStr);
} // SpecificReservedWord()

/**
 * If error, it won't set error type
 */
bool Parser::SpecificReservedWord(queue<TokenData> &tokens, string word, string &getWord) {
  if (tokens.empty() || tokens.front().type != RESERVED_TABLE) {
    return false;
  } // if

  getWord = tableManager->GetKeyContent(RESERVED_TABLE, tokens.front().entry);
  if (getWord.compare(word) == 0) {
    tokens.pop();
    return true;
  } // if

  return false;
} // SpecificReservedWord()

bool Parser::SpecificDelimiter(queue<TokenData> &tokens, string delimiter) {
  string tStr;
  return SpecificDelimiter(tokens, delimiter, tStr);
} // SpecificDelimiter()

bool Parser::SpecificDelimiter(queue<TokenData> &tokens, string delimiter, string &getDelimiter) {
  if (tokens.empty() || tokens.front().type != DELIMITER_TABLE) {
    return false;
  } // if

  getDelimiter = tableManager->GetKeyContent(DELIMITER_TABLE, tokens.front().entry);
  if (getDelimiter.compare(delimiter) == 0) {
    tokens.pop();
    return true;
  } // if

  return false;
} // SpecificDelimiter()

/**
 * Get temporary string by tempNum, e.g. [1], [2]...
 * tempNum will accumulate one
 */
string Parser::GetTemporaryString() {
  stringstream ss;
  string temp;
  ss << "[" << tempNum << "]";
  ss >> temp;
  tempNum++;
  return temp;
} // GetTemporaryString()

int Parser::StringToInt(string str) {
  int resInt = 0;
  for (int i = 0; i < str.length(); i++) {
    resInt = resInt * 10 + (int)(str[i] - '0');
  } // for

  return resInt;
} // StringToInt()
