"""Codegen stage runners — full project IL2CPP compilation via project_pipeline_adapter.

Unlike foundation-dll's codegen (which generates Subject_N entrypoints),
project-level codegen compiles the complete .NET project as-is through the
chaos-il2cpp pipeline: dotnet build → convert-to-cpp → dispatch → CMake build.
"""

from __future__ import annotations

import time

from _pipeline.stage_result import StageResult
from _core.python.project_pipeline_adapter import (
    run_project_codegen as _adapter_codegen,
    run_project_jit_codegen as _adapter_jit_codegen,
)


def run_project_codegen(ctx, stages) -> StageResult:
    """Stage 1: Build managed project + IL2CPP compile (AOT).

    Delegates to project_pipeline_adapter which handles:
      1. dotnet build managed project
      2. chaos-il2cpp convert-to-cpp (with --entry-point)
      3. Patch undefined labels
      4. Generate verification dispatch code
      5. Write project-specific runtime-entry.cpp
      6. Build entry.exe via CMake
      7. Save entry-aot.exe
    """
    return _adapter_codegen(ctx, stages)


def run_project_jit_codegen(ctx, stages) -> StageResult:
    """Stage 2: JIT codegen — run IL2CPP with --mode jit, build entry-jit.exe.

    All methods are registered for interpreter dispatch (kChaosJitMethodEntries).
    The resulting binary is saved as entry-jit.exe and the AOT binary is restored
    so subsequent stages (fact, benchmark, hotupdate) run native code.
    """
    return _adapter_jit_codegen(ctx, stages)
