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
| L6 (引擎) | ✅ 完成 | foundation-dll verification 引擎已迁 `tests/e2e/verification/`（`323e8c279`，72 文件）。中心 resolver + 驱动链 + suite_contract 更新。e2e 实跑 buffers chunk。33 引擎单测移后 PASS。 |
| L7 (29 族) | ✅ 完成 | 族根已迁 `tests/e2e/translation/`（`a78df896d`，1351 文件）。引擎经 `CHAOS_FOUNDATION_DLL→tests/e2e/translation` 指向。e2e 验证 3/4 passed，33 引擎单测 PASS。 |
| L10 | ✅ 完成 | 并入 L11 一并处理：googletest 改由根 CMake 供给（`6e29fe8df`）、`_pipeline` co-locate 引擎（`d9ec21616`）、src/managed→tests/unit/managed、CI batch runners→tests/e2e/translation、全部 workflow 重指 tests/unit/runtime-native（`611953afe`+`84ec33f0d`） |
| L11 | ✅ 完成 | 删 `testing/` 根（`20a8375bb` + 物理 rm）。单 tests/ 树达成——tests/{unit,e2e,authority,fixtures,integration,...} 为唯一测试根，testing/ 已不存在。cmake config 绿、引擎 chain 正常。142 条 dead testing/ gitignore 规则留待后续清理（惰性无害）。 |

> **Loop 状态：全部 L 任务完成 ✅**。L1-L11（测试根单根化 + foundation-dll 引擎/族迁移 + testing/ 删除）已全部交付并验证。`docs/dev/test-migration-loop-status.md` 使命完成。
>
> **已知惰性遗留（非阻塞，活跃测试入口不受影响——`--layer unit` OVERALL OK pass=2248）**：
> - `tests/verification/unit/`（31 文件）+ `foundation_dll_audit_generator.py`（`build/toolchains/run/testing/`）是**旧 verification 管线的残留**：旧管线已于 **2026-05-29 commit `6521a5e66`**（早于本 L 系列数月）主动退役——删除 `build/toolchains/run/testing/foundation_dll/`（105 个旧管线文件）。这些测试 `load_module` 的 `foundation_dll/*.py` 已不存在 → 测试必死（实测 `FileNotFoundError`）。**独立于 L 清理（非 L11 删 testing/ 所致）**，未接入 driver groups（不在 suite_contract）。属独立的"退役管线死测试清理"任务，超出 L-迁移 loop 范围，非本 loop 的遗留项。
> - `build/toolchains/run/` 下生成的 audit 骨架 + 惰性 `.gitignore` testing/ 规则（142 条）+ settings*.json 权限路径（foreign/本地）——均 inert。



## 未提交的工作树残留（非我的改动）

- `tests/suite_contract.yaml` (M) — 之前 cron session 的改动
- `tests/e2e/trust/project-test/*/codegen/_input/` — 生成产物（应 gitignore）
- `tests/unit/managed/snapshot/{_rLoad}` — 0 字节 stray（已知）
