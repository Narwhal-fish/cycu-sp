#include "Definition.h"

const string ExpMsg::INVALID_TABLE = "Invalid table type!";
const string ExpMsg::HASH_TABLE_CAPACITY_FULL = "Hash table is full!";

TokenData::TokenData() {
} // TokenData()

TokenData::TokenData(TableType type, int entry) {
  this->type = type;
  this->entry = entry;
} // TokenData()

bool TokenData::operator==(const TokenData &o) {
  return type == o.type && entry == o.entry;
} // ==

bool TokenData::operator!=(const TokenData &o) {
  return type != o.type || entry != o.entry;
} // !=

bool IdentifierEntry::operator==(const IdentifierEntry &o) {
  return id.compare(o.id) == 0 && scope == o.scope &&
         type == o.type && pointer == o.pointer;
} // ==

bool IdentifierEntry::operator!=(const IdentifierEntry &o) {
  return id.compare(o.id) != 0 || scope != o.scope ||
         type != o.type || pointer != o.pointer;
} // !=
