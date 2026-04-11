# Shared Contract Freeze 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 冻结 AOT 主线与热更运行模式共享的底层 contract（identity、object model、handle、ABI、exception、delegate、metadata token、version policy、interpreter IR 决策、package manifest），为后续所有 Phase 提供唯一参考基线。

**架构：** 本阶段以文档 + schema + 验证测试为主。每个 contract 产出一份 spec 文档（markdown），关键 contract 同时产出 JSON schema 或 C header 样例。所有 contract 通过 snapshot test 验证一致性，并通过双边 mock 验证 AOT 与热更可互操作。

**技术栈：** Markdown, JSON Schema, C headers, Python (tests)

**设计文档：** `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md` (Phase 0 节)

**预期知识沉淀：** `wiki/` — 新增 contract 参考页

---

## 要创建/修改的文件

### 新建文件

| 文件路径 | 职责 |
| --- | --- |
| `contracts/shared/v0/identity-model.md` | SubjectId 格式规范、泛型编码、嵌套类型、跨版本 stable token |
| `contracts/shared/v0/object-model.md` | ObjectHeader 布局、GC bits、type slot、sync block、string/array 布局 |
| `contracts/shared/v0/handle-model.md` | handle 类型清单、不透明约束、生命周期规则 |
| `contracts/shared/v0/abi-calling-convention.md` | EXPORT/CALL 宏、参数顺序、返回值、错误码、struct_size 约定 |
| `contracts/shared/v0/exception-boundary.md` | out_exception 模式、managed/native EH 切换点、finally 语义 |
| `contracts/shared/v0/delegate-abi.md` | delegate 内存布局、函数指针字段、method_token、target、invoke wrapper |
| `contracts/shared/v0/metadata-token-mapping.md` | PE token → stable identity 映射策略、跨热更版本引用规则 |
| `contracts/shared/v0/version-policy.md` | formatVersion 策略、struct_size 向前兼容、schema 演进规则 |
| `contracts/shared/v0/interpreter-ir-decision.md` | IR vs raw IL 架构决策记录、IR 指令集初步设计、与 TypedIL 的关系 |
| `contracts/shared/v0/package-manifest.schema.json` | hot update package manifest v0 JSON schema |
| `contracts/shared/v0/README.md` | shared contract 索引页 |
| `tests/contracts/shared/test_identity_model.py` | identity 格式 snapshot 测试 |
| `tests/contracts/shared/test_package_manifest_schema.py` | package manifest schema validation |
| `tests/contracts/shared/test_dual_mode_identity.py` | AOT + hot-update mock 双边互操作测试 |
| `tests/contracts/shared/__init__.py` | test package init |
| `subjects/SharedContractProof/source/SharedContractProof.csproj` | 最小 C# 验证程序 |
| `subjects/SharedContractProof/source/Program.cs` | 入口：验证 identity 在 AOT 侧可用 |
| `subjects/SharedContractProof/subject.manifest.json` | subject manifest |

### 修改文件

| 文件路径 | 修改内容 |
| --- | --- |
| `tests/unit/run/test_repo_layout.py` | 新增 `test_shared_contracts_directory_exists()` enforcement |
| `docs/dev/in-progress/20260411-01-shared-contract-freeze/STATUS.md` | 执行过程中持续更新 |

---

## 步骤

### Batch 1: Identity Model + Object Model（核心基础）

- [ ] **1.1** 创建 `contracts/shared/v0/` 目录和 `README.md`
- [ ] **1.2** 编写 `identity-model.md`：
  - 冻结 `ManagedNaming` 中已有的 SubjectId 格式（`{Assembly}/{Namespace}.{Type}::{Member}`）
  - 冻结泛型编码规则（`StripGenericArity` + `<TypeArg1,TypeArg2>`）
  - 冻结嵌套类型规则（`Outer+Inner` → `{Type}+{Inner}`）
  - 定义 stable token 策略（PE metadata token 在同版本 assembly 内稳定；跨版本使用 SubjectId 做 stable identity）
  - 定义 hot-update 跨版本引用规则（热更代码通过 SubjectId 而非 token 引用 AOT 类型）
- [ ] **1.3** 编写 `object-model.md`：
  - 冻结 ObjectHeader 布局（从 `runtime_core.cpp` 提取当前实际布局）
  - 冻结 StringObject 布局（length + UTF-16 data）
  - 冻结 ArrayObject 布局（length + element data）
  - 冻结 BoxedValue 布局（header + inline value）
  - 定义 GC bits 位置（header 中的标志位）
  - 定义 type info slot（指向 TypeInfoHandle 的指针）

### Batch 2: Handle Model + ABI Calling Convention

- [ ] **2.1** 编写 `handle-model.md`：
  - 冻结 handle 类型清单（从 `runtime_abi.h` 提取：TypeInfoHandle, MethodInfoHandle, FieldInfoHandle, PropertyInfoHandle, EventInfoHandle, ParameterInfoHandle, GenericContextHandle, AssemblyHandle, ImageHandle, ExceptionHandle）
  - 定义不透明约束（handle 不可解引用、不可序列化、不可跨进程）
  - 定义生命周期规则（handle 在 runtime shutdown 后失效）
