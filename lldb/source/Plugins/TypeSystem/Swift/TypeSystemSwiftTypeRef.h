//===-- TypeSystemSwiftTypeRef.h --------------------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2020 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_TypeSystemSwiftTypeRef_h_
#define liblldb_TypeSystemSwiftTypeRef_h_

#include "Plugins/TypeSystem/Swift/TypeSystemSwift.h"
#include "lldb/Core/SwiftForward.h"

#include "swift/AST/Type.h"
#include "swift/Demangling/Demangle.h"
#include "swift/Demangling/Demangler.h"

// FIXME: needed only for the DenseMap.
#include "clang/APINotes/APINotesManager.h"
#include "clang/Basic/Module.h"

namespace lldb_private {
class SwiftASTContext;
class ClangExternalASTSourceCallbacks;

/// A Swift TypeSystem that does not own a swift::ASTContext.
class TypeSystemSwiftTypeRef : public TypeSystemSwift {
  /// LLVM RTTI support.
  static char ID;

public:
  /// LLVM RTTI support.
  /// \{
  bool isA(const void *ClassID) const override {
    return ClassID == &ID || TypeSystemSwift::isA(ClassID);
  }
  static bool classof(const TypeSystem *ts) { return ts->isA(&ID); }
  /// \}

  TypeSystemSwiftTypeRef(SwiftASTContext *swift_ast_context);

  Module *GetModule() const;
  swift::CanType GetCanonicalSwiftType(CompilerType compiler_type);
  swift::Type GetSwiftType(CompilerType compiler_type);
  CompilerType ReconstructType(CompilerType type);
  CompilerType
  GetTypeFromMangledTypename(ConstString mangled_typename) override;

  CompilerType GetGenericArgumentType(lldb::opaque_compiler_type_t type,
                                      size_t idx) override;

  // PluginInterface functions
  ConstString GetPluginName() override;
  uint32_t GetPluginVersion() override;

  bool SupportsLanguage(lldb::LanguageType language) override;
  Status IsCompatible() override;

  void DiagnoseWarnings(Process &process, Module &module) const override;
  DWARFASTParser *GetDWARFParser() override;
  // CompilerDecl functions
  ConstString DeclGetName(void *opaque_decl) override {
    return ConstString("");
  }

  // CompilerDeclContext functions
  std::vector<CompilerDecl>
  DeclContextFindDeclByName(void *opaque_decl_ctx, ConstString name,
                            const bool ignore_imported_decls) override {
    return {};
  }

  bool DeclContextIsContainedInLookup(void *opaque_decl_ctx,
                                      void *other_opaque_decl_ctx) override {
    if (opaque_decl_ctx == other_opaque_decl_ctx)
      return true;
    return false;
  }

  // Tests
#ifndef NDEBUG
  bool Verify(lldb::opaque_compiler_type_t type) override;
#endif
  bool IsArrayType(lldb::opaque_compiler_type_t type,
                   CompilerType *element_type, uint64_t *size,
                   bool *is_incomplete) override;
  bool IsAggregateType(lldb::opaque_compiler_type_t type) override;
  bool IsDefined(lldb::opaque_compiler_type_t type) override;
  bool IsFunctionType(lldb::opaque_compiler_type_t type,
                      bool *is_variadic_ptr) override;
  size_t
  GetNumberOfFunctionArguments(lldb::opaque_compiler_type_t type) override;
  CompilerType GetFunctionArgumentAtIndex(lldb::opaque_compiler_type_t type,
                                          const size_t index) override;
  bool IsFunctionPointerType(lldb::opaque_compiler_type_t type) override;
  bool IsPossibleDynamicType(lldb::opaque_compiler_type_t type,
                             CompilerType *target_type, // Can pass NULL
                             bool check_cplusplus, bool check_objc) override;
  bool IsPointerType(lldb::opaque_compiler_type_t type,
                     CompilerType *pointee_type) override;
  bool IsVoidType(lldb::opaque_compiler_type_t type) override;
  // AST related queries
  uint32_t GetPointerByteSize() override;
  // Accessors
  ConstString GetTypeName(lldb::opaque_compiler_type_t type) override;
  ConstString GetDisplayTypeName(lldb::opaque_compiler_type_t type,
                                 const SymbolContext *sc) override;
  ConstString GetMangledTypeName(lldb::opaque_compiler_type_t type) override;
  uint32_t GetTypeInfo(lldb::opaque_compiler_type_t type,
                       CompilerType *pointee_or_element_clang_type) override;
  lldb::TypeClass GetTypeClass(lldb::opaque_compiler_type_t type) override;

