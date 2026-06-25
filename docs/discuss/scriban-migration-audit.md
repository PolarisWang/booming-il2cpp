# Scriban 模板化迁移审计

## 现有模板使用情况

160 个 Scriban 模板，覆盖：
- NativeAot 全系列 (CodeRegistration, DispatchEntry, VTableArray, ModuleRegistration 等)
- ReferenceProof 全系列
- RuntimeSupport 系列
- BuildSystem 系列 (cmake)

## 硬编码候选

`EmitObjectModelDeclarations` 中仍然硬编码的 C++ 生成：
1. TypeInfoV0 struct 定义 (~400 行)
2. VTable 数组定义 (~200 行)
3. IfaceMap 数组定义 (~150 行)
4. GcTypeLayout 数组定义 (~100 行)

这些已经使用 StringBuilder + AppendInterpolatedStringHandler，模式稳定。
迁移到 Scriban 的主要收益是可维护性（C++ 代码更易读），而非性能。

## 结论

当前 StringBuilder 硬编码模式已足够优化。
Scriban 迁移仅建议在有新 C++ 模板代码添加时逐步进行，不建议批量迁移。
