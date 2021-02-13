; RUN: llvm-mc -filetype=obj -triple=wasm32-unknown-unknown %p/Inputs/hello.s -o %t.hello32.o
; RUN: llc -mtriple=wasm32-unknown-unknown -filetype=obj %s -o %t.o
; RUN: wasm-ld --no-entry --no-gc-sections --allow-undefined -o %t.wasm %t.o %t.hello32.o
; RUN: obj2yaml %t.wasm | FileCheck %s

target triple = "wasm32-unknown-unknown"


; @hello_str - @bar_neg
@hello_str = external global i8*
@bar_neg = constant i32 sub (
    i32 ptrtoint (i8** @hello_str to i32),
    i32 ptrtoint (i32* @bar_neg to i32)
), section ".sec1"

@bar_pos = constant i32 sub (
    i32 ptrtoint (i32* @fizz to i32),
    i32 ptrtoint (i32* @bar_pos to i32)
), section ".sec1"


; @hello_str - @addend + 4
@fizz = constant i32 42, align 4, section ".sec2"
@addend = constant i32 sub (
    i32 ptrtoint (i8** @hello_str to i32),
    i32 ptrtoint (i32* @fizz to i32)
), section ".sec2"


; CHECK:      - Type:            DATA
; CHECK-NEXT:    Segments:
; CHECK-NEXT:      - SectionOffset:   7
; CHECK-NEXT:        InitFlags:       0
; CHECK-NEXT:        Offset:
; CHECK-NEXT:          Opcode:          I32_CONST
; CHECK-NEXT:          Value:           1024
; CHECK-NEXT:        Content:         68656C6C6F0A00
; CHECK-NEXT:      - SectionOffset:   20
; CHECK-NEXT:        InitFlags:       0
; CHECK-NEXT:        Offset:
; CHECK-NEXT:          Opcode:          I32_CONST
; CHECK-NEXT:          Value:           1032
; CHECK-NEXT:        Content:         F8FFFFFF04000000
; CHECK-NEXT:      - SectionOffset:   34
; CHECK-NEXT:        InitFlags:       0
; CHECK-NEXT:        Offset:
; CHECK-NEXT:          Opcode:          I32_CONST
; CHECK-NEXT:          Value:           1040
; CHECK-NEXT:        Content:         2A000000F0FFFFFF

