# Stage 5D macOS Reference Parity 实现计划

> **面向执行 Agent：** 必须使用 `dev:subagent-driven-development`（如可用）或 `dev:executing-plans` 来执行本计划。步骤使用复选框语法跟踪。

**目标：** 在未来的 `macOS` 宿主会话中，把 Stage 4 `HelloWorldObject`、Stage 5A `GenericEchoClosedMinimal` 与 Stage 5B `ReflectionLiteQueryMinimal` 接入 `macOS` reference host proof / gate。

**架构：** 复用当前 Windows trilogy 已经稳定的 managed closure、lowering plan 与 sample-agnostic runtime/bootstrap/codegen 边界，只新增 `macOS` 宿主所需的 proof routing、host build/run glue 与 parity gate。当前 Windows-only 的 Stage 5C `PInvokeLiteDllImportMinimal` 不纳入首轮 parity。

**技术栈：** .NET 8、C++17、CMake、`build/toolchains/macos-reference.cmake`、Python `unittest`

**设计文档：** `docs/dev/completed/20260406-07-stage-5d-macos-reference-parity/design-v1-01.md`

**预期知识沉淀：** 如真正执行闭环并拿到 `macOS` 宿主证据，再回写 `docs/architecture/roadmap-0/reference-desktop-gates.md` 与相关本地验证文档。

---

## 目标文件与职责

- 修改：`CMakeLists.txt`
  - 扩展 `macos-reference` preset route，让它能承接 Stage 4 / Stage 5A / Stage 5B proof host，而不仅是 `tests/contracts/native/bridge`。
- 修改：`build/toolchains/macos-reference.cmake`
  - 如有需要，补齐 `macOS` reference proof 所需的 host-specific 开关或输出约束。
- 按需修改：`tests/proof/native-reference/HelloWorldObject/`
  - 让 Stage 4 proof host 在 `macOS` 宿主具备可执行的 build/run 接线。
- 按需修改：`tests/proof/native-reference/GenericEchoClosedMinimal/`
  - 让 Stage 5A proof host 在 `macOS` 宿主具备可执行的 build/run 接线。
- 按需修改：`tests/proof/native-reference/ReflectionLiteQueryMinimal/`
  - 让 Stage 5B proof host 在 `macOS` 宿主具备可执行的 build/run 接线。
- 新增：`tests/unit/run/test_stage5d_macos_reference_parity.py`
  - 冻结首轮 parity 子集、`macos-reference` wiring 与 future proof run 期望。
- 修改：`docs/dev/completed/20260406-07-stage-5d-macos-reference-parity/STATUS.md`
  - 执行期维护阶段、摘要、风险与完成证据。
- 按需修改：`docs/dev/ACTIVE.md`、索引与 `notes/progress-*.md`
  - 未来真正执行时同步 active 指针与进度记录。

## 执行原则

- [ ] 保持首轮 parity 子集固定为 Stage 4 + Stage 5A + Stage 5B。
- [ ] 明确排除当前 Stage 5C `PInvokeLiteDllImportMinimal`，不要在执行期偷偷扩大范围。
- [ ] `src/**` 继续保持 sample-agnostic；样例特化只允许停留在 `tests/**`、artifacts 与任务文档中。
- [ ] 任何真实完成声明前，必须在 `macOS` 宿主上拿到最新验证证据。

## Chunk 1：冻结 `macOS` parity 静态 wiring

- [ ] 新增 `tests/unit/run/test_stage5d_macos_reference_parity.py`
- [ ] 先以静态断言固定：
  - `macos-reference` preset 的目标范围
  - 首轮 parity 子集只包含 Stage 4 / Stage 5A / Stage 5B
  - Stage 5C 不在首轮 parity 中
- [ ] 先看到正确的 RED，再补静态 wiring

## Chunk 2：接入 `macOS` reference proof host 路由

- [ ] 扩展 `CMakeLists.txt` 的 `macos-reference` route
- [ ] 让 Stage 4 / Stage 5A / Stage 5B proof host 具备 `macOS` 宿主下的最小 build/run 接线
- [ ] 不为图快把 `macOS` parity 逻辑写进 `src/**`

## Chunk 3：补齐 `macOS` host 运行验证

- [ ] 在 `macOS` 宿主上运行对应单测
- [ ] 如条件允许，补跑或接通 `verify-roadmap-0` / reference desktop 相关 gate
- [ ] 记录 stdout / stderr / exit code 等可定位证据

## 关键验证命令

- [ ] `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`
- [ ] `python -m unittest -v tests.unit.run.test_stage5d_macos_reference_parity`
- [ ] `python -m unittest -v tests.unit.run.test_stage5b_reflection_lite_windows_reference`
- [ ] `python -m unittest -v tests.unit.run.test_stage5a_generic_echo_windows_reference`
- [ ] `python -m unittest -v tests.unit.run.test_stage4_native_reference_proof`
- [ ] `python build/scripts/verify-roadmap-0.py --host-profile macos`

## 完成标准

- [ ] `macOS` reference 首轮 parity 子集已稳定落到 Stage 4 + Stage 5A + Stage 5B。
- [ ] Stage 5C 被明确保留为 Windows-only interop foundation，没有被误纳入首轮 parity。
- [ ] `macOS` 宿主上存在对应 proof / gate 证据。
- [ ] Windows 既有 Stage 4 / Stage 5A / Stage 5B / Stage 5C 基线不回退。
