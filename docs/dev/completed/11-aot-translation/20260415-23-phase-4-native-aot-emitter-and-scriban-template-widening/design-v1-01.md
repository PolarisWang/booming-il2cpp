# Phase 4 Native AOT Emitter And Scriban Template Widening Design v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## 1. 目标

把 widened lowering plan 真正落到 `NativeAotEmitter + Scriban`，确保不回退到内联字符串拼接。

## 2. 设计决策

- `NativeAotEmitter` 只组织 model，不直接拼装 widened native shape。
- 所有新增 native AOT 输出统一经过 `Templates/NativeAot*.scriban`。

## 3. 实际落点

- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot.TranslationUnit.cpp.scriban`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot.ObjectModel.cpp.scriban`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeAot.Method.cpp.scriban`

## 4. 完成定义

- widened plan model 经过 Scriban 稳定输出。
- artifact-level 测试能够锁定 emitter/template 的关键字段与输出形状。