  // Creating related types
  CompilerType GetArrayElementType(lldb::opaque_compiler_type_t type,
                                   uint64_t *stride,
                                   ExecutionContextScope *exe_scope) override;
  CompilerType GetCanonicalType(lldb::opaque_compiler_type_t type) override;
  int GetFunctionArgumentCount(lldb::opaque_compiler_type_t type) override;
  CompilerType GetFunctionArgumentTypeAtIndex(lldb::opaque_compiler_type_t type,
                                              size_t idx) override;
  CompilerType
  GetFunctionReturnType(lldb::opaque_compiler_type_t type) override;
  size_t GetNumMemberFunctions(lldb::opaque_compiler_type_t type) override;
  TypeMemberFunctionImpl
  GetMemberFunctionAtIndex(lldb::opaque_compiler_type_t type,
                           size_t idx) override;
  CompilerType GetPointeeType(lldb::opaque_compiler_type_t type) override;
  CompilerType GetPointerType(lldb::opaque_compiler_type_t type) override;

  // Exploring the type
  llvm::Optional<uint64_t>
  GetBitSize(lldb::opaque_compiler_type_t type,
             ExecutionContextScope *exe_scope) override;
  llvm::Optional<uint64_t>
  GetByteStride(lldb::opaque_compiler_type_t type,
                ExecutionContextScope *exe_scope) override;
  lldb::Encoding GetEncoding(lldb::opaque_compiler_type_t type,
                             uint64_t &count) override;
  lldb::Format GetFormat(lldb::opaque_compiler_type_t type) override;
  uint32_t GetNumChildren(lldb::opaque_compiler_type_t type,
                          bool omit_empty_base_classes,
                          const ExecutionContext *exe_ctx) override;
  uint32_t GetNumFields(lldb::opaque_compiler_type_t type) override;
  CompilerType GetFieldAtIndex(lldb::opaque_compiler_type_t type, size_t idx,
                               std::string &name, uint64_t *bit_offset_ptr,
                               uint32_t *bitfield_bit_size_ptr,
                               bool *is_bitfield_ptr) override;
  CompilerType GetChildCompilerTypeAtIndex(
      lldb::opaque_compiler_type_t type, ExecutionContext *exe_ctx, size_t idx,
      bool transparent_pointers, bool omit_empty_base_classes,
      bool ignore_array_bounds, std::string &child_name,
      uint32_t &child_byte_size, int32_t &child_byte_offset,
      uint32_t &child_bitfield_bit_size, uint32_t &child_bitfield_bit_offset,
      bool &child_is_base_class, bool &child_is_deref_of_parent,
      ValueObject *valobj, uint64_t &language_flags) override;
  uint32_t GetIndexOfChildWithName(lldb::opaque_compiler_type_t type,
                                   const char *name,
                                   bool omit_empty_base_classes) override;
  size_t
  GetIndexOfChildMemberWithName(lldb::opaque_compiler_type_t type,
                                const char *name, bool omit_empty_base_classes,
                                std::vector<uint32_t> &child_indexes) override;
  size_t GetNumTemplateArguments(lldb::opaque_compiler_type_t type) override;
  CompilerType GetTypeForFormatters(lldb::opaque_compiler_type_t type) override;
  LazyBool ShouldPrintAsOneLiner(lldb::opaque_compiler_type_t type,
                                 ValueObject *valobj) override;
  bool IsMeaninglessWithoutDynamicResolution(
      lldb::opaque_compiler_type_t type) override;

  // Dumping types
#ifndef NDEBUG
  /// Convenience LLVM-style dump method for use in the debugger only.
  LLVM_DUMP_METHOD virtual void
  dump(lldb::opaque_compiler_type_t type) const override;
#endif

  void DumpValue(lldb::opaque_compiler_type_t type, ExecutionContext *exe_ctx,
                 Stream *s, lldb::Format format, const DataExtractor &data,
                 lldb::offset_t data_offset, size_t data_byte_size,
                 uint32_t bitfield_bit_size, uint32_t bitfield_bit_offset,
                 bool show_types, bool show_summary, bool verbose,
                 uint32_t depth) override;

  bool DumpTypeValue(lldb::opaque_compiler_type_t type, Stream *s,
                     lldb::Format format, const DataExtractor &data,
                     lldb::offset_t data_offset, size_t data_byte_size,
                     uint32_t bitfield_bit_size, uint32_t bitfield_bit_offset,
                     ExecutionContextScope *exe_scope,
                     bool is_base_class) override;

