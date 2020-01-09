// CppParser.hpp
// Light weight utility to track various entities within a C
// source file.

#pragma once

#include <clang-c/Index.h>
#include <stack>
#include <string>
#include <vector>

using namespace std;

struct CppParser {
private:
  string _translationUnit;
  CXIndex _idx;
  CXTranslationUnit _tunit;
  CXCursor _cursor;

public:
  explicit CppParser(const string &translationUnit);
  static string cursorName(CXCursor cursor);
  static string cursorTypeName(CXCursor cursor);
  static string cxString(CXString str);
  void enumNodes(CXChildVisitResult (*f)(CXCursor c,
                                         CXCursor p,
                                         CXClientData d));
  void enumNodes(CXCursor cursor,
                 CXChildVisitResult (*f)(CXCursor c,
                                         CXCursor p,
                                         CXClientData d));
  static bool isValidTU(const string &filename);
  stack<pair<string, vector<string>>> trackHeapAllocs(void);
};