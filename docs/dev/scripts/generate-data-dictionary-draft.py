#!/usr/bin/env python3
"""Auto-generate data dictionary draft from dashboard output artifacts and source code.

Usage:
    python docs/dev/scripts/generate-data-dictionary-draft.py [--repo-root PATH]

Output:
    wiki/03-功能模块/05-工具能力/foundation-dll-audit-data-dictionary.md (overwrite draft)
"""

from __future__ import annotations

import argparse
import ast
import json
import re
import sys
from pathlib import Path
from typing import Any


def _find_project_root() -> Path:
    return Path(__file__).resolve().parents[3]


OUTPUT_DIR = "verification/projections/foundation-dll-audit"
GENERATOR_PATH = "build/toolchains/run/testing/foundation_dll_audit_generator.py"
KERNEL_PATH = "build/toolchains/run/testing/foundation_dll/verification_kernel.py"
CLAIMS_PATH = "build/toolchains/run/testing/foundation_dll/family_verification_claims.py"
GAP_PATH = "build/toolchains/run/testing/foundation_dll/gap_analyzer.py"


# ---- Source code analysis (AST-based) ----

def _find_returned_dict_keys(tree: ast.AST, func_name: str | None = None) -> dict[str, str]:
    """Find dict literal keys returned by functions matching func_name."""
    keys: dict[str, str] = {}

    class _Visitor(ast.NodeVisitor):
        def visit_FunctionDef(self, node: ast.FunctionDef) -> None:
            if func_name and node.name != func_name:
                return
            # Find return statements with dict literals
            for child in ast.walk(node):
                if isinstance(child, ast.Return) and isinstance(child.value, ast.Dict):
                    for key_node in child.value.keys:
                        if isinstance(key_node, ast.Constant) and isinstance(key_node.value, str):
                            keys[key_node.value] = f"{node.name}()"
                # Also find dict() constructor calls
                elif isinstance(child, ast.Return) and isinstance(child.value, ast.Call):
                    call = child.value
                    if isinstance(call.func, ast.Name) and call.func.id == "dict":
                        for kw in call.keywords:
                            if isinstance(kw.arg, str):
                                keys[kw.arg] = f"{node.name}()"
            # Recurse into nested functions
            self.generic_visit(node)

    _Visitor().visit(tree)
    return keys


def _find_function_source_map(file_path: Path) -> dict[str, int]:
    """Map function name -> line number."""
    tree = ast.parse(file_path.read_text(encoding="utf-8"))
    fmap: dict[str, int] = {}

    class _Visitor(ast.NodeVisitor):
        def visit_FunctionDef(self, node: ast.FunctionDef) -> None:
            fmap[node.name] = node.lineno
            self.generic_visit(node)
        visit_AsyncFunctionDef = visit_FunctionDef

    _Visitor().visit(tree)
    return fmap


# ---- Output artifact analysis ----

def _collect_field_paths(obj: Any, prefix: str = "", *, max_depth: int = 8) -> dict[str, str]:
    """Walk a JSON-like structure and collect all leaf field paths with sample values."""
    paths: dict[str, str] = {}
    if max_depth <= 0:
        return paths

    if isinstance(obj, dict):
        for key, value in obj.items():
            current = f"{prefix}.{key}" if prefix else key
            if isinstance(value, (dict, list)):
                paths.update(_collect_field_paths(value, current, max_depth=max_depth - 1))
            else:
                sample = str(value)[:80] if value is not None else "null"
                paths[current] = sample
    elif isinstance(obj, list) and obj:
        # Take first element as sample
        first = obj[0]
        if isinstance(first, dict):
            for key in first:
                current = f"{prefix}[].{key}" if prefix else f"{key}"
                paths[current] = str(type(first[key]).__name__)
    return paths


