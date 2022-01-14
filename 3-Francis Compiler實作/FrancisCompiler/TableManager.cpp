#include "TableManager.h"
#include <assert.h>
#include <sstream>

TableManager::TableManager() {
  Init();
} // TableManager()

void TableManager::Init() {
  delimiterTable.clear();
  reservedWordTable.clear();
  integerTable.clear();
  realTable.clear();
  identifierTable.clear();
  quadrupleTable.clear();
  informationTable.clear();
  for (int i = 0; i < HASH_TABLE_SIZE; i++) { // fill empty entry
    identifierTable.push_back(NULL_ID_ENTRY);
  } // for
} // Init()

void TableManager::LoadTable(TableType type) throw(string) {
  if (type == DELIMITER_TABLE) {
    delimiterTable.push_back(";");
    delimiterTable.push_back("(");
    delimiterTable.push_back(")");
    delimiterTable.push_back("=");
    delimiterTable.push_back("+");
    delimiterTable.push_back("-");
    delimiterTable.push_back("*");
    delimiterTable.push_back("/");
    delimiterTable.push_back("^");
    delimiterTable.push_back("¡ô");
    delimiterTable.push_back(",");
    delimiterTable.push_back(":");
    return;
  } // if
  else if (type == RESERVED_TABLE) {
    reservedWordTable.push_back("AND");
    reservedWordTable.push_back("BOOLEAN");
    reservedWordTable.push_back("CALL");
    reservedWordTable.push_back("DIMENSION");
    reservedWordTable.push_back("ELSE");
    reservedWordTable.push_back("ENP");
    reservedWordTable.push_back("ENS");
    reservedWordTable.push_back("EQ");
    reservedWordTable.push_back("GE");
    reservedWordTable.push_back("GT");
    reservedWordTable.push_back("GTO");
    reservedWordTable.push_back("IF");
    reservedWordTable.push_back("INPUT");
    reservedWordTable.push_back("INTEGER");
    reservedWordTable.push_back("LABEL");
    reservedWordTable.push_back("LE");
    reservedWordTable.push_back("LT");
    reservedWordTable.push_back("NE");
    reservedWordTable.push_back("OR");
    reservedWordTable.push_back("OUTPUT");
    reservedWordTable.push_back("PROGRAM");
    reservedWordTable.push_back("REAL");
    reservedWordTable.push_back("SUBROUTINE");
    reservedWordTable.push_back("THEN");
    reservedWordTable.push_back("VARIABLE");
    return;
  } // else if

  throw ExpMsg::INVALID_TABLE;
} // LoadTable()

/*
void TableManager::LoadTable(TableType type, string fileName) throw(string) {

} // LoadTable()
*/

int TableManager::GetTableSize(TableType type) {
  switch (type) {
    case INTEGER_TABLE:
      return integerTable.size();
    case REAL_TABLE:
      return realTable.size();
    case IDENTIFIER_TABLE:
      return GetIdentifierTableSize();
    case QUADRUPLE_TABLE:
      return quadrupleTable.size();
    case INFORMATION_TABLE:
      return informationTable.size();
    default:
      throw ExpMsg::INVALID_TABLE;
  } // switch
} // GetTableSize()

int TableManager::Insert(TableType type, string addItem) throw(string) {
  if (type == INTEGER_TABLE) {
    int pos = Find(type, addItem);
    if (pos == UNDEFINED_NUMBER) {
      integerTable.push_back(addItem);
      return integerTable.size();
    } // if

    return pos;
  } // else if
  else if (type == REAL_TABLE) {
    int pos = Find(type, addItem);
    if (pos == UNDEFINED_NUMBER) {
      realTable.push_back(addItem);
      return realTable.size();
    } // if

    return pos;
  } // else if

  throw ExpMsg::INVALID_TABLE;
} // Insert()

