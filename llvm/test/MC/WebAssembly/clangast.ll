; RUN: llc -filetype=obj %s -o - | llvm-readobj -S | FileCheck %s

target triple = "wasm32-unknown-unknown"

@dummy = hidden global [6 x i8] c"hello\00", align 1
@__clang_ast = internal constant [4 x i8] c"CPCH", section "__clangast", align 4
!0 = !{ !"anchor", !"\00" }
!wasm.custom_sections = !{ !0 }

; The size of __clang_ast should be 27.
; ┌───────────────────────────────┬───────────────────────────────┬─────────┐
; │         content type          │       contet bytes (hex)      │   size  │
; ├───────────────────────────────┼───────────────────────────────┼─────────┤
; │ section id                    │ 00                            │  1 byte │
; │ size of content (LEB128)      │ 95 80 80 80 00                │  5 byte │
; │ size of section name (LEB128) │ 8a                            │  1 byte │
; │ padding to align              │ 80 80 80 80 80 00             │  6 byte │
; │ section name                  │ 5f 5f 63 6c 61 6e 67 61 73 74 │ 10 byte │
; │ content of __clang_ast        │ 43 50 43 48                   │  4 byte │
; ├───────────────────────────────┼───────────────────────────────┼─────────┤
; │             sum               │                -              │ 27 byte │
; └─────────────────────────────────────────────────────────────────────────┘

; The content of __clang_ast should be aligned by 4,
; so the size of section name is padded to round up.

; CHECK:  Section {
; CHECK:    Type: CUSTOM (0x0)
; CHECK:    Size: 4
; CHECK:    Offset: 97
; CHECK:    Name: __clangast
; CHECK:  }
; CHECK:  Section {
; CHECK:    Type: CUSTOM (0x0)
; CHECK:    Size: 1
; CHECK:    Offset: 124
; CHECK:    Name: anchor
; CHECK:  }