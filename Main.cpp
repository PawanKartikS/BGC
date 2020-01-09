// CppParser.hpp
// Light weight utility to track various entities within a C
// source file.
// As of now, only tracks heap allocs.

#include "CppParser.hpp"
#include <clang-c/Index.h>
#include <iostream>
using namespace std;

int
main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "Invoke: Main <File.c>\n";
    return 1;
  }

  if (!CppParser::isValidTU(argv[1])) {
    cerr << "Invalid file: File must be a valid C source "
            "file!\n";
    return 1;
  }

  CppParser parser{argv[1]};
  auto container = parser.trackHeapAllocs();

  while (!container.empty()) {
    auto pair = container.top();
    container.pop();

    cout << pair.first << ": \n";
    while (!pair.second.empty()) {
      cout << pair.second.back() << "\n";
      pair.second.pop_back();
    }
  }

  return 0;
}