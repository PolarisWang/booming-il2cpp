# Progress v1-04: Batch 4 完成

**时间**: 2026-04-12
**Batch**: 4 — vtable 虚方法分派

## 产出

- `src/native/runtime-core/vtable_registry.h` — 新增，TypeVTable / VTableSlot 结构 + RegisterTypeVTable / ResolveVirtualMethodPointer
- `src/native/runtime-core/vtable_registry.cpp` — 新增，按 type_token 索引 + 继承链向上查找
- `src/native/runtime-core/CMakeLists.txt` — 添加 vtable_registry.cpp
- `src/native/bootstrap/bootstrap.cpp` — include vtable_registry.h；ResolveVirtualMethod 先查 vtable registry、再查 UnresolvedVirtualCallEntry；IsKnownResolvedVirtualHandle 接受大地址函数指针
- `subjects/VTableDispatchProof/` — 新增 subject (Animal→Dog/Cat abstract class 多态)

## 注意

- vtable_registry 是新增基础设施，现有 UnresolvedVirtualCallEntry 机制保留不变
- 生成的 C++ vtable 初始化代码需要 managed pipeline 支持 (codegen 扩展留在 A.11)
- VTableDispatchProof subject 结构已创建，需要运行 managed pipeline 生成 artifacts 才能端到端测试
- 现有 InterfaceDispatchProof subject 已覆盖虚分派验证

## 验证结果

- cmake 构建成功 ✓
- 所有 5 个 GC smoke tests 通过 ✓
- C4819 警告 (UTF-8 注释)，非错误

## 下一步

Batch 5 — 完整异常处理 + 泛型运行时
