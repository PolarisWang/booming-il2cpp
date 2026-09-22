# aot-exception-type-resolution

文件系统异常类型解析链路 — 现状与结案

> 最近更新：2026-09-22
> 本任务目录下的文件编号对应同一个根因链（见 `root-cause-complete.md`）。

---

## 根因链（4 层，逐层实测）

| 层 | 现象 | 状态 |
|----|------|------|
| **层 1** | 异常类型 MethodTable parent = nullptr（`ObjectModelEmission.cs:879-884` 的 parent 只认 instruction TargetReference，异常类型从未作为 TargetReference 出现） | ✅ **已修**（`GetSyntheticReferenceTypeBaseSubjectId` + `Pass 1b` 补 catch 类型完整基类链） |
| **层 2** | 反射镜像只有 16 个类型且全是 Sdk（`Methods.ModuleData.cs:47-49` 只扫当前 assembly） | ❌ **未修** |
| **层 3** | `ResolveTypeByName` 因镜像缺失返回 0 | ❌ 未修 |
| **层 4** | `RaiseManagedException` 抛 null 异常对象 → `chaos_eh_match_type` 必失败 | ❌ 未修 |

层 2-4 未修 ⇒ factoryGap 仍为 62 项。

## 最近修复（layer 1 + Path 1）

- **`b3d793b17`**：`ResolveObjectTypeInfo` Path 1（tag 句柄）补 `type_info` 赋值 —— 从 `desc->type_info_ptr`
  读取**静态 MethodTable**（`&chaos_mt_*`），tag 句柄不再返回 nullptr，异常路径不再经
  `RaiseManagedException → std::abort() → SIGABRT`。
- **2026-09-22 review 处置**：7 条 review 中 3 条高/中 severity 全部误报；落地了 4 条有效项的注释/log 修正，
  并把误报模式固化为 review 流程门槛。详见 [`review-false-positives-2026-09-22.md`](review-false-positives-2026-09-22.md)。

## 遗留验证缺口

- **无 Path 1 回归 fact**：`b3d793b17` 的 `regression_check` 仅「text-json build 阶段通过」——
  **只编译通过，未验证异常路径运行时行为**（编译期成立 ≠ 运行时等价）。若要做，B 项需一个能构造
  `ResolveTypeByName → ObjectNew` → 走 Path 1 → `stable_id != 0` 的 fact。

## 下一步

1. （可选）补 Path 1 回归 fact，把 `b3d793b17` 的修复锁进验证链。
2. 层 2-4 的修法在 `root-cause-complete.md` / `layer3-design.md`（反射镜像收录 exception 类型，
   需先在 `Methods.ModuleData.cs` 放开非当前 assembly 扫描或提供兜底镜像）。

---

## 架构映射

| 域 | 涉及组件 | 本次改动 |
|----|---------|---------|
| **CodeGen** | `NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs`（BuildExceptionTypeTable）、`ObjectModelEmission.cs`（mt parent）、`Methods.ModuleData.cs`（镜像收集，未修） | 层 1 基类链 |
| **运行时** | `core/object_creation.cpp`（Path 1 type_info）、`exception_helpers.cpp`（ResolveTypeByName）、`instantiation/`（RuntimeInstantiatedType） | Path 1 补 type_info + 注释/log |
| **验证** | `aot-exception-type-resolution/` 已有 `root-cause-complete.md`、`factorygap-rootcause-2026-09-21.md` | review-false-positives-2026-09-22.md |