  void DumpTypeDescription(
      lldb::opaque_compiler_type_t type,
      lldb::DescriptionLevel level = lldb::eDescriptionLevelFull) override;
  void DumpTypeDescription(
      lldb::opaque_compiler_type_t type, Stream *s,
      lldb::DescriptionLevel level = lldb::eDescriptionLevelFull) override;
  void DumpSummary(lldb::opaque_compiler_type_t type, ExecutionContext *exe_ctx,
                   Stream *s, const DataExtractor &data,
                   lldb::offset_t data_offset, size_t data_byte_size) override;
  bool IsPointerOrReferenceType(lldb::opaque_compiler_type_t type,
                                CompilerType *pointee_type) override;
  llvm::Optional<size_t>
  GetTypeBitAlign(lldb::opaque_compiler_type_t type,
                  ExecutionContextScope *exe_scope) override;
  CompilerType GetBuiltinTypeForEncodingAndBitSize(lldb::Encoding encoding,
                                                   size_t bit_size) override {
    return CompilerType();
  }
  bool IsTypedefType(lldb::opaque_compiler_type_t type) override;
  CompilerType GetTypedefedType(lldb::opaque_compiler_type_t type) override;
  CompilerType
  GetFullyUnqualifiedType(lldb::opaque_compiler_type_t type) override;
  CompilerType GetNonReferenceType(lldb::opaque_compiler_type_t type) override;
  CompilerType
  GetLValueReferenceType(lldb::opaque_compiler_type_t type) override;
  CompilerType
  GetRValueReferenceType(lldb::opaque_compiler_type_t type) override;
  uint32_t GetNumDirectBaseClasses(lldb::opaque_compiler_type_t type) override;
  CompilerType GetDirectBaseClassAtIndex(lldb::opaque_compiler_type_t type,
                                         size_t idx,
                                         uint32_t *bit_offset_ptr) override;
  bool IsReferenceType(lldb::opaque_compiler_type_t type,
                       CompilerType *pointee_type, bool *is_rvalue) override;

  // Swift-specific methods.
  lldb::TypeSP GetCachedType(ConstString mangled) override;
  void SetCachedType(ConstString mangled, const lldb::TypeSP &type_sp) override;
  bool IsImportedType(lldb::opaque_compiler_type_t type,
                      CompilerType *original_type) override;
  CompilerType GetErrorType() override;
  CompilerType GetReferentType(lldb::opaque_compiler_type_t type) override;
  CompilerType GetInstanceType(lldb::opaque_compiler_type_t type) override;
  TypeAllocationStrategy
  GetAllocationStrategy(lldb::opaque_compiler_type_t type) override;
  CompilerType
  CreateTupleType(const std::vector<TupleElement> &elements) override;
  void DumpTypeDescription(
      lldb::opaque_compiler_type_t type, bool print_help_if_available,
      bool print_extensions_if_available,
      lldb::DescriptionLevel level = lldb::eDescriptionLevelFull) override;
  void DumpTypeDescription(
      lldb::opaque_compiler_type_t type, Stream *s,
      bool print_help_if_available, bool print_extensions_if_available,
      lldb::DescriptionLevel level = lldb::eDescriptionLevelFull) override;

private:
  /// Helper that creates an AST type from \p type.
  void *ReconstructType(lldb::opaque_compiler_type_t type);
  /// Cast \p opaque_type as a mangled name.
  const char *AsMangledName(lldb::opaque_compiler_type_t opaque_type);

  /// Wrap \p node as \p Global(TypeMangling(node)), remangle the type
  /// and create a CompilerType from it.
  CompilerType RemangleAsType(swift::Demangle::Demangler &Dem,
                              swift::Demangle::NodePointer node);

  /// Demangle the mangled name of the canonical type of \p type and
  /// drill into the Global(TypeMangling(Type())).
  ///
  /// \return the child of Type or a nullptr.
  swift::Demangle::NodePointer
  DemangleCanonicalType(swift::Demangle::Demangler &Dem,
                        lldb::opaque_compiler_type_t type);

  /// Return an APINotes manager for the module with module id \id.
  /// APINotes are used to get at the SDK swiftification annotations.
  clang::api_notes::APINotesManager *
  GetAPINotesManager(ClangExternalASTSourceCallbacks *source, unsigned id);

  /// The sibling SwiftASTContext.
  SwiftASTContext *m_swift_ast_context = nullptr;

  /// The APINotesManager responsible for each Clang module.
  llvm::DenseMap<clang::Module *,
                 std::unique_ptr<clang::api_notes::APINotesManager>>
      m_apinotes_manager;
};

} // namespace lldb_private
#endif
