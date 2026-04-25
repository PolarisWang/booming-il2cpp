task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
title: CoreLib Supplemental Runtime Skeleton Coverage Widening
status_file: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md
task_dir: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
phase: implementation
updated_at: 2026-04-25 16:20:56 +08:00
summary: `System.Convert::ToChar(...)` 已在最新 formal `20260425-153952-windows-ff29` 中清零，当前 child 继续沿新的 4C `Convert family` 自动推进 `System.Convert::ToString` widening。
next_step: 先补 `ToString(string, IFormatProvider)` passthrough 与 value-type instance `ToString([IFormatProvider])` 的 RED 用例，再实现最小通用 handler 并顺序跑定向 pytest、相关全量 pytest 和 formal proof。
