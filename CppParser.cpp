// CppParser.cpp
// Light weight utility to track various entities within a C
// source file.

#include "CppParser.hpp"
#include <iostream>
#include <stdexcept>
#include <unordered_set>

CppParser::CppParser(const string &translationUnit)
    : _translationUnit(
          translationUnit) // Name of the file we're parsing
    , _idx(clang_createIndex(0, 0)) // Clang's index
    , _tunit(
          clang_parseTranslationUnit( // The TU we obtain
                                      // from clang's index
              _idx, _translationUnit.c_str(), nullptr,
              0, // Note that we do not yet fetch a cursor
                 // as _tunit might be null. We only do that
                 // after we've verified _tunit is not null
                 // (file is valid!)
              nullptr, 0, 0)) {
  if (_tunit == nullptr) {
    throw invalid_argument("Invalid translation unit " +
                           _translationUnit);
  }

  _cursor = clang_getTranslationUnitCursor(_tunit);
}

// The traditional way to get a cursor's name is to call
// clang's API, and then call dispose() to free the alloc'd
// memory. These wrappers simply cast void* to char* and
// return it's string value without the need for calling
// dispose() later.
string
CppParser::cursorName(CXCursor cursor) {
  return CppParser::cxString(
      clang_getCursorDisplayName(cursor));
}

string
CppParser::cursorTypeName(CXCursor cursor) {
  return CppParser::cxString(
      clang_getTypeSpelling(clang_getCursorType(cursor)));
}

string
CppParser::cxString(CXString str) {
  return string((char *)str.data);
}

// Enumates the nodes in a translation unit.
// f is a pointer to a function or a lambda that is passed
// to clang_visitChildre(). Mark any containers as static to
// use them inside the lambda or function.
void
CppParser::enumNodes(CXChildVisitResult (*f)(
    CXCursor c, CXCursor p, CXClientData d)) {
  clang_visitChildren(_cursor, f, nullptr);
}

// Overloaded method that allows you to further visit nodes
// from a particular node with modified recurse behaviour.
void
CppParser::enumNodes(
    CXCursor cursor,
    CXChildVisitResult (*f)(CXCursor c, CXCursor p,
                            CXClientData d)) {
  clang_visitChildren(cursor, f, nullptr);
}

bool
CppParser::isValidTU(const string &filename) {
  auto pos = filename.find(".");
  return pos != string::npos &&
         filename.substr(pos + 1) == "c";
}

// Tracks all the heap allocations in the translation unit.
// Container storage format:
// FuncName, vector containing heap allocs in the function.
stack<pair<string, vector<string>>>
CppParser::trackHeapAllocs(void) {
  static string var;
  static string func;
  static unordered_set<string> freed;
  static stack<pair<string, vector<string>>> heapalloc;

  auto lambda =
      [](CXCursor child, CXCursor parent,
         CXClientData data) -> CXChildVisitResult {
    if (CppParser::cursorName(child) == "malloc") {
      // first entry
      if (heapalloc.empty()) {
        heapalloc.push({func, {var}});
        return CXChildVisit_Recurse;
      }

      auto lFunc = heapalloc.top().first;
      auto lVar = heapalloc.top().second.back();

      // we're within the function's scope
      if (lFunc == func) {
        if (lVar != var)
          heapalloc.top().second.push_back(var);
      } else {
        // out of function's scope
        heapalloc.push({func, {var}});
      }

    } else {
      CXCursorKind kind = clang_getCursorKind(child);
      if (kind == CXCursorKind::CXCursor_VarDecl) {
        var = CppParser::cursorName(child);
      } else if (kind ==
                 CXCursorKind::CXCursor_FunctionDecl) {
        func = CppParser::cursorName(child);
      } else if (kind == CXCursorKind::CXCursor_CallExpr &&
                 CppParser::cursorName(child) == "free") {
        /**
         * TODO:
         * There's no need to call
         * clang_Cursor_getNumArguments() as free() takes
         * only 1 argument.
         */
        const int kargs =
            clang_Cursor_getNumArguments(child);

        /* The var that we've freed */
        auto arg = CppParser::cursorName(
            clang_Cursor_getArgument(child, 0));
        auto funcScope = heapalloc.top().first;
        /* Mark as freed */
        auto key = funcScope + "-" + arg;
        freed.insert(key);
      }
    }

    return CXChildVisit_Recurse;
  };

  CppParser::enumNodes(lambda);

  stack<pair<string, vector<string>>> heapVars;
  while (!heapalloc.empty()) {
    auto p = heapalloc.top();
    auto func = p.first;
    heapalloc.pop();

    for (const auto &var : p.second) {
      auto key = func + "-" + var;
      if (freed.find(key) == freed.end())
        heapVars.push({func, {var}});
    }
  }

  return heapVars;
}