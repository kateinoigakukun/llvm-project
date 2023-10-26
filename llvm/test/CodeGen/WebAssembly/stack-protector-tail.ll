; RUN: llc %s -o - | FileCheck %s

target triple = "wasm32-unknown-unknown"

define void @test() sspreq "target-features"="+tail-call" {
  tail call void @callee()
  ret void
}

declare hidden void @callee()
