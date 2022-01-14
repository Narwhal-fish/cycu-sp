#ifndef DEFINITION_H_INCLUDED
#define DEFINITION_H_INCLUDED

#include <string>
using namespace std;

#define HASH_TABLE_SIZE 100
#define UNDEFINED_NUMBER -1

#define READ_FILE_STAGE 1
#define OUTPUT_STAGE 2
#define EXIT_STAGE 3

/*
 * Token definition area
 */
enum TableType {
  UNDEFINED_TABLE = -1,
  TEMPORARY_TABLE = 0,
  DELIMITER_TABLE,
  RESERVED_TABLE,
  INTEGER_TABLE,
  REAL_TABLE,
  IDENTIFIER_TABLE,
  QUADRUPLE_TABLE,
  INFORMATION_TABLE
};

enum DataType {
  UNDEFINED_DATA = 0,
  ARRAY = 1,
  BOOLEAN,
  CHARACTER,
  INTEGER,
  LABEL,
  REAL,
  CALLEE
};

struct TokenData {
  TableType type;
  int entry;

  TokenData();
  TokenData(TableType type, int entry);
  bool operator==(const TokenData &o);
  bool operator!=(const TokenData &o);
};

struct IdentifierEntry {
  string id;
  int scope;
  DataType type;
  int pointer;

  bool operator==(const IdentifierEntry &o);
  bool operator!=(const IdentifierEntry &o);
};

struct QuadrupleForm {
  TokenData opr;  // operator
  TokenData op1;  // operand 1
  TokenData op2;  // operand 2
  TokenData res;  // result
};

const TokenData NULL_TOKEN = {UNDEFINED_TABLE, UNDEFINED_NUMBER};
const IdentifierEntry NULL_ID_ENTRY = {"", UNDEFINED_NUMBER, UNDEFINED_DATA, UNDEFINED_NUMBER};
const QuadrupleForm NULL_QUADRUPLE = {NULL_TOKEN, NULL_TOKEN, NULL_TOKEN, NULL_TOKEN};

const TokenData COLON_TOKEN = {DELIMITER_TABLE, 1};

/*
 * Error definition area
 */
enum ErrorType {
  UNDEFINED_ERROR = 0,
  SYNTAX_ERROR = 1,
  ILLEGAL_ID_NAME_ERROR,
  INNER_HEADING_ERROR,
  NO_HEADING_ERROR,
  NO_ENDING_ERROR,
  DUPLICATED_DEFINE_ERROR,
  UNDEFINED_IDENTIFIER_ERROR,
  NOT_APPLICABLE_IDENTIFIER_ERROR,
  TOO_MANY_POINTS_ERROR,
  DIMENSION_ERROR,
  CALL_REFERENCE,
  GOTO_REFERENCE
};

struct ErrorInfo {
  int line;
  ErrorType type;
};

/*
 * Exception definition area
 */
class ExpMsg {
public:
  const static string INVALID_TABLE;
  const static string HASH_TABLE_CAPACITY_FULL;
};

#endif // DEFINITION_H_INCLUDED
