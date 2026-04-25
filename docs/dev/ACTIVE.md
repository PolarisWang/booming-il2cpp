task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
title: CoreLib Supplemental Runtime Skeleton Coverage Widening
status_file: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md
task_dir: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
phase: implementation
updated_at: 2026-04-25 11:57:41 +08:00
summary: 已在新的 4C `Convert family` 上补齐 `System.Convert::ToChar` 的 `SByte/Int16/UInt16/UInt32/UInt64` supplemental runtime-skeleton translated coverage，并通过 contracts、audit fixture 与 `SolutionCorePack::windows-corelib-reference-native-hotupdate-proof` formal proof。
next_step: 继续收口 `System.Convert::ToChar(Int32/Int64)` forwarder 形态，再决定是否扩到 `Single/Double -> Char` 或切换到下一批 `Convert` primitive lane。