int TableManager::Insert(TableType type, int addItem) throw(string) {
  if (type == INFORMATION_TABLE) {
    informationTable.push_back(addItem);
    return informationTable.size();
  } // if

  throw ExpMsg::INVALID_TABLE;
} // Insert()

int TableManager::Insert(TableType type, const IdentifierEntry &addItem) throw(string) {
  if (type == IDENTIFIER_TABLE) {
    if (GetIdentifierTableSize() >= identifierTable.size()) {
      throw ExpMsg::HASH_TABLE_CAPACITY_FULL;
    } // if

    string idKey = addItem.id;
    int scopeKey = addItem.scope;
    int pos = Find(type, idKey, scopeKey);
    if (pos != UNDEFINED_NUMBER) {  // inserting-key exist
      return pos;
    } // if

    int startIndex = HashValue(idKey);
    int step = 0;
    while (identifierTable.at(ProbeValue(startIndex, step)) != NULL_ID_ENTRY) {
      step++;
    } // while

    pos = ProbeValue(startIndex, step);
    identifierTable.at(pos) = addItem;
    return pos;
  } // if

  throw ExpMsg::INVALID_TABLE;
} // Insert()

int TableManager::Insert(TableType type, const QuadrupleForm &addItem) throw(string) {
  if (type == QUADRUPLE_TABLE) {
    quadrupleTable.push_back(addItem);
    return quadrupleTable.size();
  } // if

  throw ExpMsg::INVALID_TABLE;
} // Insert()

int TableManager::Insert(TableType type, int insertPos, const QuadrupleForm &addItem) throw(string) {
  if (type == QUADRUPLE_TABLE) {
    quadrupleTable.insert(quadrupleTable.begin() + insertPos - 1, addItem);
    return insertPos;
  } // if

  throw ExpMsg::INVALID_TABLE;
} // Insert()

int TableManager::InsertForce(TableType type, const IdentifierEntry &addItem) throw(string) {
  if (type == IDENTIFIER_TABLE) {
    if (GetIdentifierTableSize() >= identifierTable.size()) {
      throw ExpMsg::HASH_TABLE_CAPACITY_FULL;
    } // if

    string key = addItem.id;
    int pos;
    int startIndex = HashValue(key);
    int step = 0;
    while (identifierTable.at(ProbeValue(startIndex, step)) != NULL_ID_ENTRY) {
      step++;
    } // while

    pos = ProbeValue(startIndex, step);
    identifierTable.at(pos) = addItem;
    return pos;
  } // if

  throw ExpMsg::INVALID_TABLE;
} // InsertForce()

int TableManager::Find(TableType type, string searchKey) throw(string) {
  string upperStr = searchKey;
  for (unsigned int i = 0; i < searchKey.length(); i++) {
    upperStr[i] = toupper(searchKey[i]);
  } // for

  if (type == DELIMITER_TABLE) {
    for (unsigned int i = 0; i < delimiterTable.size(); i++) {
      if (delimiterTable.at(i).compare(upperStr) == 0) {
        return i + 1;
      } // if
    } // for

    return UNDEFINED_NUMBER;
  } // if
  else if (type == RESERVED_TABLE) {
    for (unsigned int i = 0; i < reservedWordTable.size(); i++) {
      if (reservedWordTable.at(i).compare(upperStr) == 0) {
        return i + 1;
      } // if
    } // for

    return UNDEFINED_NUMBER;
  } // else if
  else if (type == INTEGER_TABLE) {
    for (unsigned int i = 0; i < integerTable.size(); i++) {
      if (integerTable.at(i).compare(searchKey) == 0) {
        return i + 1;
      } // if
    } // for

    return UNDEFINED_NUMBER;
  } // else if
  else if (type == REAL_TABLE) {
    for (unsigned int i = 0; i < realTable.size(); i++) {
      if (realTable.at(i).compare(searchKey) == 0) {
        return i + 1;
      } // if
    } // for

    return UNDEFINED_NUMBER;
  } // else if

  throw ExpMsg::INVALID_TABLE;
} // Find()

