# T0-1: type_registry API 单元测试 — 实现计划

> **面向执行 Agent：** 必须使用 dev-executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。执行完成时必须先归档当前子任务目录（移入 `docs/dev/completed/`），再同步父 roadmap。

**目标：** 补齐 `type_registry` API 的隔离单元测试，覆盖 `chaos_register_type()`、`chaos_find_type_by_stable_id()`、`ChaosTypeAddInterface()` 三条核心 API。

**架构：** 使用 GTest 框架 + 内存 domain 上下文。测试链接到 `chaos_runtime_core`（含 memory_domain、vtable_registry 等依赖），通过 `RegisterMemoryDomain` + `DomainScope` 设置分配上下文。

**技术栈：** Google Test, C++20, CMake

**架构审核模式：** normal

**结构告警重点：** 无（纯新增测试文件，不修改既有逻辑）

**权责图审核主题：** n/a

**AOT/IL2CPP/Test Governance Intake：** n/a

**设计文档：** n/a（来自父 roadmap brainstorm-v1-01.md）

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"测试通过 → 归档 completed → 合并&提交"固定链路。

---

## 文件清单

### 新建文件
1. `testing/src/native/runtime-core/type_registry_test.cpp` — 测试源文件

### 修改文件
2. `testing/src/native/runtime-core/CMakeLists.txt` — 新增 `chaos_type_registry_test` 测试目标

## 测试架构

### Fixture 设计

```
type_registry_test.cpp:
  ┌─────────────────────────────────────────┐
  │ TypeRegistryTest : public ::testing::Test │
  │   SetUp():                              │
  │     - RegisterMemoryDomain()            │
  │     - PushDomain() / DomainScope        │
  │   TearDown():                          │
  │     - PopDomain()                      │
  │     - UnregisterMemoryDomain()         │
  └─────────────────────────────────────────┘
```

### 测试用例矩阵

| # | 测试名 | 覆盖场景 | 验证点 |
|---|--------|---------|--------|
| 1 | `RegisterAndFind` | 注册一个引用类型，按 stable_id 查找 | 返回非空、stable_id 匹配、type_shape 正确 |
| 2 | `RegisterMultipleTypes` | 注册多个不同名的类型 | 所有类型都能按各自的 stable_id 找到 |
| 3 | `FindNonExistent` | 查找不存在的 stable_id | 返回 nullptr |
| 4 | `RegisterNullName` | name 参数传 nullptr | 返回 nullptr, out_stable_id=0 |
| 5 | `RegisterDuplicateName` | 相同 name 注册两次 | 第二次成功（不同 stable_id 不可能，但 registry 允许多次插入）|
| 6 | `RegisterWithParent` | 注册时指定 parent MethodTable | stable_id 正确、parent_mt 指向传入值 |
| 7 | `RegisterWithInterfaceMap` | 注册时传入 iface_map | iface_count、iface_map 正确 |
| 8 | `AddInterface` | 注册后追加接口映射 | runtime_iface_count 增加、runtime_iface_map 包含新接口 |
| 9 | `AddInterfaceIdempotent` | 相同接口追加两次 | 第二次返回 true、runtime_iface_count 不重复增加 |
| 10 | `AddInterfacePreMapped` | 追加 AOT iface_map 中已存在的接口 | 返回 true、不重复追加 |
| 11 | `AddInterfaceNullMT` | mt 传 nullptr | 返回 false |
| 12 | `RegisterUntilFull` | 填充到 256 上限 | 第 257 次返回 nullptr |

## 实现步骤

### Step 1: 创建测试源文件

创建 `testing/src/native/runtime-core/type_registry_test.cpp`，包含：
- `#include` 头文件链（gtest, type_registry.h, memory_domain.h, type_info.h）
- `TypeRegistryTest` fixture class（SetUp/TearDown 管理 domain 生命周期）
- 全部 12 个测试用例
- 辅助函数：`MakeMethodTable()`（构造最小 MethodTable 用于 parent 参数）、`MakeInterfaceMap()`（构造接口映射数组）

### Step 2: 修改 CMakeLists.txt

在 `testing/src/native/runtime-core/CMakeLists.txt` 添加：

```cmake
add_chaos_test(type_registry_test type_registry_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)
```

### Step 3: 构建并验证

```bash
cmake --build build/testing --target type_registry_test
cd build/testing && ctest -R type_registry_test --output-on-failure
```

### Step 4: 收尾

- 确认全部测试通过
- 将子任务目录移入 `docs/dev/completed/`
- 提交代码