- [ ] **2.2** 编写 `abi-calling-convention.md`：
  - 冻结 `CHAOS_RUNTIME_ABI_EXPORT` / `CHAOS_RUNTIME_ABI_CALL` 宏语义
  - 冻结参数顺序（RuntimeState*, ThreadState*, ..., out_exception*）
  - 冻结返回值约定（成功返回值或 NULL + out_exception）
  - 冻结 RuntimeStatus / BridgeStatus 枚举值
  - 冻结 `struct_size` 向前兼容约定（新版本可追加字段，旧版本通过 struct_size 判断可用字段数）

### Batch 3: Exception + Delegate + Token Mapping

- [ ] **3.1** 编写 `exception-boundary.md`：
  - 冻结 out_exception 传递模式
  - 定义 managed → native 异常边界（raise_managed_exception）
  - 定义 native → managed 异常边界（method_invoke 的 out_exception）
  - 定义 finally/fault 执行保证
- [ ] **3.2** 编写 `delegate-abi.md`：
  - 冻结 delegate 内存布局（从 `bootstrap.cpp` 提取 DelegateInstance 结构）
  - 冻结 create_delegate / delegate_invoke 语义
  - 定义 AOT delegate → hot-update target 的桥接规则
- [ ] **3.3** 编写 `metadata-token-mapping.md`：
  - 定义 PE token 的使用范围（同版本 assembly 内有效）
  - 定义 SubjectId 作为跨版本 stable identity
  - 定义热更包引用 AOT 类型/方法的查找策略（SubjectId → token 映射表）

### Batch 4: Version Policy + IR Decision + Package Manifest

- [ ] **4.1** 编写 `version-policy.md`：
  - 冻结 `formatVersion: "v0"` 语义
  - 定义 v0 允许的变更（追加字段）和禁止的变更（删除/重命名字段）
  - 定义 struct_size 兼容检查规则
  - 定义 schema 演进策略（v0 → v1 的升级条件）
- [ ] **4.2** 编写 `interpreter-ir-decision.md`：
  - 记录架构决策：选择选项 B（IL → 内部 IR → 解释执行）
  - 定义 IR 与 TypedIL 的关系（IR 是 TypedIL 的可执行变体，共享 identity 但指令集不同）
  - 列出 IR 最小指令集（~30 条：ldc, ldloc, stloc, ldarg, ldfld, stfld, call, callvirt, br, ret, newobj, box, newarr, throw, ...）
  - 定义 IR 操作数编码（typed operand stack，不用 evaluation stack 模拟）
  - 明确 Phase 7 的 IR 实现范围
- [ ] **4.3** 创建 `package-manifest.schema.json`：
  - 定义 hot update package manifest v0 schema
  - 包含 formatVersion, packageId, targetAotVersion, assemblies[], supplementalMetadata, signature

### Batch 5: 测试 + Subject + Enforcement

- [ ] **5.1** 创建 `tests/contracts/shared/__init__.py`
- [ ] **5.2** 编写 `test_identity_model.py`：
  - 验证现有 10 个 subject 的 `source.entry` 字段符合冻结格式
  - 验证 `ManagedNaming` 的所有 Create* 方法输出符合 spec
- [ ] **5.3** 编写 `test_package_manifest_schema.py`：
  - 生成合法/非法 package manifest 样例
  - 验证 schema validation 通过/拒绝
- [ ] **5.4** 编写 `test_dual_mode_identity.py`：
  - 模拟 AOT 侧用 SubjectId 注册一组类型
  - 模拟 hot-update 侧用 SubjectId 查询相同类型
  - 验证双边 identity 匹配
- [ ] **5.5** 创建 `subjects/SharedContractProof/` subject：
  - 最小 C# 程序（定义一个类 + 一个方法）
  - subject.manifest.json 正确使用 identity 格式
- [ ] **5.6** 在 `test_repo_layout.py` 新增 `test_shared_contracts_directory_exists()` enforcement
- [ ] **5.7** 运行全部 contract 测试：`python -m pytest tests/contracts/shared/ -v`
- [ ] **5.8** 运行 repo layout 测试：`python -m pytest tests/unit/run/test_repo_layout.py -v`

### Batch 6: 验证与完成

- [ ] **6.1** 检查所有 11 份 spec 文档内部一致性
- [ ] **6.2** 验证 `contracts/native/v0/runtime_abi.h` 中的 handle 类型与 `handle-model.md` 一致
- [ ] **6.3** 验证 `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs` 中的 ManagedNaming 与 `identity-model.md` 一致
- [ ] **6.4** 运行完整项目测试套件
- [ ] **6.5** 更新 STATUS.md，标记完成
- [ ] **6.6** 更新父 roadmap STATUS.md

---

## 验证命令

```bash
# contract 测试
python -m pytest tests/contracts/shared/ -v

# repo layout enforcement
python -m pytest tests/unit/run/test_repo_layout.py -v

# 完整测试套件
python -m pytest tests/unit/run/ tests/tooling/run/ -q
```

## 执行时维护

- 每完成一个 Batch，更新 `STATUS.md` 的最近摘要和下一步
- 每完成一个 Batch，追加 `notes/progress-v1-XX.md`
- Batch 5 完成后更新 `docs/dev/ACTIVE.md`
- 全部完成后移动到 `docs/dev/completed/`
