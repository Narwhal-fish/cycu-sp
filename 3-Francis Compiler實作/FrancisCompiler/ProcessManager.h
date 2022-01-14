#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "Compiler.h"
#include <vector>
#include <string>
using namespace std;

class ProcessManager
{
public:
  void Start();

private:
  void InteractiveMode();
  bool ReadFilePart();
  bool CompilePart();
  bool OutputPart();
  bool AskNextStepPart(int &nextStep);
  bool ReadSource(string fileName);
  bool OutputCode(string name);

  Compiler compiler;
  vector<string> srcCode;
  vector<string> codeStrVec;  // intermediate code
  vector<string> errStrVec;  // error message
  vector<string> debugInfo;  // dump all table information
};

#endif // PROCESSMANAGER_H