/**
 * Use identifier name and scope number as key. Search item by key in identifier table.
 * If set findFunctionName false, searching will skip function name. Default true.
 */
int TableManager::Find(TableType type, string idKey, int scopeKey, bool findFunctionName) throw(string) {
  if (type == IDENTIFIER_TABLE) {
    int startIndex = HashValue(idKey);
    int step = 0;
    string cmpId;
    int cmpScope;
    bool isFunctionName;
    while (step < identifierTable.size()) {
      if (identifierTable.at(ProbeValue(startIndex, step)) == NULL_ID_ENTRY) {  // empty
        break;
      } // if

      cmpId = identifierTable.at(ProbeValue(startIndex, step)).id;
      cmpScope = identifierTable.at(ProbeValue(startIndex, step)).scope;
      isFunctionName = identifierTable.at(ProbeValue(startIndex, step)).scope == UNDEFINED_NUMBER &&
                       identifierTable.at(ProbeValue(startIndex, step)).type == UNDEFINED_DATA &&
                       identifierTable.at(ProbeValue(startIndex, step)).pointer != UNDEFINED_NUMBER;
      if (idKey.compare(cmpId) == 0 && scopeKey == cmpScope && (findFunctionName || ! isFunctionName)) {
        return ProbeValue(startIndex, step);
      } // else if

      step++;
    } // while

    return UNDEFINED_NUMBER;
  } // if

  throw ExpMsg::INVALID_TABLE;
} // Find()

string TableManager::GetKeyContent(TableType type, int entry) throw(string) {
  if (type == DELIMITER_TABLE) {
    return delimiterTable.at(entry - 1);
  } // if
  else if (type == RESERVED_TABLE) {
    return reservedWordTable.at(entry - 1);
  } // else if
  else if (type == INTEGER_TABLE) {
    return integerTable.at(entry - 1);
  } // else if
  else if (type == REAL_TABLE) {
    return realTable.at(entry - 1);
  } // else if
  else if (type == IDENTIFIER_TABLE) {
    return identifierTable.at(entry).id;
  } // else if
  else if (type == TEMPORARY_TABLE) {
    stringstream ss;
    string str;
    ss << "T" << entry;
    ss >> str;
    return str;
  } // else if

  throw ExpMsg::INVALID_TABLE;
} // GetKeyContent()

bool TableManager::GetArrayDimension(string arrayName, int scope, vector<int> &dim) {
  int entry = Find(IDENTIFIER_TABLE, arrayName, scope);
  if (entry == UNDEFINED_NUMBER || identifierTable.at(entry).type != ARRAY) {
    return false;
  } // if

  int d = informationTable.at(identifierTable.at(entry).pointer);
  int start = identifierTable.at(entry).pointer + 1;
  for (int i = start; i < start + d; i++) {
    dim.push_back(informationTable.at(i));
  } // for

  return true;
} // GetArrayDimension()

bool TableManager::GetIdentifierEntry(string idName, int scope, IdentifierEntry &idEntry) {
  int entry = Find(IDENTIFIER_TABLE, idName, scope);
  if (entry == UNDEFINED_NUMBER) {
    return false;
  } // if

  idEntry = identifierTable.at(entry);
  return true;
} // GetIdentifierDataType()

/**
 * When lexical analysis stage, it may produce some invalid identifier name
 * (because some error will be found in late stage, e.g. undefined identifier)
 * Clear these invalid identifier by scan a table.  Invalid identifier has
 * three undefined fields.
 * After clear invalid identifier, this function will adjust some identifier
 * to the correct position in the table.
 */
