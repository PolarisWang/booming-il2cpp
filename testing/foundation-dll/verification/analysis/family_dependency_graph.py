"""family_dependency_graph — Family 依赖图分析工具.

扫描所有 family 的 contract 和 codegen 输出，检测 subject 代码重叠/共享，
输出依赖图（dot / mermaid / json 格式）。

Usage:
    python -m verification.analysis.family_dependency_graph [options]

Options:
    --assembly NAME      目标 assembly (default System.Private.CoreLib)
    --output FORMAT      dot | mermaid | json | table (default table)
    --min-jaccard N      最小 Jaccard 阈值，低于此值不显示 (default 0.02)
    --top N              只显示前 N 个依赖对 (default 全部)
    --type-overlap       切换到按类型重叠分析而非方法重叠分析 (default method-level)
    --shared-subjects    只显示共享 subject 列表
"""

from __future__ import annotations

import argparse
import json
import sys
from collections import defaultdict
from pathlib import Path
from typing import Any

# ── Constants ──────────────────────────────────────────────────────────

_FOUNDATION_DLL = Path(__file__).resolve().parents[2]
"""testing/foundation-dll/"""

DEFAULT_ASSEMBLY = "System.Private.CoreLib"
DEFAULT_MIN_JACCARD = 0.02


# ── Data Loading ──────────────────────────────────────────────────────

def load_all_families(assembly: str) -> dict[str, dict[str, Any]]:
    """Load all family contracts from the assembly directory."""
    assembly_dir = _FOUNDATION_DLL / assembly
    if not assembly_dir.exists():
        print(f"Error: assembly directory not found: {assembly_dir}", file=sys.stderr)
        sys.exit(1)

    families: dict[str, dict[str, Any]] = {}
    for d in sorted(assembly_dir.iterdir()):
        if not d.is_dir():
            continue
        # Skip non-family directories
        if d.name in ("reports", "results"):
            continue

        contract_file = d / "capability-family-contract.json"
        if not contract_file.exists():
            continue

        try:
            data = json.loads(contract_file.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError) as e:
            print(f"  [warn] failed to parse {contract_file}: {e}", file=sys.stderr)
            continue

        subjects = data.get("methodSubjectIds", [])
        families[d.name] = {
            "displayName": data.get("displayName", d.name),
            "subjectCount": len(subjects),
            "subjects": subjects,
            "ownerSubjectId": data.get("ownerSubjectId", ""),
            "familyId": data.get("familyId", ""),
        }

    return families


# ── Analysis ──────────────────────────────────────────────────────────

def compute_subject_overlap(
    families: dict[str, dict[str, Any]],
) -> list[tuple[float, str, str, int, int, int]]:
    """Compute Jaccard similarity on methodSubjectIds between all family pairs.

    Returns list of (jaccard, f1, f2, intersection_size, union_size, max_size).
    """
    fnames = sorted(families.keys())
    results: list[tuple[float, str, str, int, int, int]] = []

    for i in range(len(fnames)):
        for j in range(i + 1, len(fnames)):
            a = set(families[fnames[i]]["subjects"])
            b = set(families[fnames[j]]["subjects"])
            if not a or not b:
                continue

            intersection = a & b
            if not intersection:
                continue

            union = a | b
            jaccard = len(intersection) / len(union)
            results.append((jaccard, fnames[i], fnames[j], len(intersection), len(union), max(len(a), len(b))))

    results.sort(key=lambda x: -x[0])
    return results


def compute_type_overlap(
    families: dict[str, dict[str, Any]],
) -> list[tuple[float, str, str, int, set[str]]]:
    """Compute type-level overlap (same .NET type tested by both families)."""
    type_to_families: defaultdict[str, set[str]] = defaultdict(set)

    for fname, finfo in families.items():
        seen = set()
        for subj in finfo["subjects"]:
            # Extract type name from subjectId: "Assembly/Namespace.Type::Method:..."
            assembly_type = subj.split("::")[0] if "::" in subj else subj
            type_name = assembly_type.split("/")[-1] if "/" in assembly_type else assembly_type
            if type_name not in seen:
                seen.add(type_name)
                type_to_families[type_name].add(fname)

    # For each family pair, find shared types
    fnames = sorted(families.keys())
    results: list[tuple[float, str, str, int, set[str]]] = []

    for i in range(len(fnames)):
        for j in range(i + 1, len(fnames)):
            types_a = {t for t, fs in type_to_families.items() if fnames[i] in fs}
            types_b = {t for t, fs in type_to_families.items() if fnames[j] in fs}
            shared = types_a & types_b
            if shared:
                jaccard = len(shared) / len(types_a | types_b) if (types_a | types_b) else 0
                results.append((jaccard, fnames[i], fnames[j], len(shared), shared))

    results.sort(key=lambda x: -x[0])
    return results


