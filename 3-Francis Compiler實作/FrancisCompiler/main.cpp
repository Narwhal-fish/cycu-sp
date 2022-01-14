/*
 * project name: Francis Compiler
 *
 * id:     10827216
 * author: ¤ý¥ßµ¾
 */

// #define DEBUG

#ifdef DEBUG
#include "Test.h"
#else
#define NDEBUG // ban assert
#endif // DEBUG


#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "ProcessManager.h"

int main() {
#ifdef DEBUG
  Test();
#endif
  ProcessManager pm;
  pm.Start();

  return 0;
} // main()
