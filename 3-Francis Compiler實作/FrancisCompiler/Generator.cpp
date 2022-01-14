#include "Generator.h"
#include <sstream>
#include <assert.h>

Generator::Generator() {
  Init();
} // Generator()

void Generator::Init() {
  tableManager = nullptr;
} // Init()

void Generator::Init(TableManager &tm) {
  tableManager = &tm;
} // Init()

bool Generator::ProgramHeading(string id) {
  if (tableManager->Find(IDENTIFIER_TABLE, id, UNDEFINED_NUMBER, false) == UNDEFINED_NUMBER) {  // duplicated function name
    return false;
  } // if

  tableManager->FixIdentifierTableValue(id, UNDEFINED_NUMBER, 4, tableManager->GetTableSize(QUADRUPLE_TABLE) + 1);
  return true;
} // ProgramHeading()

void Generator::EndOfProgram(bool isEnp) {
  QuadrupleForm qf = NULL_QUADRUPLE;
  if (isEnp) {
    qf.opr = TokenData(RESERVED_TABLE, tableManager->Find(RESERVED_TABLE, "ENP"));
  } // if
  else {
    qf.opr = TokenData(RESERVED_TABLE, tableManager->Find(RESERVED_TABLE, "ENS"));
  } // else

  tableManager->Insert(QUADRUPLE_TABLE, qf);
} // EndOfProgram()

bool Generator::VariableDeclare(string id, int scope, DataType type) {
  if (tableManager->Find(IDENTIFIER_TABLE, id, scope) != UNDEFINED_NUMBER) {  // duplicated id define
    return false;
  } // if
  else if (tableManager->Find(IDENTIFIER_TABLE, id, UNDEFINED_NUMBER, false) == UNDEFINED_NUMBER) {
    // if no define identifier name (because same name in other scope)
    IdentifierEntry idItem = {id, scope, type, tableManager->GetTableSize(QUADRUPLE_TABLE) + 1};
    tableManager->InsertForce(IDENTIFIER_TABLE, idItem);
  } // else if
  else {
    tableManager->FixIdentifierTableValue(id, UNDEFINED_NUMBER, 2, scope);
    tableManager->FixIdentifierTableValue(id, scope, 3, type);
    tableManager->FixIdentifierTableValue(id, scope, 4, tableManager->GetTableSize(QUADRUPLE_TABLE) + 1);
  } // else

  QuadrupleForm qf = NULL_QUADRUPLE;
  qf.opr = TokenData(IDENTIFIER_TABLE, tableManager->Find(IDENTIFIER_TABLE, id, scope));
  tableManager->Insert(QUADRUPLE_TABLE, qf);
  return true;
} // VariableDeclare()

bool Generator::ArrayDeclare(string id, int scope, DataType type, const vector<int> &dim) {
  if (tableManager->Find(IDENTIFIER_TABLE, id, scope) != UNDEFINED_NUMBER) {  // duplicated id define
    return false;
  } // if
  else if (tableManager->Find(IDENTIFIER_TABLE, id, UNDEFINED_NUMBER, false) == UNDEFINED_NUMBER) {
    // if no define identifier name (because same name in other scope)
    IdentifierEntry idItem = {id, scope, ARRAY, tableManager->GetTableSize(INFORMATION_TABLE) + 1};
    tableManager->InsertForce(IDENTIFIER_TABLE, idItem);
  } // else if
  else {
    tableManager->FixIdentifierTableValue(id, UNDEFINED_NUMBER, 2, scope);
    tableManager->FixIdentifierTableValue(id, scope, 3, ARRAY);
    tableManager->FixIdentifierTableValue(id, scope, 4, tableManager->GetTableSize(INFORMATION_TABLE) + 1);
  } // else

  QuadrupleForm qf = NULL_QUADRUPLE;
  qf.opr = TokenData(IDENTIFIER_TABLE, tableManager->Find(IDENTIFIER_TABLE, id, scope));
  tableManager->Insert(QUADRUPLE_TABLE, qf);

  tableManager->Insert(INFORMATION_TABLE, type);  // insert information table
  tableManager->Insert(INFORMATION_TABLE, dim.size());
  for (int i = 0; i < dim.size(); i++) {
    tableManager->Insert(INFORMATION_TABLE, dim.at(i));
  } // for

  return true;
} // ArrayDeclare()

