task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
title: CoreLib Supplemental Runtime Skeleton Coverage Widening
status_file: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md
task_dir: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
phase: implementation
updated_at: 2026-04-25 12:24:10 +08:00
summary: 已在新的 4C `Convert family` 上继续补齐 `System.Convert::ToChar` 的 `Int32/Int64` forwarder supplemental runtime-skeleton translated coverage，并通过 contracts、audit fixture 与 `SolutionCorePack::windows-corelib-reference-native-hotupdate-proof` formal proof。
next_step: 评估是否继续吃 `System.Convert::ToChar(Single/Double)` 的 boxed `IConvertible` lane；若抽象成本偏高，则切回下一批纯 primitive `Convert` widening，避免把 object/provider lane 直接堆进 emitter 单点。
