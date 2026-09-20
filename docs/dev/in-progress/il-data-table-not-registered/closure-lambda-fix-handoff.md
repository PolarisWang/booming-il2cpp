# 交接：闭包无 body 修复（lambda 误伤）→ worktree 执行

> **交接日期**: 2026-09-20
> **交接人**: main 会话
> **目标**: `closure-body-rootcause.md` 中已定位但未验证的修复
> **执行方式**: **worktree**（主树有并发 WIP，不可直接跑 pipeline）

## 〇、一句话结论

**`MethodEmission.cs:169` 的 `Contains("<>c::")` 一刀切，把 12 个 lambda
误发成 fallback stub**。修复 = 收窄判定（排除 `b__` lambda）。
**已写、已编译通过、未验证、已从主树剥出** —— 待你在干净 worktree 验证。

## 一、根因（已验证，见 `closure-body-rootcause.md`）

```
method.SubjectId = "...<>c::<EnsureInitialized_3_int_bool_object_System_Funcint_3>b__14_0:System.Int32()"
规则: Contains("<>c::") → 命中 → 发 fallback stub
正目标: 6 个 display-class .cctor/.ctor（newobj 结构化发射器无法降低）
误伤:   12 个 lambda b__N（只是 ldc.i4 0; ret 的平凡 IL，可翻译）
```

实测（threading chunk 生成物）：12/12 lambda 全落 fallback stub，
运行时走 Phase 3 `NotImplementedException`。

## 二、修复内容（patch 已导出）

**patch 文件**: `/tmp/handoff_lambda_fix.patch`（48 行，`git diff` 格式）
**备份**: `/tmp/handoff_lambda_fix.cs`（完整文件）

```bash
# 在 worktree 里应用
git apply /tmp/handoff_lambda_fix.patch    # 或 cp /tmp/handoff_lambda_fix.cs <目标文件>
```

改动要点：
1. `EmitManagedMethod` 的判定从 `Contains("<>c::")` 改为
   `Contains("<>c::") && !IsLambdaBodySubjectId(...)`
2. 新增 `IsLambdaBodySubjectId()`：按 `<>c::` 之后的 `b__` 区分 lambda 与构造器

**Generator + TPG 已编译通过（0 error）**，但**未跑管线验证**。

## 三、⚠️ 为什么必须在 worktree 做 —— 主树阻塞

### 3.1 并发 WIP（`1c830ad24`）导致 threading 无法跑 fact

`entry.exe --fact-json` 会在 **si=218**（
`ReaderWriterLockSlim::TryEnterUpgradeableReadLock`）**挂死**
（忙等：输出停滞但 CPU 持续涨 663s→969s）。

**挂死链**：
```
test → SubjectInstanceFactory.Create<ReaderWriterLockSlim>()
     → GetUninitializedObject()
     → LookupTypeNameByInfoPtr 返回 null   ← WIP `1c830ad24` 的 bug
     → 零初始化对象被当绑定句柄 → 死等
```

**证据**：隔离实验（回退自改、保留他改）两次同位置挂死，
最后输出均 `[GUO] resolved type_name=(null)`。
**与我们的 lambda 修复无关**（memory: `concurrent-wip-clobbers-attribution`）。

### 3.2 主树当前还有两处对方未提交改动

| 文件 | 状态 |
|---|---|
| `ExceptionEmission.Linear.cs` | XmlTextWriter 临时诊断桩 |
| `RuntimeHelperShapeRegistry.CoreStubs.Part1.S19.cs` | XmlTextWriter ctor ABI 修正 |

**不要在 main 工作树碰它们**；用 worktree 隔离。

## 四、验证步骤（worktree 内）

### 前置检查（memory 血泪）
```bash
tasklist | grep -iE "MSBuild|cl\.exe|python"    # 无并发
git log -1 --format='%h %ad %s'                  # base = 3f6f1c9a5 起
worktree 是否基于干净 commit                      # 不含 1c830ad24 的 WIP
```

### 步骤
```bash
# 1. 建 worktree（基于 3f6f1c9a5 之后 / 或当前 main 但独占）
git worktree add -b attempt-closure-lambda <base> .claude/worktrees/closure-lambda

# 2. 应用修复（二选一）
cd .claude/worktrees/closure-lambda
cp /tmp/handoff_lambda_fix.cs \
   src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs

# 3. 重建 TPG（Debug！pipeline 用这个）
dotnet build src/managed/Chaos.IL2CPP.Generator/Chaos.IL2CPP.Generator.csproj -c Debug
dotnet build src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Chaos.IL2CPP.Tools.TestProjectGenerator.csproj -c Debug

# 4. 跑 threading pipeline
CHAOS_FOUNDATION_DLL=D:/agent/chaos-il2cpp/testing/foundation-dll \
PYTHONPATH=D:/agent/chaos-il2cpp/tests/e2e \
python tests/e2e/verification/chunk_pipeline.py \
  --assembly System.Private.CoreLib --chunk threading --stages build,fact
```

### 验收标准
1. **build 成功，entry.exe mtime 晚于生成代码**（防陈旧复用）
2. `failed` 应从 8 下降：目标把 `EnsureInitialized_3`、
   `QueueUserWorkItem×2`、SpinWait 相关拿下
   （⚠️ 这些还各自有第二道闸，见下）
3. `.cctor/.ctor` 仍走 fallback（防回归）—— 抽查生成代码
4. lambda 转真实 body（不再是 `ChaosExternalRuntimeFallback` stub）

### ⚠️ 期望管理（部分转绿的物理边界）
| 目标项目 | 第一道闸 | 第二道闸 |
|---|---|---|
| `EnsureInitialized_3`（4 参） | ✅ lambda 开闸后闭包可执行 | —— 若还失败，查闭包 `b__14_0` 本身 |
| `QueueUserWorkItem×2` | lambda b__23/24/28/29 | **3 参重载本身无 native**（`managed_primitive_entries.h` 只导出 2 参）→ 可能仍失败，属诚实边界 |
| SpinWait/SyncLock 族 | lambda b__8/9/14/15/20/21 | 可能仍失败，需实测 |

**不要为了凑绿写占位实现** —— fake-pass 戒律。

## 五、参考 material

| 项 | 位置 |
|---|---|
| 根因全 doc | `docs/dev/in-progress/il-data-table-not-registered/closure-body-rootcause.md` |
| 前置立项 | 同目录 `STATUS.md` 第八节（定量 12/12） |
| 归因实验 | doc 第四节（隔离实验表） |
| memory | `concurrent-wip-clobbers-attribution` / `tpg-bundles-stale-generator-dll` |
| 同类先例 | `discriminator-lookup-key-never-matched`（查表键格式不符，同族第五次） |

## 六、交接后状态确认

- ✅ 主树已剥出本修复（`git checkout` 回退，工作树只剩对方两处改动）
- ✅ patch 在 `/tmp/handoff_lambda_fix.patch`
- ✅ 文档已提交（`3f6f1c9a5`）
- ⏳ **本修复未提交** —— 待你在 worktree 验证后提交，commit 需含
  `root_cause` / `fix_strategy` / `regression_check` 三段
- ⏳ 同批可顺手验证：`Interlocked`（批次 1）与 `LazyInitializer`（批次 2）
  的既有结果在干净树是否稳定（它们修的是 shape 层，不受 GUO WIP 影响，
  但 threading 全量基线需要干净树确认）