bool Generator::LabelDeclare(string id, int scope) {
  if (tableManager->Find(IDENTIFIER_TABLE, id, scope) != UNDEFINED_NUMBER) {  // duplicated id define
    return false;
  } // if
  else if (tableManager->Find(IDENTIFIER_TABLE, id, UNDEFINED_NUMBER, false) == UNDEFINED_NUMBER) {
    // if no define identifier name (because same name in other scope)
    IdentifierEntry idItem = {id, scope, LABEL, UNDEFINED_NUMBER};
    tableManager->InsertForce(IDENTIFIER_TABLE, idItem);
  } // else if
  else {
    tableManager->FixIdentifierTableValue(id, UNDEFINED_NUMBER, 2, scope);
    tableManager->FixIdentifierTableValue(id, scope, 3, LABEL);
  } // else

  QuadrupleForm qf = NULL_QUADRUPLE;
  qf.opr = TokenData(IDENTIFIER_TABLE, tableManager->Find(IDENTIFIER_TABLE, id, scope));
  tableManager->Insert(QUADRUPLE_TABLE, qf);
  return true;
} // LabelDeclare()

bool Generator::LabelDefine(string id, int scope, int quaPos, ErrorType &et) {
  IdentifierEntry idEntry;
  if (! tableManager->GetIdentifierEntry(id, scope, idEntry) ||
      idEntry.type == UNDEFINED_DATA) {
    et = UNDEFINED_IDENTIFIER_ERROR;
    return false;
  } // if
  else if (idEntry.type != LABEL) {
    et = NOT_APPLICABLE_IDENTIFIER_ERROR;
    return false;
  } // else if
  else if (idEntry.pointer != UNDEFINED_NUMBER) {
    et = DUPLICATED_DEFINE_ERROR;
    return false;
  } // else if

  tableManager->FixIdentifierTableValue(id, scope, 4, quaPos);
  return true;
} // LabelDefine()

bool Generator::GotoStatement(string id, int scope, ErrorType &et) {
  IdentifierEntry idEntry;
  if (! tableManager->GetIdentifierEntry(id, scope, idEntry) ||
      idEntry.type == UNDEFINED_DATA) {
    et = UNDEFINED_IDENTIFIER_ERROR;
    return false;
  } // if
  else if (idEntry.type != LABEL) {
    et = NOT_APPLICABLE_IDENTIFIER_ERROR;
    return false;
  } // else if

  QuadrupleForm qf = NULL_QUADRUPLE;
  qf.opr = TokenData(RESERVED_TABLE, tableManager->Find(RESERVED_TABLE, "GTO"));
  qf.res = TokenData(IDENTIFIER_TABLE, tableManager->Find(IDENTIFIER_TABLE, id, scope));
  tableManager->Insert(QUADRUPLE_TABLE, qf);
  return true;
} // GotoStatement()

bool Generator::CallStatement(string id, int scope, const vector<string> &arg) {
  QuadrupleForm qf = NULL_QUADRUPLE;
  qf.opr = TokenData(RESERVED_TABLE, tableManager->Find(RESERVED_TABLE, "CALL"));
  qf.op1 = TokenData(IDENTIFIER_TABLE, tableManager->Find(IDENTIFIER_TABLE, id, UNDEFINED_NUMBER));
  qf.res = TokenData(INFORMATION_TABLE, tableManager->GetTableSize(INFORMATION_TABLE) + 1);

  vector<TokenData> argTok;
  for (int i = 0; i < arg.size(); i++) {
    TokenData tok = NULL_TOKEN;
    if (! IsValidOperand(arg.at(i), scope, tok)) {
      return false;
    } // if

    argTok.push_back(tok);
  } // for

  tableManager->FixIdentifierTableValue(id, UNDEFINED_NUMBER, 3, CALLEE);
  tableManager->Insert(QUADRUPLE_TABLE, qf);
  tableManager->Insert(INFORMATION_TABLE, arg.size());
  for (int i = 0; i < arg.size(); i++) {
    tableManager->Insert(INFORMATION_TABLE, argTok.at(i).type);
    tableManager->Insert(INFORMATION_TABLE, argTok.at(i).entry);
  } // for

  return true;
} // CallStatement()

