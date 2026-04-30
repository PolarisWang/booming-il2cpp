#!/usr/bin/env python3
"""Auto-generate provenance registry from source code analysis.

Usage:
    python docs/dev/scripts/generate-provenance-registry.py [--repo-root PATH] [--write]

Scans the dashboard generator Python files and creates a mapping
from output artifact fields to their generating functions.
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


CORE_FILES: list[tuple[str, str]] = [
    ("generator", "build/toolchains/run/testing/foundation_dll_audit_generator.py"),
    ("kernel", "build/toolchains/run/testing/foundation_dll/verification_kernel.py"),
    ("claims", "build/toolchains/run/testing/foundation_dll/family_verification_claims.py"),
    ("gap", "build/toolchains/run/testing/foundation_dll/gap_analyzer.py"),
]

OUTPUT_DIR = "verification/projections/foundation-dll-audit"


class _Analyzer(ast.NodeVisitor):
    """Walk Python AST to find functions that construct output dicts."""

    def __init__(self) -> None:
        self.functions: dict[str, dict[str, Any]] = {}
        self.returns: list[tuple[str, int, set[str]]] = []  # (func_name, lineno, keys)

    def visit_FunctionDef(self, node: ast.FunctionDef) -> None:
        entry: dict[str, Any] = {
            "lineno": node.lineno,
            "args": [a.arg for a in node.args.args],
            "calls": [],
            "return_keys": set(),
        }
        for child in ast.walk(node):
            if isinstance(child, ast.Call) and isinstance(child.func, ast.Name):
                entry["calls"].append(child.func.id)
            if isinstance(child, ast.Return) and isinstance(child.value, ast.Dict):
                for key_node in child.value.keys:
                    if isinstance(key_node, ast.Constant) and isinstance(key_node.value, str):
                        entry["return_keys"].add(key_node.value)
            # Handle asdict() / dataclass field references
            if isinstance(child, ast.Call) and isinstance(child.func, ast.Attribute):
                if child.func.attr == "to_dict":
                    entry["calls"].append("to_dict")
        self.functions[node.name] = entry
        if entry["return_keys"]:
            self.returns.append((node.name, node.lineno, entry["return_keys"]))
        self.generic_visit(node)


def _collect_docstring_desc(tree: ast.AST) -> dict[str, str]:
    """Extract first-line docstring from each function."""
    desc: dict[str, str] = {}
    for node in ast.walk(tree):
        if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
            doc = ast.get_docstring(node)
            if doc:
                desc[node.name] = doc.split("\n")[0]
    return desc


def _load_output_fields(repo_root: Path) -> dict[str, set[str]]:
    """Walk actual output JSON files to discover field names."""
    out_root = repo_root / OUTPUT_DIR
    fields_by_artifact: dict[str, set[str]] = {}

    for artifact_file in [
        "program.json",
        "dll-matrix.json",
        "family-verification.json",
    ]:
        path = out_root / artifact_file
        if not path.is_file():
            continue
        with open(path, encoding="utf-8") as f:
            data = json.load(f)
        collected: set[str] = set()
        _walk_json_fields(data, "", collected)
        fields_by_artifact[artifact_file] = collected

    # DLL detail
    dll_dir = out_root / "dlls"
    if dll_dir.is_dir():
        files = sorted(dll_dir.glob("*.json"))
        if files:
            with open(files[0], encoding="utf-8") as f:
                data = json.load(f)
            collected = set()
            _walk_json_fields(data, "", collected)
            fields_by_artifact["dll-detail.json"] = collected

    return fields_by_artifact


def _walk_json_fields(obj: Any, prefix: str, out: set[str]) -> None:
    if isinstance(obj, dict):
        for key, value in obj.items():
            current = f"{prefix}.{key}" if prefix else key
            out.add(current)
            _walk_json_fields(value, current, out)
    elif isinstance(obj, list) and obj:
        if isinstance(obj[0], dict):
            _walk_json_fields(obj[0], f"{prefix}[]", out)


def generate_registry(repo_root: Path) -> str:
    analyzers: dict[str, _Analyzer] = {}
    docstrings: dict[str, dict[str, str]] = {}

    for label, rel_path in CORE_FILES:
        full = repo_root / rel_path
        if not full.is_file():
            continue
        tree = ast.parse(full.read_text(encoding="utf-8"))
        analyzer = _Analyzer()
        analyzer.visit(tree)
        analyzers[label] = analyzer
        docstrings[label] = _collect_docstring_desc(tree)

    output_fields = _load_output_fields(repo_root)

    # Build markdown
    lines: list[str] = []
    lines.append("# Foundation DLL Audit 数据来源注册表")
    lines.append("")
    lines.append("> 由 `docs/dev/scripts/generate-provenance-registry.py` 自动生成 baseline。")
    lines.append("> 人工审查确认后提交 commit。后续改 dashboard 代码后须 diff 检查。")
    lines.append("")

    # Section 1: Function inventory
    lines.append("## 1. 函数清单")
    lines.append("")
    for label, analyzer in analyzers.items():
        lines.append(f"### {label}")
        lines.append("")
        lines.append("| 函数 | 行号 | 参数 | 调用函数 | 返回的 dict keys |")
        lines.append("|------|------|------|---------|-----------------|")
        for fname, info in sorted(analyzer.functions.items(), key=lambda x: x[1]["lineno"]):
            rkeys = ", ".join(sorted(info["return_keys"])[:10])
            if len(info["return_keys"]) > 10:
                rkeys += "..."
            calls = ", ".join(info["calls"][:5])
            if len(info["calls"]) > 5:
                calls += "..."
            doc = docstrings.get(label, {}).get(fname, "")
            lines.append(
                f"| {fname} | L{info['lineno']} | {', '.join(info['args'][:4])} | {calls} | {rkeys} |"
            )
        lines.append("")

    # Section 2: Field provenance table
    lines.append("## 2. 字段来源映射表")
    lines.append("")
    lines.append("| 字段 | 出现工件 | 定义函数 | 数据来源链路 | 枚举值 |")
    lines.append("|------|---------|---------|------------|--------|")

    # Build reverse lookup: field key -> potential source functions
    field_to_func: dict[str, list[tuple[str, str, int]]] = {}
    for label, analyzer in analyzers.items():
        for fname, info in analyzer.functions.items():
            for key in info["return_keys"]:
                field_to_func.setdefault(key, []).append((label, fname, info["lineno"]))

    all_fields_seen: set[str] = set()
    for artifact_name, fields in output_fields.items():
        for field_path in sorted(fields):
            if field_path in all_fields_seen:
                continue
            all_fields_seen.add(field_path)
            # Extract leaf key
            leaf = field_path.replace("[]", "").split(".")[-1]
            funcs = field_to_func.get(leaf, [])
            func_str = "; ".join(f"{l}/{f} [L{n}]" for l, f, n in funcs[:3]) if funcs else "(直接赋值)"
            lines.append(
                f"| `{field_path}` | {artifact_name} | {func_str} | (待补充) | (待补充) |"
            )

    lines.append("")
    lines.append("---")
    lines.append("*基线版本 v1，生成时间: (请填写日期)*")
    lines.append("")

    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo-root", type=Path, default=_find_project_root())
    parser.add_argument("--write", action="store_true", help="Write registry to wiki")
    args = parser.parse_args()

    repo_root = args.repo_root.resolve()
    registry = generate_registry(repo_root)

    output_path = (
        repo_root
        / "wiki"
        / "03-功能模块"
        / "05-工具能力"
        / "foundation-dll-audit-provenance-registry.md"
    )

    if args.write:
        output_path.write_text(registry, encoding="utf-8")
        print(f"Registry baseline written to {output_path.relative_to(repo_root)}")
    else:
        print(registry)


if __name__ == "__main__":
    main()
