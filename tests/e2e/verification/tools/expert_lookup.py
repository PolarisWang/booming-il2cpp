#!/usr/bin/env python3
"""expert_lookup.py — 确定性 Expert 查找工具。

从 expert-registry.json 中读取领域定义，按域编号或关键词匹配合适的 Expert。

用法:
    # 按域编号查找
    python expert_lookup.py --domain 6
    python expert_lookup.py --domain 6 --verbose

    # 按关键词查找（返回所有匹配的 Expert）
    python expert_lookup.py --keyword "编译失败"
    python expert_lookup.py --keyword "GC" --verbose

    # 列出所有域
    python expert_lookup.py --list-domains

    # 列出所有 Expert
    python expert_lookup.py --list-experts

    # 查找子控制器
    python expert_lookup.py --controller codegen-ctl
"""

from __future__ import annotations

import json
import sys
from pathlib import Path


def _repo_root() -> Path:
    """Repository root."""
    return Path(__file__).resolve().parents[4]


def _registry_path() -> Path:
    return _repo_root() / ".ai" / "skills" / "discovery" / "expert-registry.json"


def load_registry() -> dict:
    """Load expert-registry.json, with helpful error on failure."""
    path = _registry_path()
    if not path.exists():
        print(f"ERROR: expert-registry.json not found at {path}", file=sys.stderr)
        sys.exit(1)
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as e:
        print(f"ERROR: Failed to parse {path}: {e}", file=sys.stderr)
        sys.exit(1)


def lookup_by_domain(registry: dict, domain_id: str, verbose: bool = False) -> list[dict]:
    """Look up expert(s) by domain ID (string like '6').

    Returns the default expert and any alternate experts.
    """
    domains = registry.get("domains", {})
    domain = domains.get(domain_id)
    if not domain:
        print(f"ERROR: Domain '{domain_id}' not found in registry")
        print(f"  Valid domain IDs: {', '.join(sorted(domains.keys(), key=int))}")
        return []

    default = domain["defaultExpert"]
    alternates = domain.get("alternateExperts", [])
    all_experts = [default] + alternates

    if verbose:
        print(f"Domain {domain_id}: {domain['label']} ({domain['name']})")
        print(f"  Description: {domain['description']}")
        print(f"  Default expert: {default}")
        if alternates:
            print(f"  Alternate experts: {', '.join(alternates)}")
        print(f"  Sub-controller: {_controller_for_domain(registry, domain_id)}")

    # Return structured results
    results = []
    for expert in all_experts:
        results.append({
            "expert": expert,
            "domain_id": domain_id,
            "domain_name": domain["name"],
            "domain_label": domain["label"],
            "is_default": expert == default,
            "keywords": registry.get("expert_keywords", {}).get(expert, []),
            "sub_controller": _controller_for_domain(registry, domain_id),
        })
    return results


def lookup_by_keyword(registry: dict, keyword: str, verbose: bool = False) -> list[dict]:
    """Look up expert(s) by keyword matching.

    Matches against expert_keywords in the registry.
    Returns all matching experts, ordered by relevance (most keywords matched first).
    """
    expert_keywords = registry.get("expert_keywords", {})
    keyword_lower = keyword.lower()

    # Score each expert by number of matching keywords
    scored: list[tuple[int, str, list[str]]] = []
    for expert, kw_list in expert_keywords.items():
        matches = [kw for kw in kw_list if keyword_lower in kw.lower()]
        if matches:
            scored.append((len(matches), expert, matches))

    # Sort by match count descending
    scored.sort(key=lambda x: -x[0])

    if not scored:
        if verbose:
            print(f"No experts matched keyword '{keyword}'")
        return []

    results = []
    for count, expert, matches in scored:
        if verbose:
            controller = _controller_for_expert(registry, expert)
            print(f"  {expert} ({count} keyword(s) matched: {', '.join(matches)})"
                  + (f" — controller: {controller}" if controller else ""))
        results.append({
            "expert": expert,
            "match_count": count,
            "matched_keywords": matches,
            "sub_controller": _controller_for_expert(registry, expert),
        })

    return results


def _controller_for_domain(registry: dict, domain_id: str) -> str | None:
    """Find sub-controller name for a domain ID."""
    index = registry.get("expert_sub_controller_domain_index", {})
    return index.get(domain_id)


