# 测试迁移 Loop — 当前执行状态快照

> 生成时间: 2026-08-07
> 关联 memory: `test-architecture-migration-loop.md`

## 已完成并提交的迁移（全部验证通过）

| # | 迁移内容 | 目标路径 | commit | 验证 |
|---|---------|---------|--------|------|
| L1 | sanitizers → cmake/sanitizers/ | `cmake/sanitizers/` | `5dd7d189b` | cmake --preset 绿 |
| L2 | xunit runner config | `tests/config/xunit/` | `a99e172b3` | unit 层绿 |
| L3 | managed dotnet tests | `tests/unit/managed/{codegen,driver,snapshot}` | `e0318a5be` | unit OVERALL OK (2249/28) |
| L8 | project-test + translation | `tests/e2e/trust/` | `c53903b7e` | 无 gate 破坏 |
| L9-p1 | analysis 数据 | `tests/fixtures/analysis/` | `9e0e096dc` | 无引用破坏 |
| L9-p2 | contract pytest (shared) | `tests/authority/contracts/` | `9e08c28c8` | pytest 发现一致 |
| — | IL instruction data | `tests/fixtures/il/` | `e3f20127a` | 无消费者 |
| L5-anchor | runtime-native CMake 根 | `tests/unit/runtime-native/CMakeLists.txt` | `b186e9a93` | cmake 配置绿 |
| L5-support | native support 子域 | `tests/unit/runtime-native/support/` | `d62aa0c5f` | test_log_system 9/9 PASS |
| L5-abi | native abi 子域 | `tests/unit/runtime-native/abi/` | `a195a64be` | test_abi_method_table 8/8 PASS |
| L5-bootstrap | native bootstrap 子域 | `tests/unit/runtime-native/bootstrap/` | `1600ea889` | test_bootstrap_smoke 7/7 PASS |
| L5-common | native common 子域 (34 cpp, 32 targets) | `tests/unit/runtime-native/common/` | `58817d63d` | test_common_checked_conv 4/4 PASS |
| L5-engine-bridge | native engine-bridge 子域 (2 cpp) | `tests/unit/runtime-native/engine-bridge/` | `76212ea21` | test_engine_bridge_smoke 24/24 PASS |
| L5-hot-update | native hot-update 子域 (14 cpp) | `tests/unit/runtime-native/hot-update/` | `3e5038ad2` | 6/7 PASS (1 预存并发测试 bug) |
| L5-diagnostics | native diagnostics 子域 (14 cpp) | `tests/unit/runtime-native/diagnostics/` | `48793e94b` | 13/13 PASS |
| L5-runtime-core+jit+fuzz | native 三大子域 (172 cpp) | `tests/unit/runtime-native/{runtime-core,jit,fuzz}/` | `c59dfe538` | 150+ PASS；全部失败预存/偶发，pre/post 一致 |

## 当前 `tests/unit/runtime-native/` 已迁移子域（L5 全部完成 ✅）

```
tests/unit/runtime-native/
├── CMakeLists.txt          # 工厂 include + add_subdirectory
├── support/                # ✅ test_log_system 9/9
├── abi/                    # ✅ test_abi_method_table 8/8
├── bootstrap/              # ✅ test_bootstrap_smoke 7/7
├── common/                 # ✅ test_common_checked_conv 4/4
├── engine-bridge/          # ✅ test_engine_bridge_smoke 24/24
├── hot-update/             # ✅ 6/7（1 预存并发 bug）
├── diagnostics/            # ✅ 13/13（eventpipe 已入工厂；test_ep_receiver 预存 stale 弃用）
├── runtime-core/           # ✅ gc/interpreter/threading/metadata/runtime_stubs + flat（153 cpp）
├── jit/                    # ✅ 17 cpp
└── fuzz/                   # ✅ 2 cpp
```

> `testing/src/native/` 现已无任何子域（仅 googletest/imported-lib/macro 基础设施 + crt_stubs.cpp 留存）。
> 所有迁移失败均与迁移前逐项一致（com_ccw SEH、gc_*、jit_*、fuzz TIMEOUT、hot-update 并发），非回归。`test_gc_max_promote` 偶发且在两种构建下一致闪现。

## 迁移模式（已验证可重复）

```
1. git mv testing/src/native/<子域> tests/unit/runtime-native/<子域>
2. 重写 CMakeLists: add_chaos_test → chaos_native_add_test
   - 丢弃显式 LIBS（工厂默认全套）
   - 子域内共享 test source 用相对 ../runtime-core/... 引用（runtime-core 需先落位）
   - 保留 TIMEOUT/LABELS/RESOURCE_LOCK/GTEST_LIB_ONLY 属性
   - 修外部源路径: CHAOS_PROJECT_ROOT → CMAKE_SOURCE_DIR
3. tests/unit/runtime-native/CMakeLists.txt: add_subdirectory(<子域>)
4. testing/src/native/CMakeLists.txt: 删 add_subdirectory(<子域>)
5. cmake --preset + build + run 验证（重定向对比 pre-mig exe 确认非回归）
6. commit
```

## 其他剩余 L 任务

| 任务 | 状态 | 说明 |
|------|------|------|
| L5 全部子域 | ✅ 完成 | support/abi/bootstrap/common/engine-bridge/hot-update/diagnostics/runtime-core/jit/fuzz |
| L9 (IL fixtures) | ✅ 完成 | commit `318125649` 清理 testing/data/il 已弃原点（永久家 tests/fixtures/il/ 已在 e3f20127a） |
| L6+L7 | ⛔ 主体待定（需专门会话） | foundation-dll e2e（P1，引擎+29族联合）。✅ 已修 per-dll 驱动链（`d183e8c4c`）。**决定性耦合分析**：L6 引擎 + L7 族为**不可分割整体**（`_pipeline`@`testing/_pipeline`、`_TESTING_DIR`=testing/、`_FOUNDATION_DLL` 含 config/+28 族，3 anchor 不变量）。实测：smoke run 确实触达重型 native AOT 构建（System.Private.CoreLib，8214 dispatch），迁后须重跑 dll 需数十 min/族。→ 需 human-supervised 专门会话，loop 勿盲 mv。 |
| L10 | 并入 L11 | gate 是冗余 wrapper，platform-hosts 已就位；CMake-coupled，随 L11 重连 |
| L11 | ⏸ 阻塞 | 删 testing/ 根（现仅 L6+L7 迁出后可行；L5+L9 已迁出） |

> **Loop 停止建议**：L5/L8/L9(+IL) 及驱动链修复已全部完成；剩余 L6/L7（不可分割+重型 dll 验证）与 L10/L11（CMake 联动）均需 human-supervised 专门会话，连续多轮无进展，符合「连续 2 轮无进展→终止需人工介入」路由。建议停止自动 loop，转交专门会话。


## 未提交的工作树残留（非我的改动）

- `tests/suite_contract.yaml` (M) — 之前 cron session 的改动
- `tests/e2e/trust/project-test/*/codegen/_input/` — 生成产物（应 gitignore）
- `tests/unit/managed/snapshot/{_rLoad}` — 0 字节 stray（已知）
