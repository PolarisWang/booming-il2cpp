# 进展记录 v1-05

## 时间

- 2026-04-11 05:22:00 +08:00

## 本轮完成

- 建立 Batch 3 threading/GC 的范围收敛与设计入口：
  - 新增 `brainstorm-v1-02.md`
  - 新增 `design-v1-02.md`
  - 新增 `tests/unit/run/test_phase2_threading_gc_proof.py`
- 落地 Batch 3 threading/GC 最小 proof slice：
  - `ManagedFieldModel` / loader 补充 `IsStatic` 与 `IsThreadStatic`
  - `SemanticWorldStage` 新增 `requires-thread-static-storage` 与 `requires-monitor-enter-exit`
  - `LinkerStage` 新增 `IncludeThreadStaticFieldClosure(...)` 与 `monitor-enter-exit` dependency reason
  - `NativeReferenceLoweringPlanner` / `NativeReferenceProofEmitter` 新增 `managed-threading.threadstatic-monitor.minimal`
  - 新增模板 `NativeReferenceProof.ThreadingThreadStaticMonitor.cpp.scriban`
  - 新增 `subjects/ThreadingProof/`
  - `runtime_core` 新增：
    - `ThreadStaticInt32Add`
    - `MonitorEnter`
    - `MonitorExit`
    - `GcSafepoint`
    - `ReportThreadRoot`
    - `EnqueueFinalizer`
    - `DrainFinalizerQueue`

## 验证结果

- `python -m pytest tests/unit/run/test_phase2_threading_gc_proof.py -v` -> 3 passed
- `python -m pytest tests/unit/run -v` -> 160 passed
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug` -> succeeded
- `dotnet build subjects/ThreadingProof/source/ThreadingProof.csproj -c Debug` -> succeeded
- `cmake -S . -B artifacts/cmake-batch3-check -DROADMAP0_PRESET_TARGET=windows-x64-reference` -> succeeded
- `cmake --build artifacts/cmake-batch3-check --target chaos_runtime_core --config Debug` -> succeeded

## 备注

- `dotnet build` 仍有若干既有 nullable warning 与 `LoaderStage.cs(153)` 的 self-assignment warning，本轮未新增 build error。
- Batch 3 的 GC 仍是 proof-first helper 表面，不代表 collector / finalizer thread / root scanning 已完成。

## Wiki

- 本轮仍未写入 wiki。
- 原因：Phase 2 的 runtime completeness 能力矩阵仍在继续扩展，等 Batch 4/5 的最小 slice 稳定后再统一沉淀。

## 下一步

- 收敛 Batch 4 nested EH / marshaling 的最小 proof 边界，并建立新的 RED 测试入口。
