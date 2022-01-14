#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include "Definition.h"
#include <string>
#include <vector>
using namespace std;

class TableManager
{
public:
  TableManager();
  void Init();
  void LoadTable(TableType type) throw(string);
  // void LoadTable(TableType type, string fileName) throw(string);
  int GetTableSize(TableType type);
  int Insert(TableType type, string addItem) throw(string);
  int Insert(TableType type, int addItem) throw(string);
  int Insert(TableType type, const IdentifierEntry &addItem) throw(string);
  int Insert(TableType type, const QuadrupleForm &addItem) throw(string);
  int Insert(TableType type, int insertPos, const QuadrupleForm &addItem) throw(string);
  int InsertForce(TableType type, const IdentifierEntry &addItem) throw(string);  // accept duplicated key
  int Find(TableType type, string searchKey) throw(string);
  int Find(TableType type, string idKey, int scopeKey, bool findFunctionName = true) throw(string);
  string GetKeyContent(TableType type, int entry) throw(string);
  bool GetArrayDimension(string arrayName, int scope, vector<int> &dim);
  bool GetIdentifierEntry(string idName, int scope, IdentifierEntry &idEntry);
  void ClearInvalidIdentifier();
  bool FixIdentifierTableValue(string idKey, int scopeKey, int field, int value);
  void CheckCallReference(vector<ErrorInfo> &errorInfo);  // check every call reference
  void FillGotoAddr(vector<ErrorInfo> &errorInfo);  // pass 2 for quadruple table
  void GetIntegerTable(vector<string> &integerTable);
  void GetRealTable(vector<string> &realTable);
  void GetIdentifierTable(vector<IdentifierEntry> &identifierTable);
  void GetQuadrupleTable(vector<QuadrupleForm> &quadrupleTable);
  void GetInformationTable(vector<int> &informationTable);
  string GetHumanCode(const QuadrupleForm &qf);

private:
  int GetIdentifierTableSize();
  int HashValue(string key);
  int ProbeValue(int index, int step);

  vector<string> delimiterTable;
  vector<string> reservedWordTable;
  vector<string> integerTable;
  vector<string> realTable;
  vector<IdentifierEntry> identifierTable;
  vector<QuadrupleForm> quadrupleTable;
  vector<int> informationTable;
};

#endif // TABLEMANAGER_H