void TableManager::ClearInvalidIdentifier() {
  for (int i = 0; i < identifierTable.size(); i++) {
    if (identifierTable.at(i).scope == UNDEFINED_NUMBER &&
        identifierTable.at(i).type == UNDEFINED_DATA &&
        identifierTable.at(i).pointer == UNDEFINED_NUMBER) {
      identifierTable.at(i) = NULL_ID_ENTRY;
      int step = 1;
      int t = ProbeValue(i, step);
      while (step < identifierTable.size() && identifierTable.at(t) != NULL_ID_ENTRY) {
        if (Insert(IDENTIFIER_TABLE, identifierTable.at(t)) != t) {  // entry was adjusted to correct position
          identifierTable.at(t) = NULL_ID_ENTRY;
        } // if

        step++;
        t = ProbeValue(i, step);
      } // while
    } // if
  } // for
} // ClearInvalidIdentifier()

bool TableManager::FixIdentifierTableValue(string idKey, int scopeKey, int field, int value) {
  int pos = Find(IDENTIFIER_TABLE, idKey, scopeKey);
  if (pos == UNDEFINED_NUMBER) {
    return false;
  } // if

  if (field == 2) {
    identifierTable.at(pos).scope = value;
    return true;
  } // if
  else if (field == 4) {
    identifierTable.at(pos).pointer = value;
    return true;
  } // else if
  else if (field == 3) {
    switch (value) {
      case 1:
        identifierTable.at(pos).type = ARRAY;
        return true;
      case 2:
        identifierTable.at(pos).type = BOOLEAN;
        return true;
      case 3:
        identifierTable.at(pos).type = CHARACTER;
        return true;
      case 4:
        identifierTable.at(pos).type = INTEGER;
        return true;
      case 5:
        identifierTable.at(pos).type = LABEL;
        return true;
      case 6:
        identifierTable.at(pos).type = REAL;
        return true;
      case 7:
        identifierTable.at(pos).type = CALLEE;
        return true;
      default:
        break;
    } // switch
  } // else if

  return false;
} // FixIdentifierTableValue()

/**
 * Check if every function has defined.  If defined, remove corresponding error
 * information for CALL_REFERENCE
 */
void TableManager::CheckCallReference(vector<ErrorInfo> &errorInfo) {
  int nextCallRef = 0;
  for (int i = 0; i < quadrupleTable.size(); i++) {
    while (nextCallRef < errorInfo.size() &&
           errorInfo.at(nextCallRef).type != CALL_REFERENCE) {
      nextCallRef++;
    } // while

    if (nextCallRef >= errorInfo.size()) {
      break;
    } // if
    else if (quadrupleTable.at(i).opr.type == RESERVED_TABLE &&  // find CALL immediate code
             quadrupleTable.at(i).opr.entry == 3) {
      int funIdPos = quadrupleTable.at(i).op1.entry;
      if (identifierTable.at(funIdPos).pointer != UNDEFINED_NUMBER) {
        vector<ErrorInfo>::iterator elemToRemove = errorInfo.begin() + nextCallRef;
        errorInfo.erase(elemToRemove);
      } // if
    } // else if
  } // for
} // CheckCallReference()

/**
 * Do pass 2 jobs. Scan quadruple table, find GTO statement. modify the forth
 * field to correct address
 */
void TableManager::FillGotoAddr(vector<ErrorInfo> &errorInfo) {
  int nextGotoRef = 0;
  for (int i = 0; i < quadrupleTable.size(); i++) {
    while (nextGotoRef < errorInfo.size() &&
           errorInfo.at(nextGotoRef).type != GOTO_REFERENCE) {
      nextGotoRef++;
    } // while

    if (nextGotoRef >= errorInfo.size()) {
      break;
    } // if
    else if (quadrupleTable.at(i).opr.type == RESERVED_TABLE &&  // GOTO statement
             quadrupleTable.at(i).opr.entry == 11) {
      if (quadrupleTable.at(i).res.type == IDENTIFIER_TABLE) {
        int labelPos = quadrupleTable.at(i).res.entry;
        TokenData tok = NULL_TOKEN;
        if (identifierTable.at(labelPos).pointer != UNDEFINED_NUMBER) {
          vector<ErrorInfo>::iterator elemToRemove = errorInfo.begin() + nextGotoRef;
          errorInfo.erase(elemToRemove);
          tok.type = QUADRUPLE_TABLE;
          tok.entry = identifierTable.at(labelPos).pointer;
        } // if

        quadrupleTable.at(i).res = tok;
      } // if
    } // else if
  } // for
} // FillGotoAddr()