def _controller_for_expert(registry: dict, expert_name: str) -> str | None:
    """Find sub-controller name for an expert."""
    for ctl_name, ctl_data in registry.get("expert_sub_controller", {}).items():
        if expert_name in ctl_data.get("experts", []):
            return ctl_name
    return None


def list_domains(registry: dict, verbose: bool = False) -> None:
    """List all domains in the registry."""
    domains = registry.get("domains", {})
    print(f"{'ID':<5} {'Name':<25} {'Label':<12} {'Default Expert':<40} {'Controller'}")
    print("-" * 100)
    for did in sorted(domains.keys(), key=int):
        d = domains[did]
        ctl = _controller_for_domain(registry, did) or "-"
        print(f"{did:<5} {d['name']:<25} {d['label']:<12} {d['defaultExpert']:<40} {ctl}")


def list_experts(registry: dict, verbose: bool = False) -> None:
    """List all experts and their keywords."""
    expert_keywords = registry.get("expert_keywords", {})
    print(f"{'Expert':<45} {'Sub-controller':<20} {'Keywords'}")
    print("-" * 120)
    for expert in sorted(expert_keywords.keys()):
        ctl = _controller_for_expert(registry, expert) or "-"
        kw = ", ".join(expert_keywords[expert][:5])
        if len(expert_keywords[expert]) > 5:
            kw += " ..."
        print(f"{expert:<45} {ctl:<20} {kw}")


def lookup_controller(registry: dict, ctl_name: str, verbose: bool = False) -> None:
    """List all experts in a sub-controller."""
    controllers = registry.get("expert_sub_controller", {})
    ctl = controllers.get(ctl_name)
    if not ctl:
        print(f"ERROR: Controller '{ctl_name}' not found")
        print(f"  Valid controllers: {', '.join(sorted(controllers.keys()))}")
        return

    print(f"Controller: {ctl_name}")
    print(f"  Domains: {ctl.get('domains', [])}")
    print(f"  Experts ({len(ctl['experts'])}):")
    for expert in ctl["experts"]:
        kw = registry.get("expert_keywords", {}).get(expert, [])
        kw_str = ", ".join(kw[:3]) + ("..." if len(kw) > 3 else "")
        print(f"    - {expert}")
        if verbose and kw_str:
            print(f"      keywords: {kw_str}")


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(
        description="Expert Lookup — 确定性 Expert 查找工具",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )

    # Exclusive modes
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--domain", type=str, default=None,
                      help="Domain ID to look up (e.g. '6' or 'translation')")
    mode.add_argument("--keyword", type=str, default=None,
                      help="Keyword to match against expert keywords")
    mode.add_argument("--list-domains", action="store_true",
                      help="List all domains")
    mode.add_argument("--list-experts", action="store_true",
                      help="List all experts and their keywords")
    mode.add_argument("--controller", type=str, default=None,
                      help="List all experts in a sub-controller")

    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose output")
    parser.add_argument("--format", choices=["text", "json"], default="text",
                        help="Output format (default: text)")

    args = parser.parse_args()
    registry = load_registry()

    # ── List domains ──
    if args.list_domains:
        list_domains(registry, args.verbose)
        return 0

    # ── List experts ──
    if args.list_experts:
        list_experts(registry, args.verbose)
        return 0

    # ── Controller lookup ──
    if args.controller:
        lookup_controller(registry, args.controller, args.verbose)
        return 0

    # ── Domain lookup ──
    if args.domain:
        results = lookup_by_domain(registry, args.domain, args.verbose)
        if not results:
            return 1

        if args.format == "json":
            print(json.dumps(results, indent=2, ensure_ascii=False))
        else:
            print(f"→ {results[0]['expert']}")
            if results[0]["sub_controller"]:
                print(f"  controller: {results[0]['sub_controller']}")
        return 0

    # ── Keyword lookup ──
    if args.keyword:
        results = lookup_by_keyword(registry, args.keyword, args.verbose)

        if args.format == "json":
            print(json.dumps(results, indent=2, ensure_ascii=False))
        elif results:
            print(f"Experts matching '{args.keyword}':")
            for r in results:
                print(f"  → {r['expert']}")
                if r["sub_controller"]:
                    print(f"     controller: {r['sub_controller']}")
        else:
            print(f"No experts matched keyword '{args.keyword}'")
            return 1
        return 0

    parser.print_help()
    return 0


if __name__ == "__main__":
    sys.exit(main())