def find_shared_subjects(
    families: dict[str, dict[str, Any]],
) -> dict[str, list[str]]:
    """Find all subjects shared by multiple families."""
    subject_to_families: defaultdict[str, list[str]] = defaultdict(list)
    for fname, finfo in families.items():
        for subj in finfo["subjects"]:
            subject_to_families[subj].append(fname)

    return {s: fs for s, fs in subject_to_families.items() if len(fs) > 1}


def find_shared_types(
    families: dict[str, dict[str, Any]],
) -> dict[str, set[str]]:
    """Find all .NET types tested by multiple families."""
    type_to_families: defaultdict[str, set[str]] = defaultdict(set)
    for fname, finfo in families.items():
        seen = set()
        for subj in finfo["subjects"]:
            assembly_type = subj.split("::")[0] if "::" in subj else subj
            type_name = assembly_type.split("/")[-1] if "/" in assembly_type else assembly_type
            if type_name not in seen:
                seen.add(type_name)
                type_to_families[type_name].add(fname)

    return {t: fs for t, fs in type_to_families.items() if len(fs) > 1}


# ── Output Formatters ─────────────────────────────────────────────────

def format_table(results: list, families: dict, min_jaccard: float, top_n: int | None = None) -> str:
    """Format as human-readable table."""
    lines: list[str] = []
    lines.append(f"{'Family 1':50s} {'Family 2':50s} {'Jaccard':8s} {'Shared':7s} {'Union':6s}")
    lines.append("-" * 125)

    displayed = 0
    for jac, f1, f2, shared, union, *_ in results:
        if jac < min_jaccard:
            break
        if top_n and displayed >= top_n:
            break
        s1 = families[f1]["subjectCount"]
        s2 = families[f2]["subjectCount"]
        lines.append(f"{f'{f1} ({s1})':50s} {f'{f2} ({s2})':50s} {jac:.4f}   {shared:3d}      {union:3d}")
        displayed += 1

    if not displayed:
        lines.append("  (no dependencies above threshold)")
    return "\n".join(lines)


def format_mermaid(results: list, families: dict, min_jaccard: float, top_n: int | None = None) -> str:
    """Format as Mermaid.js graph."""
    lines: list[str] = []
    lines.append("```mermaid")
    lines.append("graph LR")
    lines.append("    %% Family Dependency Graph")

    # Collect all families that appear
    involved: set[str] = set()
    edges: list[tuple[str, str, float, int]] = []
    displayed = 0
    for jac, f1, f2, shared, union, *_ in results:
        if jac < min_jaccard:
            break
        if top_n and displayed >= top_n:
            break
        involved.add(f1)
        involved.add(f2)
        edges.append((f1, f2, jac, shared))
        displayed += 1

    # Node definitions
    for fname in sorted(involved):
        info = families[fname]
        label = f"{fname}|{info['subjectCount']}subj"
        lines.append(f'    {_safe_id(fname)}["{label}"]')

    # Edge definitions
    for f1, f2, jac, shared in edges:
        label = f"J={jac:.3f}|{shared}"
        lines.append(f"    {_safe_id(f1)} ---|{label}| {_safe_id(f2)}")

    lines.append("```")
    return "\n".join(lines)


def format_dot(results: list, families: dict, min_jaccard: float, top_n: int | None = None) -> str:
    """Format as Graphviz DOT."""
    lines: list[str] = []
    lines.append("digraph FamilyDependencies {")
    lines.append("    rankdir=LR;")
    lines.append("    node [shape=box, style=rounded];")
    lines.append("    edge [color=gray50];")
    lines.append("")

    involved: set[str] = set()
    edges: list[tuple[str, str, float, int]] = []
    displayed = 0
    for jac, f1, f2, shared, union, *_ in results:
        if jac < min_jaccard:
            break
        if top_n and displayed >= top_n:
            break
        involved.add(f1)
        involved.add(f2)
        edges.append((f1, f2, jac, shared))
        displayed += 1

    for fname in sorted(involved):
        info = families[fname]
        label = f"{fname}\\n({info['subjectCount']} subjects)"
        lines.append(f'    "{fname}" [label="{label}"];')

    for f1, f2, jac, shared in edges:
        penwidth = max(0.5, jac * 5)
        lines.append(f'    "{f1}" -> "{f2}" [label="J={jac:.3f}\\n{shared} methods", penwidth={penwidth:.1f}];')

    lines.append("}")
    return "\n".join(lines)