def _classify_prefix(path: str) -> str:
    """Classify a field path into a documentation section."""
    if path.startswith("summary.") or path.startswith("program."):
        return "program"
    if path.startswith("rows") or path.startswith("dllMatrix"):
        return "dll-matrix"
    if "nativeProof" in path:
        return "native-proof"
    if "managedProof" in path:
        return "managed-proof"
    if "hotupdateProof" in path:
        return "hotupdate-proof"
    if "benchmarkProof" in path:
        return "benchmark-proof"
    if "testCodeProof" in path:
        return "test-code-proof"
    if "verificationGates" in path:
        return "gates"
    if "closureStatus" in path or "denominatorStatus" in path:
        return "status"
    if "claim" in path:
        return "claims"
    if "truthContract" in path:
        return "truth-contracts"
    if "dllState" in path or "currentProject" in path or "blockingReason" in path:
        return "dll-header"
    if "familyId" in path or "displayName" in path:
        return "family-header"
    return "other"


def _lookup_source(function_map: dict[str, int], field_name: str, file_label: str) -> str:
    """Try to locate which function likely produces this field."""
    # Simple heuristic: match function names containing parts of the field
    for fname, line in sorted(function_map.items(), key=lambda x: -x[1]):
        fname_clean = fname.lstrip("_").lower()
        field_lower = field_name.replace("-", "_").replace(".", "_").lower()
        # Check if function name is a prefix match or contains field name
        if field_lower.startswith(fname_clean) or fname_clean in field_lower:
            return f"{fname}() [L{line}]"
    return ""


