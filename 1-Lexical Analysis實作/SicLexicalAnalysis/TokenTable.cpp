#include "TokenTable.h"
#include <fstream>

// ============ Table ============

Table::Table(vector<TokenStr> &vec) {
  this->tableSize = vec.size();
  tableContent = new TokenStr[this->tableSize + 1];
  for (int i = 1; i <= this->tableSize; i++) {
    string &t = vec.at(i - 1);
    for (int si = 0; si < t.length(); si++) {
      if (islower(t[si]) != 0) {
        t[si] = toupper(t[si]);
      } // if
    } // for

    tableContent[i] = t;
  } // for
} // Table()

Table::~Table() {
  delete[] tableContent;
} // ~Table()

int Table::Find(TokenStr findToken) {
  string &t = findToken;
  for (int si = 0; si < t.length(); si++) {
    if (islower(t[si]) != 0) {
      t[si] = toupper(t[si]);
    } // if
  } // for

  for (int i = 1; i <= tableSize; i++) {
    if (t.compare(tableContent[i]) == 0) {
      return i;
    } // if
  } // for

  return NOT_FOUND;
} // Find()

vector<TokenStr> Table::ReadTableFile(string fileName) {
  vector<TokenStr> vec;
  ifstream fin;
  fin.open(fileName.c_str());
  if (! fin.is_open()) {
    return vec;
  } // if

  TokenStr tokenStr;
  TokenStr noWhiteSpaceTokenStr;
  while (! fin.eof()) {
    getline(fin, tokenStr);
    noWhiteSpaceTokenStr.clear();
    for (int i = 0; i < tokenStr.length(); i++) {
      if (tokenStr[i] != ' ' && tokenStr[i] != '\t') {
        noWhiteSpaceTokenStr = noWhiteSpaceTokenStr + tokenStr[i];
      } // if
    } // for

    if (tokenStr.length() > 0) {
      vec.push_back(noWhiteSpaceTokenStr);
    } // if
  } // while

  fin.close();
  return vec;
} // ReadTableFile()

// ========== Hash Table ==========

HashTable::HashTable(int tableSize) {
  this->tableSize = tableSize;
  tableContent = new TokenStr[this->tableSize];
  for (int i = 0; i < this->tableSize; i++) {
    tableContent[i].clear();
  } // for
} // HashTable()

HashTable::~HashTable() {
  delete[] tableContent;
} // ~HashTable()

int HashTable::Insert(TokenStr insertToken) {
  int curIdx = mpHashFunction(insertToken);
  while (! tableContent[curIdx].empty()) {
    curIdx = (curIdx + 1) % tableSize;
  } // while

  tableContent[curIdx] = insertToken;
  return curIdx;
} // Insert()

int HashTable::Find(TokenStr findToken) {
  int hashVal = mpHashFunction(findToken);
  int probeTime = 0;
  bool notFound = false;
  while (tableContent[(hashVal + probeTime) % tableSize].compare(findToken) != 0) {
    if (tableContent[(hashVal + probeTime) % tableSize].empty() ||
        probeTime >= tableSize) {
      notFound = true;
      break;
    } // if

    probeTime++;
  } // while

  if (notFound) {
    return NOT_FOUND;
  } // if
  else {
    return (hashVal + probeTime) % tableSize;
  } // else
} // Find()

int HashTable::mpHashFunction(TokenStr key) {
  int hashValue = 0;
  for (int i = 0; i < key.length(); i++) {
    hashValue = (hashValue + (int)key[i]) % tableSize;
  } // for

  return hashValue;
} // mpHashFunction()
