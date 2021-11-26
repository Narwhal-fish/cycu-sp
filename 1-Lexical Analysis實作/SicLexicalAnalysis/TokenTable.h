#ifndef TOKENTABLE_H
#define TOKENTABLE_H

#include <vector>
#include <string>
using namespace std;

typedef string TokenStr;
const int NOT_FOUND = -1;

class Table
{
public:
  Table(vector<TokenStr> &vec);
  ~Table();
  int Find(TokenStr findToken);
  static vector<TokenStr> ReadTableFile(string fileName);

private:
  TokenStr *tableContent; // valid index in 1 ~ tableSize
  int tableSize;
};

class HashTable
{
public:
  HashTable(int tableSize);
  ~HashTable();
  int Insert(TokenStr insertToken);
  int Find(TokenStr findToken);

private:
  int mpHashFunction(TokenStr key);

  TokenStr *tableContent; // valid index in 0 ~ tableSize - 1
  int tableSize;
};

#endif // TOKENTABLE_H
