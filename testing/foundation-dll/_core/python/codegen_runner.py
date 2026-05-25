"""Codegen stage runners — entrypoint generation + IL2CPP compile + entry.exe build."""

from __future__ import annotations

import time

from _core.python.models import FamilyContext, StageResult
from foundation_dll.pipeline_adapter import (
    build_entry_exe,
    generate_dispatch_code,
    generate_entrypoint,
    run_il2cpp_codegen,
    save_aot_binary,
)


def run_codegen(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 1: Entrypoint generation + IL2CPP compile (AOT)."""
    start = time.perf_counter()

    # 1. Generate C# entrypoint subjects
    print(f"  [codegen] Generating entrypoint...")
    ep_result = generate_entrypoint(ctx.slug, ctx.assembly, ctx.managed_dir)
    if not ep_result.get("success", False):
        return StageResult(
            stage="codegen", status="failed",
            summary=f"Entrypoint generation failed: {ep_result.get('error', 'unknown')}",
            details={"steps": {"entrypoint": ep_result}},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # 2. Run chaos-il2cpp convert-to-cpp
    print(f"  [codegen] Running IL2CPP codegen...")
    cg_result = run_il2cpp_codegen(ctx.slug, ctx.assembly, codegen_mode=ctx.codegen_mode)
    if not cg_result.get("success", False):
        return StageResult(
            stage="codegen", status="failed",
            summary=f"IL2CPP codegen failed: {cg_result.get('error', 'unknown')}",
            details={"steps": {"codegen": cg_result}},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # 3. Build entry.exe (full clean build with sentinel dispatch)
    print(f"  [codegen] Building entry.exe...")
    build_ok = build_entry_exe(ctx.slug, ctx.assembly)
    if not build_ok:
        return StageResult(
            stage="codegen", status="failed",
            summary="entry.exe build failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # 4. Generate verification dispatch code (overwrites sentinel, incremental rebuild)
    print(f"  [codegen] Generating dispatch code...")
    dispatch_ok = generate_dispatch_code(ctx.slug, ctx.assembly)
    if not dispatch_ok:
        print(f"    [codegen] WARNING: dispatch generation failed (continuing)")

    # 5. Save AOT binary for subsequent stages
    save_aot_binary(ctx.slug, ctx.assembly)

    return StageResult(
        stage="codegen", status="passed",
        summary="Entrypoint built and IL2CPP compile OK",
        details={
            "methodCount": cg_result.get("methodCount", 0),
            "dllPath": cg_result.get("dllPath", ""),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_jit_codegen(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 2: JIT entrypoint generation + entry-jit.exe build."""
    start = time.perf_counter()

    jit_exe = ctx.entry_jit_exe_path

    if jit_exe.exists():
        return StageResult(
            stage="jit_codegen", status="passed",
            summary="entry-jit.exe already exists, skipping rebuild",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [jit_codegen] Building JIT mode entry-jit.exe...")
    from foundation_dll.pipeline_adapter import run_jit_codegen as _adapter_jit

    jit_result = _adapter_jit(ctx.slug, ctx.assembly)
    if not jit_result.get("success"):
        return StageResult(
            stage="jit_codegen", status="failed",
            summary=f"JIT codegen failed: {jit_result.get('message', 'unknown')}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    return StageResult(
        stage="jit_codegen", status="passed",
        summary="JIT entry-jit.exe built and AOT entry.exe restored",
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
