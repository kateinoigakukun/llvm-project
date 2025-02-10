//===-- SwiftBasicTypes.cpp -------------------------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2016 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "SwiftBasicTypes.h"

#include "lldb/ValueObject/ValueObject.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

using namespace lldb;
using namespace lldb_private;
using namespace lldb_private::formatters;
using namespace lldb_private::formatters::swift;

extern "C" void emscripten_debugger(void);
lldb::ValueObjectSP lldb_private::formatters::swift::
    SwiftBasicTypeSyntheticFrontEnd::GetSyntheticValue() {
  auto child = m_backend.GetChildAtIndex(0, true);
  llvm::StringRef name = child->GetName();
  llvm::StringRef swiftSummary = child ? child->GetSummaryAsCString(lldb::eLanguageTypeSwift) : "<no child>";
  llvm::StringRef summary = child ? child->GetSummaryAsCString() : "<no child>";

  llvm::outs() << "[DEBUG] " << __func__ << " " << __FILE__ << ":" << __LINE__
               << " child: " << child.get();
  if (child) {
    llvm::outs() << " Name: " << name << "\n";
    llvm::outs() << " Summary(lldb::eLanguageTypeSwift): " << swiftSummary << "\n";
    llvm::outs() << " Summary(): " << summary << "\n";
  }
  return child;
}

SyntheticChildrenFrontEnd *
lldb_private::formatters::swift::SwiftBasicTypeSyntheticFrontEndCreator(
    CXXSyntheticChildren *, lldb::ValueObjectSP valobj_sp) {
  llvm::outs() << "[DEBUG] " << __func__ << " " << __FILE__ << ":" << __LINE__
               << " valobj_sp: " << valobj_sp.get() << "\n";
  static bool first = true;
  if (first) {
    first = false;
    emscripten_debugger();
  }
  if (valobj_sp)
    return new SwiftBasicTypeSyntheticFrontEnd(*valobj_sp);
  return nullptr;
}
