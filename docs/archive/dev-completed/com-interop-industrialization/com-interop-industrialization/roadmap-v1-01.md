# COM Interop 工业化补齐 — Roadmap

## 目标

对标 Memory Domain / Reflection / HotUpdate 的工业化完成度，补齐 COM Interop 剩余差距。

## 阶段

### Phase 1: CCW Phase2 升级

**goal**: 用 com_ccw.cpp.phase2 替换当前 com_ccw.cpp，消除 stub

**exit_criteria**: com_ccw.cpp 编译通过，CcwGetTypeInfoCount/CcwGetTypeInfo 真实实现

**deliverables**: 更新后的 com_ccw.cpp

**dependencies**: 无

### Phase 2: 测试构建 & 验证

**goal**: 构建所有 COM 测试并运行通过

**exit_criteria**: 10 个 com_* 测试全部通过

**deliverables**: 测试运行日志

**dependencies**: Phase 1

### Phase 3: Managed COM AOT 集成测试

**goal**: 创建 managed COM interop AOT 端到端测试

**exit_criteria**: AOT 翻译、编译、运行通过

**deliverables**: managed 测试文件

**dependencies**: Phase 2

## 子任务映射

| task_id | phase | status | owner | purpose | deliverables | exit_criteria |
|---------|-------|--------|-------|---------|-------------|---------------|
| P1-ccw-phase2 | Phase 1 | planned | main | CCW phase2 升级 | com_ccw.cpp | 编译通过，GetTypeInfoCount/GetTypeInfo 真实 |
| P2-test-verify | Phase 2 | planned | main | 构建并运行所有 COM 测试 | 测试日志 | com_* 测试全部通过 |
| P3-managed-test | Phase 3 | planned | main | Managed COM AOT 测试 | managed 测试文件 | AOT e2e 通过 |

## 依赖

- Phase 1 → Phase 2 → Phase 3 串行

## 风险

- Managed COM AOT 测试需要 il2cpp translation 管线可用（已验证）
- CCW phase2 合并时可能丢失 address-distance sanity check
