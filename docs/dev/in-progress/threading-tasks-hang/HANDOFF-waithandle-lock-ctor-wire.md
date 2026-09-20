# 交接 — WaitHandle 静态重载（分支 `lock-ctor-wire`）

> 交接日期：2026-09-20
> 分支：`lock-ctor-wire`（base `d53d4cc74`，1 个提交 `b4316f27d`）
> worktree：`.claude/worktrees/lock-ctor-wire`
> 状态：**部分交付，未合入 main**（用户标准：全部完成 + 测试通过才合）

## 一、一句话

native 校验实现与 7 个契约测试**已验证正确并保留**；
**shape 注册与 ATG 白名单已回退** —— 前者只匹配 6/31，后者制造 22 个 failed。
**接手者的第一站是查清 resolver 为何未被调用。**

## 二、保留了什么（自包含，不改变现有行为）

| 组件 | 位置 |
|---|---|
| `chaos_wait_handle_validate` / `_pair` | `runtime_stubs/threading_stubs.{h,cpp}` |
| 7 个契约测试 | `tests/contracts/native/runtime-core/waithandle_validate_contract_test.cpp` |
| 完整分类调查 | `docs/dev/in-progress/threading-tasks-hang/stubgap-classification-2026-09-20.md` |
| .NET 实测契约表 | `docs/dev/in-progress/threading-tasks-hang/waithandle-contracts.md` |
| **失败记录（必读）** | `docs/dev/in-progress/threading-tasks-hang/STATUS-waithandle-native.md` §六 |

**验证状态**：7 个契约测试在提交后复跑 **7/7 全绿**；Generator `dotnet build` 成功。

## 三、没交付什么（以及为什么）

### 3.1 shape 注册错了 4 次

| # | 假设 | 结果 |
|---|---|---|
| 1 | `registry.Register(...)` 会 emit helper 源码 | ❌ C3861 未定义 |
| 2 | `RegisterGeneric` + 按 C# 声明顺序 | ❌ 不匹配 → 走 catch-all |
| 3 | 从 mangled symbol 名解码参数 | ❌ **mangled 名不是签名** |
| 4 | 从 `GetMethodParameterTypesFromSubjectId` 实测读 | ⚠️ 只匹配 **6/31** |

**核心未解之谜**：第 4 次插在 resolver 第一行的 `WH_DEBUG` 打印
**一次都没触发** → resolver 根本没被调用。**原因未查清。**

### 3.2 ATG 白名单有害（已回退）

单独加白名单、shape 未匹配 → AOT 走 `chaos_external_runtime_*` catch-all
返回 0 不抛 → ATG 生成的 `"AOT stub did not throw"` 触发：

| | 基线 | 加白名单后 |
|---|---:|---:|
| stubGap | 321 | 267 |
| **failed** | **21** | **43** |

**把诚实 gap 换成了假红。**

## 四、接手者建议起点

1. **先查 `WH_DEBUG` 为何不触发** —— 在 `TryMatchGenericShape`
   （`RuntimeHelperShapeRegistry.cs:357`）的 entry 循环入口加打印，确认
   descriptor 是否被遍历到。
2. **确认 `typeDisplayName` 的实际值** —— 行 388 的 `StartsWith` 判定用它，
   **不是** subject id 前缀。这是第 2/3/4 次都没验证的变量。
3. **shape 先能匹配，再动 ATG** —— 否则重演 §3.2。
4. **验收口径**：`realVerified` 上升 **且** `failed` 不上升。
   不要只看 stubGap 下降 —— 它可以靠制造 failed 来"下降"。

## 五、上游背景（方向收敛过程）

本任务的方向经三轮实测推翻后收敛，过程记录在
`stubgap-classification-2026-09-20.md`：

| 原假设 | 实测 |
|---|---|
| 321 stubGap 集中锁类、共享根因 | ❌ 321 个不同方法；锁类仅 7 个 |
| 改 ATG 白名单可覆盖 206 个 | ❌ 多数方法连 native stub 都没有 |
| 7 个 SyncLock 案例可立刻转绿 | ❌ native 只返回 0，不抛异常 |

**结论**：`AOT-STUB-GAP` 是诚实的，标记的是「缺 native 实现」。
