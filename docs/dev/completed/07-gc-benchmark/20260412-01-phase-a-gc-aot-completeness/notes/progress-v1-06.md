# Progress v1-06: Batch 6 完成

**时间**: 2026-04-12
**Batch**: 6 — 委托多播 + IL opcode 扩展

## 产出

### 委托多播
- `bootstrap.cpp`:
  - `DelegateInstance` 添加 `next` + `invocation_count` 字段
  - `CombineDelegate(left, right)` — 克隆链 + 追加，O(n)
  - `RemoveDelegate(source, target)` — 移除最后一个匹配节点
  - `CloneChain` / `RebuildCounts` / `DelegateNodesEqual` 内部辅助
  - `DelegateInvoke` 修改为遍历 multicast chain，最后返回值
  - `ResolveIcall` 新增 `Delegate::Combine` / `Delegate::Remove` icall 映射
- `subjects/DelegateChainProof/` — 4 场景: 单播, 三播, remove, 置空

### IL opcode proof subjects
- `subjects/ConversionOpsProof/` — conv.i1/i2/i4/i8/u1/u2/r4/r8
- `subjects/BitwiseOpsProof/` — and/or/xor/shl/shr/shr.un/not
- `subjects/BranchOpsProof/` — beq/bne/blt/bgt/ble/bge + unsigned
- `subjects/ObjectOpsProof/` — newobj/isinst/castclass/interface dispatch
- `subjects/ArrayOpsProof/` — int[]/string[]/double[] 数组操作
- `subjects/OverflowOpsProof/` — add.ovf/sub.ovf/mul.ovf/conv.ovf

## 验证结果

- cmake 构建成功，无 error ✓
- 全量 GC tests (5/5) 通过 ✓
- dotnet build Chaos.IL2CPP.Interpreter: 0 warnings 0 errors ✓

## 注意

- opcode proof subjects 需要运行 managed pipeline 生成 artifacts 后才能端到端验证
- IL opcode 扩展 (ILToIRLowering + codegen templates) 放在 A.11，依赖 vtable/EH/generic 完成

## 所有 Batch 完成，进入 Phase A 退出验证