bool Generator::IoStatement(string IoWord, string id, int scope) {
  QuadrupleForm qf = NULL_QUADRUPLE;
  TokenData tokOptor = {RESERVED_TABLE, tableManager->Find(RESERVED_TABLE, IoWord)};
  TokenData tokVar, tokIdx;
  tokVar = NULL_TOKEN;
  tokIdx = NULL_TOKEN;
  if (IsValidOperand(id, scope, tokVar) || IsArray(id, scope, tokVar, tokIdx)) {
    qf.opr = tokOptor;
    qf.res = tokVar;
    qf.op1 = tokIdx;
    tableManager->Insert(QUADRUPLE_TABLE, qf);
    return true;
  } // if

  return false;
} // IoStatement()

bool Generator::Assign(string assignId, string expRes, int scope) {
  QuadrupleForm qf = NULL_QUADRUPLE;
  TokenData tokAss, tokExp, tokIndex;
  bool isAssValid, isExpValid;
  qf.opr = TokenData(DELIMITER_TABLE, tableManager->Find(DELIMITER_TABLE, "="));
  tokAss = NULL_TOKEN;
  tokExp = NULL_TOKEN;
  tokIndex = NULL_TOKEN;
  isAssValid = false;
  isExpValid = false;
  if (IsValidOperand(expRes, scope, tokExp) || IsArray(expRes, scope, tokExp, tokIndex)) {
    isExpValid = true;
    qf.op1 = tokExp;
    qf.op2 = tokIndex;
  } // if

  tokIndex = NULL_TOKEN;
  if (IsValidOperand(assignId, scope, tokAss) || IsArray(assignId, scope, tokAss, tokIndex)) {
    isAssValid = true;
    if (tokIndex == NULL_TOKEN) {
      qf.res = tokAss;
    } // if
    else {
      qf.op2 = tokAss;
      qf.res = tokIndex;
    } // else
  } // if

  if (isExpValid && isAssValid) {
    tableManager->Insert(QUADRUPLE_TABLE, qf);
    return true;
  } // if

  return false;
} // Assign()

/**
 * Generate expression quadruple form code. The temporary variable will be enclosed
 * in square brackets, e.g. [1], [2], [3]...
 * The array element will be enclosed in square brackets and prefix with identifier
 * name, e.g. xArray[7], yArray[279]...
 */
bool Generator::Expression(string opr1, string opr2, string optor, string res, int scope) {
  QuadrupleForm qf = NULL_QUADRUPLE;
  TokenData tok1, tok2, tokRes;
  TableType type = UNDEFINED_TABLE;
  if (optor.compare("+") == 0 || optor.compare("-") == 0 || optor.compare("*") == 0 ||
      optor.compare("/") == 0 || optor.compare("^") == 0) {
    type = DELIMITER_TABLE;
  } // if
  else if (optor.compare("OR") == 0 || optor.compare("AND") == 0 || optor.compare("EQ") == 0 ||
           optor.compare("NE") == 0 || optor.compare("GT") == 0 || optor.compare("GE") == 0 ||
           optor.compare("LT") == 0 || optor.compare("LE") == 0) {
    type = RESERVED_TABLE;
  } // else if

  if (type == DELIMITER_TABLE || type == RESERVED_TABLE) {
    if (IsValidOperand(opr1, scope, tok1) && IsValidOperand(opr2, scope, tok2) &&
        IsValidOperand(res, scope, tokRes)) {
      qf.opr = TokenData(type, tableManager->Find(type, optor));
      qf.op1 = tok1;
      qf.op2 = tok2;
      qf.res = tokRes;
      tableManager->Insert(QUADRUPLE_TABLE, qf);
      return true;
    } // if
  } // if

  return false;
} // Expression()