int TableManager::GetIdentifierTableSize() {
  int count = identifierTable.size();
  for (int i = 0; i < identifierTable.size(); i++) {
    if (identifierTable.at(i) == NULL_ID_ENTRY) {
      count--;
    } // if
  } // for

  return count;
} // GetIdentifierTableSize()

int TableManager::HashValue(string key) {
  int hashValue = 0;
  for (unsigned int i = 0; i < key.length(); i++) {
    hashValue = (hashValue + (int)key[i]) % identifierTable.size();
  } // for

  return hashValue;
} // HashValue()

int TableManager::ProbeValue(int index, int step) {
  return (index + step) % identifierTable.size();
} // ProbeValue()

void TableManager::GetQuadrupleTable(vector<QuadrupleForm> &quadrupleTable) {
  quadrupleTable = this->quadrupleTable;
} // GetQuadrupleTable()

void TableManager::GetIntegerTable(vector<string> &integerTable) {
  integerTable = this->integerTable;
} // GetIntegerTable()

void TableManager::GetRealTable(vector<string> &realTable) {
  realTable = this->realTable;
} // GetRealTable()

void TableManager::GetIdentifierTable(vector<IdentifierEntry> &identifierTable) {
  identifierTable = this->identifierTable;
} // GetIdentifierTable()

void TableManager::GetInformationTable(vector<int> &informationTable) {
  informationTable = this->informationTable;
} // GetInformationTable