def format_json(results: list, families: dict, min_jaccard: float, top_n: int | None = None) -> str:
    """Format as JSON."""
    output: dict[str, Any] = {
        "assembly": DEFAULT_ASSEMBLY,
        "totalFamilies": len(families),
        "dependencies": [],
    }

    displayed = 0
    for jac, f1, f2, shared, union, max_sz in results:
        if jac < min_jaccard:
            break
        if top_n and displayed >= top_n:
            break
        output["dependencies"].append({
            "family1": f1,
            "family1Subjects": families[f1]["subjectCount"],
            "family2": f2,
            "family2Subjects": families[f2]["subjectCount"],
            "jaccard": round(jac, 4),
            "sharedSubjects": shared,
            "unionSubjects": union,
            "strength": "strong" if jac >= 0.15 else ("medium" if jac >= 0.05 else "weak"),
        })
        displayed += 1

    return json.dumps(output, indent=2, ensure_ascii=False)


def format_shared_subjects(shared: dict[str, list[str]]) -> str:
    """Format shared subject list as table."""
    lines: list[str] = []
    lines.append(f"{'Method Signature':80s} {'Families'}")
    lines.append("-" * 120)
    for subj, fs in sorted(shared.items(), key=lambda x: -len(x[1])):
        lines.append(f"{subj[:78]:80s} {', '.join(fs)}")
    return "\n".join(lines)


def format_shared_types(shared_types: dict[str, set[str]]) -> str:
    """Format shared type list as table sorted by family count."""
    lines: list[str] = []
    lines.append(f"{'.NET Type':40s} {'Families':7s} {'Family Names'}")
    lines.append("-" * 100)
    for t, fs in sorted(shared_types.items(), key=lambda x: -len(x[1])):
        lines.append(f"{t:40s} {len(fs):3d}      {', '.join(sorted(fs))}")
    return "\n".join(lines)


def _safe_id(name: str) -> str:
    """Create a safe graph node ID from a family name."""
    return name.replace("-", "_").replace(",", "_").replace(".", "_")


# ── CLI ───────────────────────────────────────────────────────────────

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Family dependency graph analysis for foundation-dll assemblies",
    )
    parser.add_argument(
        "--assembly", default=DEFAULT_ASSEMBLY,
        help=f"Target assembly (default: {DEFAULT_ASSEMBLY})",
    )
    parser.add_argument(
        "--output", choices=["dot", "mermaid", "json", "table", "shared-subjects", "shared-types"],
        default="table",
        help="Output format (default: table)",
    )
    parser.add_argument(
        "--min-jaccard", type=float, default=DEFAULT_MIN_JACCARD,
        help=f"Minimum Jaccard threshold (default: {DEFAULT_MIN_JACCARD})",
    )
    parser.add_argument(
        "--top", type=int, default=None,
        help="Limit to top N dependency pairs (default: all)",
    )
    parser.add_argument(
        "--type-overlap", action="store_true",
        help="Analyze type-level overlap instead of method-level",
    )

    args = parser.parse_args()

    # Load data
    families = load_all_families(args.assembly)

    if not families:
        print(f"No families found for assembly '{args.assembly}'", file=sys.stderr)
        sys.exit(1)

    # Handle special output modes
    if args.output == "shared-subjects":
        shared = find_shared_subjects(families)
        print(f"Total shared subjects: {len(shared)}")
        print()
        print(format_shared_subjects(shared))
        return

    if args.output == "shared-types":
        shared_types = find_shared_types(families)
        print(f"Total shared types: {len(shared_types)}")
        print()
        print(format_shared_types(shared_types))
        return

    # Compute overlap
    if args.type_overlap:
        results = compute_type_overlap(families)
    else:
        results = compute_subject_overlap(families)

    if not results:
        print("No dependencies found (all families are independent)")
        return

    # Format output
    formatters = {
        "table": format_table,
        "mermaid": format_mermaid,
        "dot": format_dot,
        "json": format_json,
    }

    output = formatters[args.output](results, families, args.min_jaccard, args.top)
    print(output)


if __name__ == "__main__":
    main()