bool Generator::IfStatement(string condStr, int scope, int ifCondPos, int elsePos, int nextStatementPos) {
  TokenData condTok;
  if (! IsValidOperand(condStr, scope, condTok)) {
    return false;
  } // if

  QuadrupleForm ifQf = NULL_QUADRUPLE;
  QuadrupleForm jumpQf = NULL_QUADRUPLE;
  ifQf.opr = TokenData(RESERVED_TABLE, tableManager->Find(RESERVED_TABLE, "IF"));
  ifQf.op1 = condTok;
  ifQf.op2 = TokenData(QUADRUPLE_TABLE, ifCondPos + 1);
  if (elsePos != UNDEFINED_NUMBER) {
    ifQf.res = TokenData(QUADRUPLE_TABLE, elsePos + 2);
  } // if

  jumpQf.opr = TokenData(RESERVED_TABLE, tableManager->Find(RESERVED_TABLE, "GTO"));
  jumpQf.res = TokenData(QUADRUPLE_TABLE, nextStatementPos + 2);

  tableManager->Insert(QUADRUPLE_TABLE, ifCondPos, ifQf);
  if (elsePos != nextStatementPos) {
    tableManager->Insert(QUADRUPLE_TABLE, elsePos + 1, jumpQf);
  } // if

  return true;
} // IfStatement()

bool Generator::IsTemporary(string operand, int &tempNum) {
  if (operand.length() <= 0) {
    return false;
  } // if
  else if (operand[0] == '[' && operand[operand.length() - 1] == ']') {
    string numStr = operand.substr(1, operand.length() - 2);
    stringstream ss;
    ss << numStr;
    ss >> tempNum;
    return true;
  } // else if

  return false;
} // IsTemporary()

/**
 * Is operand existent identifier or temporary.
 */
bool Generator::IsValidOperand(string operand, int scope, TokenData &tokData) {
  tokData.entry = tableManager->Find(IDENTIFIER_TABLE, operand, scope, false);
  if (tokData.entry != UNDEFINED_NUMBER) {
    tokData.type = IDENTIFIER_TABLE;
    return true;
  } // if

  tokData.entry = tableManager->Find(INTEGER_TABLE, operand);
  if (tokData.entry != UNDEFINED_NUMBER) {
    tokData.type = INTEGER_TABLE;
    return true;
  } // else if

  tokData.entry = tableManager->Find(REAL_TABLE, operand);
  if (tokData.entry != UNDEFINED_NUMBER) {
    tokData.type = REAL_TABLE;
    return true;
  } // else if

  if (IsTemporary(operand, tokData.entry)) {
    tokData.type = TEMPORARY_TABLE;
    return true;
  } // if

  if (operand.compare("0") == 0 || operand.compare("1") == 0) {
    tokData.entry = tableManager->Insert(INTEGER_TABLE, operand);
    tokData.type = INTEGER_TABLE;
    return true;
  } // if

  return false;
} // IsValidOperand()

bool Generator::IsArray(string operand, int scope, TokenData &idTok, TokenData &indexTok) {
  if (operand.length() <= 0) {
    return false;
  } // if
  else if (operand[0] != '[' && operand.find("[") != string::npos && operand.find("]") != string::npos) {
    string idStr, indexStr;
    idStr = operand.substr(0, operand.find_first_of("["));
    indexStr = operand.substr(operand.find_first_of("[") + 1, operand.find_last_of("]") - operand.find_first_of("[") - 1);
    if (IsValidOperand(idStr, scope, idTok) && IsValidOperand(indexStr, scope, indexTok)) {
      return true;
    } // if
  } // else if

  return false;
} // IsArray()
