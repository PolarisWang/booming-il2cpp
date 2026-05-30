"""Stage runners package — exports all stage functions for pipeline orchestration."""

from __future__ import annotations

from verification.orchestration.context import FamilyContext, StageResult

# preflight
from .preflight import run_preflight

# codegen
from .codegen import run_codegen, run_jit_codegen

# fact
from .fact import run_fact, run_fact_jit, run_fact_cross_verify, run_managed_fact, run_cross_verify, run_managed_patch_fact

# microbench
from .microbench import run_microbench

# audit
from .audit import run_audit

# asm_compare
from .asm_compare import run_asm_compare

# benchmark
from .benchmark import run_benchmark

# hotupdate
from .hotupdate import (
    run_hotupdate,
    run_hotupdate_aot_bench,
    run_hotupdate_jit_fact,
    run_hotupdate_jit_bench,
    run_patch_cross_verify,
)

# Stage registry: ordered list of (stage_name, function, enabled_by_default)
STAGE_REGISTRY: list[tuple[str, str, bool]] = [
    ("preflight",              "run_preflight",               True),
    ("codegen",                "run_codegen",                 True),
    ("jit_codegen",            "run_jit_codegen",             True),
    ("managed_fact",           "run_managed_fact",            True),
    ("cross_verify",           "run_cross_verify",            True),
    ("managed_patch_fact",     "run_managed_patch_fact",      True),
    ("fact",                   "run_fact",                    True),
    ("fact_jit",               "run_fact_jit",                True),
    ("fact_cross_verify",      "run_fact_cross_verify",       True),
    ("microbench",             "run_microbench",              True),
    ("audit",                  "run_audit",                   True),
    ("asm_compare",            "run_asm_compare",             True),
    ("benchmark",              "run_benchmark",               True),
    ("hotupdate",              "run_hotupdate",               True),
    ("patch_cross_verify",     "run_patch_cross_verify",      True),
    ("hotupdate_aot_benchmark","run_hotupdate_aot_bench",     True),
    ("hotupdate_jit_fact",     "run_hotupdate_jit_fact",      True),
    ("hotupdate_jit_benchmark","run_hotupdate_jit_bench",     True),
]


def get_stage_names() -> list[str]:
    return [name for name, _, _ in STAGE_REGISTRY]


def get_default_stages() -> list[str]:
    return [name for name, _, enabled in STAGE_REGISTRY if enabled]


# Function-to-module mapping (overrides rsplit derivation for non-trivial cases)
_FUNC_MODULE_OVERRIDES = {
    "run_preflight": "preflight",
    "run_hotupdate_jit_fact": "hotupdate",
    "run_hotupdate_jit_bench": "hotupdate",
    "run_hotupdate_aot_bench": "hotupdate",
    "run_fact_jit": "fact",
    "run_fact_cross_verify": "fact",
    "run_managed_fact": "fact",
    "run_cross_verify": "fact",
    "run_managed_patch_fact": "fact",
    "run_jit_codegen": "codegen",
    "run_hotupdate": "hotupdate",
    "run_patch_cross_verify": "hotupdate",
}


def _func_to_module(func_name: str) -> str:
    """Derive module name from function name, with overrides for edge cases."""
    if func_name in _FUNC_MODULE_OVERRIDES:
        return _FUNC_MODULE_OVERRIDES[func_name]
    if func_name.startswith("run_"):
        return func_name[len("run_"):]
    return func_name


def lookup_stage(name: str):
    """Look up a stage function by name. Returns None if not found."""
    for sname, func_name, _ in STAGE_REGISTRY:
        if sname == name:
            for mod_name in ("preflight", "codegen", "fact", "microbench",
                             "audit", "asm_compare", "benchmark", "hotupdate"):
                try:
                    mod = __import__(f"verification.stages.{mod_name}", fromlist=[func_name])
                    fn = getattr(mod, func_name, None)
                    if fn is not None:
                        return fn
                except ImportError:
                    continue
            return None
    return None


__all__ = [
    "run_preflight", "run_codegen", "run_jit_codegen",
    "run_fact", "run_fact_jit", "run_fact_cross_verify",
    "run_managed_fact", "run_cross_verify",
    "run_managed_patch_fact",
    "run_microbench", "run_audit", "run_asm_compare",
    "run_benchmark",
    "run_hotupdate", "run_hotupdate_aot_bench",
    "run_hotupdate_jit_fact", "run_hotupdate_jit_bench",
    "run_patch_cross_verify",
    "STAGE_REGISTRY", "get_stage_names", "get_default_stages", "lookup_stage",
]