string TableManager::GetHumanCode(const QuadrupleForm &qf) {
  string str;
  string humanCode = "Unknown Intermediate Code";
  if (qf.opr.type == IDENTIFIER_TABLE) {  /** >>>>> id <<<<< */
    humanCode = GetKeyContent(IDENTIFIER_TABLE, qf.opr.entry);
  } // if
  else if (qf.opr.type == RESERVED_TABLE && (qf.opr.entry == 6 || qf.opr.entry == 7)) {  /** >>>>> ENP, ENS <<<<< */
    humanCode = GetKeyContent(RESERVED_TABLE, qf.opr.entry);
  } // else if
  else if (qf.opr.type == RESERVED_TABLE && qf.opr.entry == 12) {  /** >>>>> IF <<<<< */
    humanCode = GetKeyContent(RESERVED_TABLE, qf.opr.entry);
    str = " " + GetKeyContent(qf.op1.type, qf.op1.entry);
    humanCode = humanCode + str;
  } // else if
  else if (qf.opr.type == RESERVED_TABLE && qf.opr.entry == 11) {  /** >>>>> GTO <<<<< */
    humanCode = GetKeyContent(RESERVED_TABLE, qf.opr.entry);
    TokenData resTok = qf.res;
    if (resTok != NULL_TOKEN) {
      int quaPos = resTok.entry;
      str.clear();
      for (int i = 0; i < identifierTable.size(); i++) {
        if (identifierTable.at(i).type == LABEL && identifierTable.at(i).pointer == quaPos) {
          str = identifierTable.at(i).id;
        } // if
      } // for

      if (! str.empty()) {
        humanCode = humanCode + " " + str;
      } // if
    } // if
  } // else if
  else if (qf.opr.type == RESERVED_TABLE && qf.opr.entry == 3) {  /** >>>>> CALL <<<<< */
    humanCode = GetKeyContent(RESERVED_TABLE, qf.opr.entry) + " " +
                GetKeyContent(IDENTIFIER_TABLE, qf.op1.entry);
    string argStr = "(";
    int argNum = informationTable.at(qf.res.entry - 1);
    int argType, argEntry;
    for (int i = 0; i < argNum; i++) {
      argType = informationTable.at(qf.res.entry + i * 2);
      argEntry = informationTable.at(qf.res.entry + i * 2 + 1);
      str = GetKeyContent((TableType)argType, argEntry);
      argStr = argStr + str;
      if (i < argNum - 1) {
        argStr = argStr + ", ";
      } // if
      else { // i == argNum - 1
        argStr = argStr + ")";
      } // else
    } // for

    humanCode = humanCode + argStr;
  } // else if
  else if (qf.opr.type == RESERVED_TABLE && (qf.opr.entry == 13 || qf.opr.entry == 20)) {  /** >>>>> INPUT, OUTPUT <<<<< */
    humanCode.clear();
    str = GetKeyContent(RESERVED_TABLE, qf.opr.entry) + " ";  // input, output
    humanCode = humanCode + str;

    str = GetKeyContent(qf.res.type, qf.res.entry);  // identifier
    humanCode = humanCode + str;

    TokenData tokTemp = qf.op1;
    if (tokTemp != NULL_TOKEN) {  // index
      str = GetKeyContent(qf.op1.type, qf.op1.entry);
      str = "(" + str + ")";
      humanCode = humanCode + str;
    } // if
  } // else if
  else if ((qf.opr.type == DELIMITER_TABLE && qf.opr.entry >= 5 && qf.opr.entry <= 9) ||  /** >>>>> +, -, *, /, ^ <<<<< */
           (qf.opr.type == RESERVED_TABLE && (qf.opr.entry == 1 ||                        /** >>>>> AND, EQ, GE, GT, LE, LT, NE, OR <<<<< */
                                              qf.opr.entry >= 8 && qf.opr.entry <= 10 ||
                                              qf.opr.entry >= 16 && qf.opr.entry <= 19))) {
    humanCode.clear();
    str = GetKeyContent(qf.res.type, qf.res.entry);  // result
    str = str + " = ";
    humanCode = humanCode + str;

    str = GetKeyContent(qf.op1.type, qf.op1.entry);  // operand 1
    humanCode = humanCode + str;

    str = " " + GetKeyContent(qf.opr.type, qf.opr.entry) + " ";  // operator
    humanCode = humanCode + str;

    str = GetKeyContent(qf.op2.type, qf.op2.entry);  // operator 2
    humanCode = humanCode + str;
  } // else if
  else if (qf.opr.type == DELIMITER_TABLE && qf.opr.entry == 4) {  /** >>>>> = <<<<< */
    humanCode.clear();
    if (qf.op2.type == IDENTIFIER_TABLE && identifierTable.at(qf.op2.entry).type == ARRAY) {  // id[index] = id
      str = GetKeyContent(IDENTIFIER_TABLE, qf.op2.entry);  // id
      humanCode = humanCode + str;

      str = GetKeyContent(qf.res.type, qf.res.entry);  // index
      str = "(" + str + ") = ";
      humanCode = humanCode + str;

      str = GetKeyContent(qf.op1.type, qf.op1.entry);  // operand
      humanCode = humanCode + str;
    } // if
    else {  // id = id or id = id[index]
      str = GetKeyContent(qf.res.type, qf.res.entry);  // result
      str = str + " = ";
      humanCode = humanCode + str;

      str = GetKeyContent(qf.op1.type, qf.op1.entry);  // operand
      humanCode = humanCode + str;

      TokenData tokTemp = qf.op2;
      if (tokTemp != NULL_TOKEN) {  // index
        str = GetKeyContent(qf.op2.type, qf.op2.entry);
        str = "(" + str + ")";
        humanCode = humanCode + str;
      } // if
    } // else
  } // else if

  return humanCode;
} // GetHumanCode()