def generate_draft(repo_root: Path) -> str:
    output_root = repo_root / OUTPUT_DIR

    # Load source function maps
    function_maps: dict[str, dict[str, int]] = {}
    for label, rel_path in [
        ("generator", GENERATOR_PATH),
        ("kernel", KERNEL_PATH),
        ("claims", CLAIMS_PATH),
        ("gap", GAP_PATH),
    ]:
        full = repo_root / rel_path
        if full.is_file():
            function_maps[label] = _find_function_source_map(full)
        else:
            function_maps[label] = {}

    # Load output artifacts
    artifacts: dict[str, dict[str, Any]] = {}
    for artifact_file in [
        "program.json",
        "dll-matrix.json",
        "family-verification.json",
        "family-verification-claims.json",
        "truth-contracts.json",
    ]:
        path = output_root / artifact_file
        if path.is_file():
            with open(path, encoding="utf-8") as f:
                artifacts[artifact_file] = json.load(f)
        else:
            artifacts[artifact_file] = {}

    # Collect field paths from each artifact
    artifact_fields: dict[str, dict[str, str]] = {}
    for name, data in artifacts.items():
        artifact_fields[name] = _collect_field_paths(data)

    # Also load a sample DLL detail JSON
    dll_dir = output_root / "dlls"
    if dll_dir.is_dir():
        dll_files = sorted(dll_dir.glob("*.json"))
        if dll_files:
            with open(dll_files[0], encoding="utf-8") as f:
                dll_data = json.load(f)
            artifact_fields["dll-detail.json"] = _collect_field_paths(dll_data)

    # ---- Build markdown ----
    lines: list[str] = []
    lines.append("# Foundation DLL Audit 数据字典")
    lines.append("")
    lines.append("> 本文档由 `docs/dev/scripts/generate-data-dictionary-draft.py` 自动生成，需人工审查确认。")
    lines.append("> 生成时间: (请填写审查日期)")
    lines.append("")
    lines.append("## 1. 顶层输出工件清单")
    lines.append("")
    lines.append("| 工件 | 路径 | 用途 |")
    lines.append("|------|------|------|")
    lines.append("| program.json | verification/projections/foundation-dll-audit/program.json | 程序摘要 |")
    lines.append("| dll-matrix.json | (同上目录) | DLL 矩阵行 |")
    lines.append("| family-verification.json | (同上目录) | 每个 family 的 gate 评估结果 |")
    lines.append("| family-verification-claims.json | (同上目录) | 验证 claims |")
    lines.append("| truth-contracts.json | (同上目录) | 真值合约 |")
    lines.append("| dashboard.html | (同上目录) | 主仪表板 HTML |")
    lines.append("| dlls/{AssemblyName}.html | (同上目录)/dlls/ | 每个 DLL 详情页 |")
    lines.append("| dlls/{AssemblyName}.json | (同上目录)/dlls/ | 每个 DLL 原始数据 |")
    lines.append("")

    # ---- Section 2: program.json ----
    lines.append("## 2. program.json 摘要字段")
    lines.append("")
    if "program.json" in artifact_fields:
        lines.append("| 字段 | 类型 | 当前值 | 推测来源 |")
        lines.append("|------|------|--------|---------|")
        for path, sample in sorted(artifact_fields["program.json"].items()):
            source = _lookup_source(function_maps.get("generator", {}), path.split(".")[-1], "generator")
            lines.append(f"| `{path}` | string | {sample} | {source} |")
    lines.append("")

    # ---- Section 3: dll-matrix.json ----
    lines.append("## 3. DLL Matrix（dll-matrix.json rows）")
    lines.append("")
    if "dll-matrix.json" in artifact_fields:
        # Group by row keys
        row_keys = {k: v for k, v in artifact_fields["dll-matrix.json"].items() if k.startswith("rows[].")}
        if row_keys:
            lines.append("### rows[] 每行字段")
            lines.append("")
            lines.append("| 字段 | 含义 | 计算流程 | 代码来源 | 枚举值 |")
            lines.append("|------|------|---------|---------|--------|")
            for path, sample in sorted(row_keys.items()):
                field_name = path.split(".")[-1]
                source = _lookup_source(function_maps.get("generator", {}), field_name, "generator")
                lines.append(f"| `{field_name}` | (待补充) | (待补充) | {source} | (待补充) |")
    lines.append("")

    # ---- Section 4: family-verification.json ----
    lines.append("## 4. family-verification.json 结构")
    lines.append("")
    if "family-verification.json" in artifact_fields:
        # Show the assembly-level fields then family-level
        fam_keys = {k: v for k, v in artifact_fields["family-verification.json"].items() if "families[]" in k}
        if fam_keys:
            lines.append("### families[] 每 family 的 gate 字段")
            lines.append("")
            lines.append("| 字段路径 | 类型 | 当前值 |")
            lines.append("|----------|------|--------|")
            for path, sample in sorted(fam_keys.items()):
                lines.append(f"| `{path}` | {type(sample).__name__} | {sample} |")
    lines.append("")

    # ---- Per-gate sub-sections ----
    gate_sections = [
        ("native-proof", "Native Proof"),
        ("managed-proof", "Managed Proof"),
        ("hotupdate-proof", "HotUpdate Proof"),
        ("benchmark-proof", "Benchmark Proof"),
    ]
    for gate_key, gate_title in gate_sections:
        lines.append(f"## 5. {gate_title} 子视图")
        lines.append("")
        lines.append("| 字段 | 含义 | 计算流程 | 代码来源 |")
        lines.append("|------|------|---------|---------|")
        if "dll-detail.json" in artifact_fields:
            dll_fields = {k: v for k, v in artifact_fields["dll-detail.json"].items() if gate_key in k}
            for path, sample in sorted(dll_fields.items()):
                source = _lookup_source(
                    {**function_maps.get("kernel", {}), **function_maps.get("generator", {})},
                    path.split(".")[-1],
                    "kernel",
                )
                lines.append(f"| `{path}` | (待补充) | (待补充) | {source} |")
        lines.append("")

    # ---- Enum dictionary ----
    lines.append("## 6. 枚举值字典")
    lines.append("")
    lines.append("### dllState")
    lines.append("")
    lines.append("| 值 | 含义 | 来源 |")
    lines.append("|----|------|------|")
    lines.append("| `completed` | DLL 已完成 | _dll_state() |")
    lines.append("| `in-progress` | DLL 进行中 | _dll_state() |")
    lines.append("| `blocked` | DLL 受阻 | _dll_state() |")
    lines.append("| `not-started` | DLL 未开始 | _dll_state() |")
    lines.append("")
    lines.append("### closureStatus")
    lines.append("")
    lines.append("| 值 | 含义 |")
    lines.append("|----|------|")
    lines.append("| `closed` | 所有非豁免门控通过 |")
    lines.append("| `waived` | 通过豁免记录放弃 |")
    lines.append("| `excluded` | 排除在范围外 |")
    lines.append("| `platform-blocked` | 平台限制 |")
    lines.append("| `in-progress` | 至少一个门控未通过 |")
    lines.append("")
    lines.append("### testCodeStatus")
    lines.append("")
    lines.append("| 值 | 含义 |")
    lines.append("|----|------|")
    lines.append("| `present` | 测试代码存在 |")
    lines.append("| `needs-tests` | 无测试目录 |")
    lines.append("| `coverage-widened` | 前次合作为 needs-tests 或 coverage-widened |")
    lines.append("| `no-coverage` | methodCount <= 0 |")
    lines.append("")
    lines.append("### gateState（verificationGates 有效值）")
    lines.append("")
    lines.append("| 值 | 含义 |")
    lines.append("|----|------|")
    lines.append("| `passed` | 已通过 |")
    lines.append("| `pending` | 待处理 |")
    lines.append("| `blocked` | 受阻 |")
    lines.append("| `not-required` | 不要求 |")
    lines.append("| `in-progress` | 进行中 |")
    lines.append("| `missing-evidence` | 缺少证据 |")
    lines.append("")
    lines.append("### denominatorStatus")
    lines.append("")
    lines.append("| 值 | 含义 |")
    lines.append("|----|------|")
    lines.append("| `candidate-derived` | 候选推导 |")
    lines.append("| `audit-confirmed` | 审计确认 |")
    lines.append("| `auto-derived` | 自动推导 |")
    lines.append("")

    # ---- Calculation flow ----
    lines.append("## 7. 计算流程总图")
    lines.append("")
    lines.append("```")
    lines.append("program.json                    capability-family-ledger.json")
    lines.append("    |                                   |")
    lines.append("    v                                   v")
    lines.append("build_foundation_dll_audit_payload()")
    lines.append("  |-- _build_projects()           -> project rows + evidence")
    lines.append("  |-- family_verification_claims  -> claims (methodSubjectIds)")
    lines.append("  |-- verification_kernel         -> gate evaluations")
    lines.append("  |-- case_index_loader           -> test/bench/hotupdate cases")
    lines.append("  |-- truth_contracts             -> truth contracts")
    lines.append("  |-- _compute_capability_closure -> closure stats")
    lines.append("  |-- _compute_gate_progress      -> workflow progress")
    lines.append("  |")
    lines.append("  v")
    lines.append("write_projection_bundle()")
    lines.append("  |-- program.json, dll-matrix.json, family-verification.json")
    lines.append("  |-- dashboard.html, dlls/{Name}.html")
    lines.append("```")
    lines.append("")

    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo-root", type=Path, default=_find_project_root())
    parser.add_argument("--write", action="store_true", help="Write draft to wiki")
    args = parser.parse_args()

    repo_root = args.repo_root.resolve()
    draft = generate_draft(repo_root)

    output_path = (
        repo_root
        / "wiki"
        / "03-功能模块"
        / "05-工具能力"
        / "foundation-dll-audit-data-dictionary.md"
    )

    if args.write:
        output_path.write_text(draft, encoding="utf-8")
        print(f"Draft written to {output_path.relative_to(repo_root)}")
    else:
        print(draft)


if __name__ == "__main__":
    main()
