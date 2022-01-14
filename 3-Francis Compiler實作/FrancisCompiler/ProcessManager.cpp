#include "ProcessManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Definition.h"
using namespace std;

void ProcessManager::Start() {
  cout << ">>> Welcome to use Francis Compiler <<<" << endl;
  InteractiveMode();
  cout << ">>> Bye bye! <<<" << endl;
} // Start()

void ProcessManager::InteractiveMode() {
  int nextStep = READ_FILE_STAGE;
  do {
    switch (nextStep) {
      case READ_FILE_STAGE:
        if (! ReadFilePart()) {
          return;
        } // if

        CompilePart();
      case OUTPUT_STAGE:
        if (! OutputPart()) {
          return;
        } // if
      default:
        if (! AskNextStepPart(nextStep)) {
          return;
        } // if
    } // switch
  } while (nextStep != EXIT_STAGE);
} // InteractiveMode()

bool ProcessManager::ReadFilePart() {
  string str;
  cout << "Input the file name of source code:" << endl;
  cout << "> ";
  cin >> str;
  while (str.compare("exit") != 0 && ! ReadSource(str)) {
    cout << "### No such file! ###" << endl;
    cout << "Input the file name of source code:" << endl;
    cout << "> ";
    cin >> str;
  } // while

  if (str.compare("exit") == 0) {
    return false;
  } // if

  cout << "Success!" << endl;
  return true;
} // ReadFilePart()

bool ProcessManager::CompilePart() {
  cout << "Compile..." << endl;
  compiler.Compile(srcCode);
  codeStrVec.clear();
  errStrVec.clear();
  debugInfo.clear();
  compiler.GetIntermediateStr(codeStrVec);
  compiler.GetErrorMessageStr(errStrVec);
  compiler.DumpTableInfo(debugInfo);
  cout << "Complete!" << endl;
  return true;
} // CompilePart()

bool ProcessManager::OutputPart() {
  string str;
  cout << "Select a option to output Intermediate code:" << endl;
  cout << "(1) Console" << endl;
  cout << "(2) Text File" << endl;
  cout << "> ";
  cin >> str;
  while (str.compare("exit") != 0 && str.compare("1") != 0 &&
         str.compare("2") != 0 && str.compare("debug") != 0) {
    cout << "### Invalid! ###" << endl;
    cout << "Select a option to output Intermediate code:" << endl;
    cout << "(1) Console" << endl;
    cout << "(2) Text File" << endl;
    cout << "> ";
    cin >> str;
  } // while

  if (str.compare("exit") == 0) {
    return false;
  } // if
  else if (str.compare("1") == 0) {
    OutputCode("stdout");
    cout << "Success!" << endl;
  } // else if
  else if (str.compare("2") == 0) {
    cout << "Input the destination file name:" << endl;
    cout << "> ";
    cin >> str;
    while (str.compare("exit") != 0 && ! OutputCode(str)) {
      cout << "### Failed! ###" << endl;
      cout << "Input the destination file name:" << endl;
      cout << "> ";
      cin >> str;
    } // while

    if (str.compare("exit") == 0) {
      return false;
    } // if

    cout << "Success!" << endl;
  } // else if
  else if (str.compare("debug") == 0) {
    cout << "Debug mode output, dump all table information!" << endl;
    OutputCode("debug");
    cout << "Success!" << endl;
  } // else if

  return true;
} // OutputPart()

bool ProcessManager::AskNextStepPart(int &nextStep) {
  string str;
  cout << "Continue?" << endl;
  cout << "(1) Select other file" << endl;
  cout << "(2) Continue output" << endl;
  cout << "(3) Exit" << endl;
  cout << "> ";
  cin >> str;
  while (str.compare("exit") != 0 && str.compare("1") != 0 &&
         str.compare("2") != 0 && str.compare("3") != 0) {
    cout << "### Invalid! ###" << endl;
    cout << "Continue?" << endl;
    cout << "(1) Select other file" << endl;
    cout << "(2) Continue output" << endl;
    cout << "(3) Exit" << endl;
    cout << "> ";
    cin >> str;
  } // while

  if (str.compare("exit") == 0) {
    return false;
  } // if
  else if (str.compare("1") == 0) {
    nextStep = READ_FILE_STAGE;
  } // else if
  else if (str.compare("2") == 0) {
    nextStep = OUTPUT_STAGE;
  } // else if
  else if (str.compare("3") == 0) {
    nextStep = EXIT_STAGE;
  } // else if

  return true;
} // AskNextStepPart()

bool ProcessManager::ReadSource(string fileName) {
  ifstream fin((fileName + ".txt").c_str());
  if (! fin.is_open()) {
    return false;
  } // if

  srcCode.clear();
  string str;
  getline(fin, str);
  while (! fin.eof()) {
    srcCode.push_back(str);
    getline(fin, str);
  } // while

  if (str.length() > 0) {
    srcCode.push_back(str);
  } // if

  fin.close();
  return true;
} // ReadSource()

bool ProcessManager::OutputCode(string name) {
  if (name.compare("stdout") == 0) {
    for (int i = 0; i < codeStrVec.size(); i++) {
      cout << i + 1 << "\t" << codeStrVec.at(i) << endl;
    } // for

    if (! errStrVec.empty()) cout << endl;
    for (int i = 0; i < errStrVec.size(); i++) {
      cout << errStrVec.at(i) << endl;
    } // for
  } // if
  else if (name.compare("debug") == 0) {
    cout << "================= Debug =================" << endl;
    for (int i = 0; i < debugInfo.size(); i++) {
      cout << debugInfo.at(i) << endl;
    } // for

    cout << endl << "Error Message:" << endl;
    for (int i = 0; i < errStrVec.size(); i++) {
      cout << errStrVec.at(i) << endl;
    } // for

    cout << "================= Debug =================" << endl;
    ofstream fout((name + ".txt").c_str());
    for (int i = 0; i < debugInfo.size(); i++) {
      fout << debugInfo.at(i) << endl;
    } // for

    fout << endl << "Error Message:" << endl;
    for (int i = 0; i < errStrVec.size(); i++) {
      fout << errStrVec.at(i) << endl;
    } // for

    fout.close();
  } // else if
  else {
    name = name.empty() ? "default" : name;
    ofstream fout((name + ".txt").c_str());
    if (! fout.is_open()) {
      return false;
    } // if

    for (int i = 0; i < codeStrVec.size(); i++) {
      fout << i + 1 << "\t" << codeStrVec.at(i) << endl;
    } // for

    if (! errStrVec.empty()) fout << endl;
    for (int i = 0; i < errStrVec.size(); i++) {
      fout << errStrVec.at(i) << endl;
    } // for

    fout.close();
  } // else

  return true;
} // OutputCode()
