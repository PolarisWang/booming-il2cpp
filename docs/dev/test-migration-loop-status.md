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

## 当前 `tests/unit/runtime-native/` 已迁移子域

```
tests/unit/runtime-native/
├── CMakeLists.txt          # 工厂 include + add_subdirectory
├── support/                # ✅ test_log_system 9/9
├── abi/                    # ✅ test_abi_method_table 8/8
├── bootstrap/              # ✅ test_bootstrap_smoke 7/7
└── common/                 # ✅ test_common_checked_conv 4/4
```

## L5 剩余待迁移子域（从 `testing/src/native/`）

| 子域 | .cpp 数 | 备注 |
|------|---------|------|
| `jit` | 17 | 含 TIMEOUT 属性 |
| `engine-bridge` | 2 | 小 |
| `hot-update` | 14 | |
| `runtime-core` | 153 | 最大块（含 gc/threading/interpreter/metadata/runtime_stubs 子目录） |
| `fuzz` | 2 | 小 |
| `diagnostics` | 14 | |

## 迁移模式（已验证可重复）

```
1. git mv testing/src/native/<子域> tests/unit/runtime-native/<子域>
2. 重写 CMakeLists: add_chaos_test → chaos_native_add_test
   - 丢弃显式 LIBS（工厂默认全套）
   - 如有自有 stubs: 加 WITHOUT_CODEGEN_STUB
   - 保留 TIMEOUT/LABELS 属性
   - 修外部源路径: CHAOS_PROJECT_ROOT → CMAKE_SOURCE_DIR
3. tests/unit/runtime-native/CMakeLists.txt: add_subdirectory(<子域>)
4. testing/src/native/CMakeLists.txt: 删 add_subdirectory(<子域>)
5. cmake --preset + build + run 验证
6. commit
```

## 其他剩余 L 任务

| 任务 | 状态 | 说明 |
|------|------|------|
| L5 剩余子域 | 进行中 | 按上述模式逐个迁移 |
| L6+L7 | 待定 | foundation-dll e2e（P1，引擎+29族联合，per-dll 脚本链验证） |
| L10 | 并入 L11 | gate 是冗余 wrapper，platform-hosts 已就位 |
| L11 | 待定 | 删 testing/ 根（阻塞于 L5+L6+L7 迁出） |

## 未提交的工作树残留（非我的改动）

- `tests/suite_contract.yaml` (M) — 之前 cron session 的改动
- `tests/e2e/trust/project-test/*/codegen/_input/` — 生成产物（应 gitignore）
- `tests/unit/managed/snapshot/{_rLoad}` — 0 字节 stray（已知）
