# Roadmap: CRT Fix + Codegen Tests + Foundation-DLL Expansion

## 目标

本 roadmap 覆盖三个独立工作流，分批推进：

- **Phase A**: 修复 CRT `_CrtDbgReport` linker error，解锁 15+ 个被阻塞的 native 测试目标
- **Phase B-1**: 为 codegen 模块补充基础测试（IL smoke + ABI contract）
- **Phase B-2**: 扩展 foundation-DLL 验证到下一个能力族

## 范围边界

- 仅涉及 testing/ 目录下的 native C++ 测试
- foundation-DLL 扩展只选一个 family
- 不涉及 managed codegen 修改

## 非目标

- 不修改 chaos 预编译库的编译配置
- 不重构 add_chaos_test() CMake 宏
- 不做全量 foundation-DLL 33 family 覆盖

## 全局优先级评估

| 阶段 | 方案 | P1 性能影响 | P2 架构影响 | P3 热更影响 | 裁决 |
|------|------|-----------|-----------|-----------|------|
| A | CRT 配置对齐 | 无 | 正面（消除链接错误） | 无 | PASS |
| B-1 | 新增 codegen 测试 | 无 | 正面（填补覆盖盲区） | 无 | PASS |
| B-2 | Foundation-DLL 扩展 | 无 | 正面（扩大验证覆盖） | 无 | PASS |

## 阶段列表

### Phase A: CRT 链接错误修复

- **goal**: 消除 `_CrtDbgReport` unresolved external，使 interpreter/bootstrap/codegen/engine-bridge 测试可链接
- **exit_criteria**: `test_interpreter_smoke` 能链接通过；ctest 列表中新增的测试可运行
- **deliverables**: CMakeLists.txt 修改
- **dependencies**: 无
- **resolved_decisions**: 修复方案应在 CMake 层解决，不修改预编译 .lib

### Phase B-1: Codegen 基础测试

- **goal**: 为 codegen 模块补充 IL smoke + ABI contract 测试
- **exit_criteria**: 新增测试编译链接通过并运行 PASS
- **deliverables**: 新的测试源文件 + CMakeLists.txt 条目
- **dependencies**: Phase A（代码生成测试需要 codegen.lib 能链接）

### Phase B-2: Foundation-DLL 扩展

- **goal**: 选取一个已验证 pipeline 的 family 扩展验证覆盖
- **exit_criteria**: 目标 family 的 verify_family() 7-stage pipeline PASS
- **deliverables**: 新的 family 验证结果
- **dependencies**: 无（独立于 native 测试基础设施）

## 子任务映射

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|-----------|-----------------|
| crt-fix | A | planned | 修复 CRT _CrtDbgReport linker error | none | medium |
| codegen-tests | B-1 | planned | 补充 codegen IL smoke + ABI 测试 | crt-fix | small |
| foundation-expand | B-2 | planned | 扩展 foundation-DLL 到下一 family | none | medium |

## 依赖

- crt-fix → codegen-tests（codegen 测试需要 CRT 修复后才能链接）
- foundation-expand 与 crt-fix 无依赖，理论上可与 Phase A 并行

## 风险

| 风险 | 影响 | 概率 | 缓解 |
|------|------|------|------|
| CRT 修复方案复杂（需重建 chaos libs） | 高 | 低 | 优先在 CMake 层面解决，不动预编译 .lib |
| foundation-DLL 选定 family 有隐藏依赖 | 中 | 中 | 选已验证 pipeline 的 family（如 primitive-numeric-conversions） |
| 同 session 时间不够覆盖三个 | 低 | 中 | foundation-expand 可独立执行 |

## 推进顺序

串行执行：crt-fix → codegen-tests → foundation-expand
