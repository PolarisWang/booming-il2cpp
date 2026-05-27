"""Cross-platform verification aggregator — scans per-platform family-verification outputs
and produces cross-platform-matrix.json + cross-platform-dashboard.html.

Usage:
    python cross_platform_dashboard.py
        (scans docs/verification/foundation-dll-audit/*/ for per-platform data)

Output:
    docs/verification/foundation-dll-audit/aggregate/
        cross-platform-matrix.json
        cross-platform-dashboard.html
"""

from __future__ import annotations

import json
import sys
from html import escape
from pathlib import Path
from typing import Any

from build.toolchains.run.runtime import detect_host_platform

# ── Platform buckets ──────────────────────────────────────────────────────

PLATFORM_BUCKETS: list[dict[str, Any]] = [
    {
        "name": "desktop",
        "displayName": "Desktop",
        "platforms": ["windows-x64", "linux-x64", "macos-arm64", "macos-x64"],
        "aggregateRule": "all-pass",
    },
    {
        "name": "mobile",
        "displayName": "Mobile",
        "platforms": ["android-arm64", "ios-arm64"],
        "aggregateRule": "any-pass",
    },
]


def _load_json(path: Path) -> dict[str, Any] | None:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError):
        return None


def _discover_platforms(audit_root: Path) -> list[str]:
    """Scan audit_root for platform directories (skip aggregate/, dlls/, and other known non-platform dirs)."""
    platforms: list[str] = []
    if not audit_root.is_dir():
        return platforms
    # Known non-platform directories to skip
    skip_dirs = {"aggregate", "dlls"}
    for entry in sorted(audit_root.iterdir()):
        if entry.is_dir() and entry.name not in skip_dirs:
            platforms.append(entry.name)
    return platforms


def _extract_gate_status(family: dict[str, Any]) -> dict[str, str]:
    """Extract all verification gate statuses from a family record."""
    gates: dict[str, str] = {}
    vg = dict(family.get("verificationGates") or {})
    for code, state in vg.items():
        gates[str(code)] = str(state)
    return gates


def _extract_benchmark_info(family: dict[str, Any]) -> dict[str, Any]:
    """Extract benchmark summary from a family record."""
    bp = dict(family.get("benchmarkProof") or {})
    avg = bp.get("averageSpeedupPercent")
    return {
        "status": str(bp.get("status", "pending")),
        "averageSpeedupPercent": round(float(avg), 2) if avg is not None else None,
        "nativeFasterCount": int(bp.get("nativeFasterCount") or 0),
        "managedFasterCount": int(bp.get("managedFasterCount") or 0),
    }


def _compute_bucket_status(
    per_platform: dict[str, dict[str, Any]],
    bucket: dict[str, Any],
) -> str:
    """Compute aggregate status for a platform bucket.

    'all-pass': all platforms in bucket must have closureStatus == 'closed'.
    'any-pass': at least one platform in bucket must have closureStatus == 'closed'.
    """
    bucket_platforms = list(bucket.get("platforms", []))
    rule = str(bucket.get("aggregateRule", "all-pass"))

    statuses = []
    for plat in bucket_platforms:
        plat_data = per_platform.get(plat)
        if plat_data is None:
            statuses.append("missing")
        else:
            statuses.append(str(plat_data.get("closureStatus", "missing")))

    if rule == "all-pass":
        return "passed" if all(s == "closed" for s in statuses) else "blocked"
    elif rule == "any-pass":
        return "passed" if any(s == "closed" for s in statuses) else "blocked"
    return "unknown"


def build_cross_platform_matrix(audit_root: Path) -> dict[str, Any]:
    """Scan per-platform verification outputs and build a cross-platform matrix.

    Args:
        audit_root: Root of per-platform audit directories
                    (e.g., docs/verification/foundation-dll-audit/).

    Returns:
        Cross-platform matrix dict.
    """
    platforms = _discover_platforms(audit_root)
    if not platforms:
        print(f"WARNING: no platform directories found in {audit_root}", file=sys.stderr)
        return {
            "schemaVersion": 1,
            "platforms": [],
            "platformBuckets": PLATFORM_BUCKETS,
            "families": [],
        }

    print(f"Discovered platforms: {', '.join(platforms)}")

    # ── Load per-platform family-verification.json ──
    platform_families: dict[str, list[dict[str, Any]]] = {}
    platform_programs: dict[str, dict[str, Any]] = {}
    for plat in platforms:
        fv_path = audit_root / plat / "family-verification.json"
        fv_data = _load_json(fv_path)
        prog_path = audit_root / plat / "program.json"
        prog_data = _load_json(prog_path)
        if fv_data is None:
            print(f"  WARNING: family-verification.json not found for platform '{plat}'")
            platform_families[plat] = []
        else:
            platform_families[plat] = (
                list(list(fv_data.get("assemblies") or [{}])[0].get("families") or [])
                if fv_data.get("assemblies")
                else []
            )
        platform_programs[plat] = prog_data or {}

    # ── Build unified family index across all platforms ──
    # Key: familyId → { displayName, perPlatform: { plat → data } }
    all_family_ids: dict[str, dict[str, Any]] = {}
    for plat, families in platform_families.items():
        for family in families:
            fid = str(family.get("familyId") or "")
            if not fid:
                continue
            if fid not in all_family_ids:
                all_family_ids[fid] = {
                    "familyId": fid,
                    "displayName": str(family.get("displayName") or fid.rsplit("/", 1)[-1]),
                    "perPlatform": {},
                }
            per_plat: dict[str, Any] = {
                "closureStatus": str(family.get("closureStatus", "missing")),
                "denominatorStatus": str(family.get("denominatorStatus", "")),
                "methodCount": int(family.get("methodCount") or 0),
                "verificationGates": _extract_gate_status(family),
                "benchmark": _extract_benchmark_info(family),
            }
            all_family_ids[fid]["perPlatform"][plat] = per_plat

    # ── Compute bucket status ──
    families_list = []
    for fid, entry in all_family_ids.items():
        bucket_status: dict[str, str] = {}
        for bucket in PLATFORM_BUCKETS:
            bucket_status[str(bucket["name"])] = _compute_bucket_status(
                entry["perPlatform"], bucket
            )
        entry["bucketStatus"] = bucket_status
        families_list.append(entry)

    families_list.sort(key=lambda f: f["familyId"])

    # ── Per-platform summary ──
    platform_summaries: dict[str, dict[str, Any]] = {}
    for plat in platforms:
        families = platform_families.get(plat, [])
        total = len(families)
        closed = sum(1 for f in families if f.get("closureStatus") == "closed")
        platform_summaries[plat] = {
            "totalFamilies": total,
            "closedFamilies": closed,
            "inProgressFamilies": sum(1 for f in families if f.get("closureStatus") in ("in-progress", "")),
            "blockedFamilies": sum(1 for f in families if f.get("closureStatus") == "platform-blocked"),
        }

    host_platform = detect_host_platform()
    parts = host_platform.split("-", 1)

    matrix = {
        "schemaVersion": 1,
        "platform": host_platform,
        "os": parts[0] if len(parts) > 0 else "unknown",
        "arch": parts[1] if len(parts) > 1 else "unknown",
        "platforms": platforms,
        "platformBuckets": PLATFORM_BUCKETS,
        "platformSummaries": platform_summaries,
        "families": families_list,
    }

    print(f"  Families: {len(families_list)}, Platforms: {len(platforms)}")
    return matrix


def _render_cross_platform_dashboard(matrix: dict[str, Any], root_prefix: str) -> str:
    """Render a cross-platform dashboard HTML page."""
    platforms = list(matrix.get("platforms", []))
    bucket_defs = list(matrix.get("platformBuckets", []))
    families = list(matrix.get("families", []))
    plat_summaries = dict(matrix.get("platformSummaries", {}))

    # ── Styles (reuse dashboard.css principles) ──
    styles = """
    <style>
      * { box-sizing: border-box; margin: 0; padding: 0; }
      body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
             background: #0d1117; color: #e6edf3; margin: 0; padding: 20px; }
      h1 { font-size: 24px; font-weight: 600; margin-bottom: 4px; }
      .eyebrow { font-size: 12px; text-transform: uppercase; letter-spacing: 1px; color: #8b949e; }
      .page-header { max-width: 1400px; margin: 0 auto 20px; }
      .top-nav { margin: 12px 0; display: flex; gap: 8px; flex-wrap: wrap; }
      .top-nav a { color: #58a6ff; text-decoration: none; font-size: 13px; padding: 4px 10px;
                   border: 1px solid #30363d; border-radius: 6px; }
      .top-nav a:hover { background: #161b22; }
      .tab-bar { display: flex; gap: 2px; margin: 16px 0; border-bottom: 1px solid #30363d; }
      .tab-button { padding: 8px 20px; border: none; background: none; color: #8b949e;
                    cursor: pointer; font-size: 14px; border-bottom: 2px solid transparent; }
      .tab-button.active { color: #f0f6fc; border-bottom-color: #f78166; }
      .tab-button:hover { color: #e6edf3; }
      .tab-content { display: none; max-width: 1400px; margin: 0 auto; }
      .tab-content.active { display: block; }
      table { width: 100%; border-collapse: collapse; font-size: 13px; }
      th { text-align: left; padding: 8px 10px; background: #161b22; border-bottom: 2px solid #30363d;
           font-weight: 600; color: #8b949e; font-size: 12px; text-transform: uppercase;
           white-space: nowrap; position: sticky; top: 0; z-index: 2; }
      td { padding: 8px 10px; border-bottom: 1px solid #21262d; white-space: nowrap; }
      tr:hover { background: #161b22 !important; }
      .plat-col { background: #0d1117 !important; }
      .plat-col:hover { background: #161b22 !important; }
      .badge-closed { background: #1b4123; color: #3fb950; padding: 3px 8px; border-radius: 12px;
                      font-size: 11px; font-weight: 600; }
      .badge-in-progress { background: #2d1d00; color: #d29922; padding: 3px 8px; border-radius: 12px;
                           font-size: 11px; font-weight: 600; }
      .badge-blocked, .badge-failed { background: #3d1515; color: #f85149; padding: 3px 8px;
                                      border-radius: 12px; font-size: 11px; font-weight: 600; }
      .badge-missing { background: #21262d; color: #8b949e; padding: 3px 8px; border-radius: 12px;
                       font-size: 11px; font-weight: 600; }
      .badge-passed { background: #1b4123; color: #3fb950; padding: 3px 8px; border-radius: 12px;
                      font-size: 11px; font-weight: 600; }
      .badge-pending { background: #21262d; color: #8b949e; padding: 3px 8px; border-radius: 12px;
                       font-size: 11px; font-weight: 600; }
      .badge-skip { background: #1c2128; color: #6e7681; padding: 3px 8px; border-radius: 12px;
                    font-size: 11px; font-weight: 600; }
      .summary-card { background: #161b22; border: 1px solid #30363d; border-radius: 8px;
                      padding: 16px; display: inline-block; margin: 8px; min-width: 200px; }
      .summary-card h3 { font-size: 14px; color: #8b949e; margin-bottom: 8px; }
      .summary-card .number { font-size: 32px; font-weight: 700; }
      .platform-cards { display: flex; flex-wrap: wrap; gap: 8px; margin: 16px 0; }
      .gate-indicators { display: flex; gap: 3px; }
      .gate-dot { width: 10px; height: 10px; border-radius: 50%; display: inline-block; }
      .gate-passed { background: #3fb950; }
      .gate-failed { background: #f85149; }
      .gate-pending { background: #8b949e; }
      .section-title { font-size: 16px; font-weight: 600; margin: 20px 0 12px; color: #f0f6fc; }
      .truncate { max-width: 250px; overflow: hidden; text-overflow: ellipsis; display: inline-block;
                  vertical-align: middle; }
      @media (max-width: 768px) {
        table, thead, tbody, th, td, tr { display: block; }
        th { display: none; }
        td { padding: 6px 8px; border: none; }
        td:before { content: attr(data-label); font-weight: 600; color: #8b949e;
                    display: inline-block; width: 100px; }
      }
    </style>
    """

    # ── Platform summary cards ──
    plat_cards_html = ""
    for plat in platforms:
        ps = plat_summaries.get(plat, {})
        total = ps.get("totalFamilies", 0)
        closed = ps.get("closedFamilies", 0)
        pct = round((closed / total) * 100, 1) if total > 0 else 0.0
        color = "#3fb950" if pct >= 90 else ("#d29922" if pct >= 50 else "#f85149")
        plat_cards_html += f"""
        <div class="summary-card">
          <h3>{escape(plat)}</h3>
          <div class="number" style="color:{color}">{pct}%</div>
          <div style="color:#8b949e;font-size:13px">{closed}/{total} families closed</div>
        </div>"""

    # ── Bucket row helper ──
    def _badge(status: str) -> str:
        mapping = {
            "closed": "badge-closed",
            "passed": "badge-passed",
            "in-progress": "badge-in-progress",
            "failed": "badge-failed",
            "blocked": "badge-blocked",
            "missing": "badge-missing",
            "pending": "badge-pending",
            "skip": "badge-skip",
            "not-required": "badge-skip",
        }
        cls = mapping.get(status, "badge-pending")
        return f'<span class="{cls}">{escape(status)}</span>'

    def _gate_dots(gates: dict[str, str]) -> str:
        dots = ""
        for gate_state in gates.values():
            gcls = "gate-passed" if gate_state == "passed" else ("gate-failed" if gate_state == "failed" else "gate-pending")
            dots += f'<span class="gate-dot {gcls}" title="{escape(gate_state)}"></span>'
        return f'<div class="gate-indicators">{dots}</div>'

    # ── Tab 1: Overview ──
    overview_html = f"""
    <div id="tab-overview" class="tab-content active">
      <div class="platform-cards">{plat_cards_html}</div>
      <div class="section-title">Platform Buckets</div>
      <table>
        <thead><tr><th>Bucket</th><th>Platforms</th><th>Rule</th></tr></thead>
        <tbody>
          {''.join(
            f'<tr><td>{escape(b.get("displayName", b["name"]))}</td>'
            f'<td>{escape(", ".join(b["platforms"]))}</td>'
            f'<td>{escape(b["aggregateRule"])}</td></tr>'
            for b in bucket_defs
          )}
        </tbody>
      </table>
      <div class="section-title">Cross-Platform Summary</div>
      <table>
        <thead>
          <tr>
            <th>Family</th>
            <th>Desktop</th>
            <th>Mobile</th>
            {''.join(f'<th class="plat-col">{escape(p)}</th>' for p in platforms)}
          </tr>
        </thead>
        <tbody>
          {''.join(
            f'<tr>'
            f'<td><span class="truncate" title="{escape(f["familyId"])}">{escape(f.get("displayName", ""))}</span></td>'
            f'<td>{_badge(f.get("bucketStatus", {}).get("desktop", "missing"))}</td>'
            f'<td>{_badge(f.get("bucketStatus", {}).get("mobile", "missing"))}</td>'
            + ''.join(
                f'<td data-label="{escape(p)}">{_badge(f.get("perPlatform", {}).get(p, {}).get("closureStatus", "missing"))}</td>'
                for p in platforms
            )
            + '</tr>'
            for f in families
          )}
        </tbody>
      </table>
    </div>
    """

    # ── Tab 2: Per-Platform Matrix ──
    platform_tabs_html = ""
    for plat in platforms:
        rows = ""
        for family in families:
            pp = family.get("perPlatform", {}).get(plat, {})
            cs = str(pp.get("closureStatus", "missing"))
            gates = dict(pp.get("verificationGates", {}))
            bm = dict(pp.get("benchmark", {}))
            bm_speedup = bm.get("averageSpeedupPercent")
            bm_str = f"{bm_speedup:.1f}%" if bm_speedup is not None else "n/a"
            rows += f"""
            <tr>
              <td><span class="truncate" title="{escape(family["familyId"])}">{escape(family.get("displayName", ""))}</span></td>
              <td>{_badge(cs)}</td>
              <td>{_gate_dots(gates)}</td>
              <td>{_badge(bm.get("status", "pending"))} {escape(bm_str)}</td>
            </tr>"""
        platform_tabs_html += f"""
        <div id="tab-plat-{escape(plat)}" class="tab-content">
          <div class="section-title">Platform: {escape(plat)} — Gate Matrix</div>
          <table>
            <thead><tr><th>Family</th><th>Closure</th><th>Gates</th><th>Benchmark</th></tr></thead>
            <tbody>{rows}</tbody>
          </table>
        </div>"""

    # ── JavaScript for tabs ──
    plat_tab_buttons = "".join(
        f'<button class="tab-button" data-tab="plat-{escape(p)}" onclick="switchTab(\'plat-{escape(p)}\')">{escape(p)}</button>'
        for p in platforms
    )

    scripts = """
    <script>
      function switchTab(tabId) {
        document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
        document.querySelectorAll('.tab-button').forEach(el => el.classList.remove('active'));
        var tab = document.getElementById('tab-' + tabId);
        if (tab) tab.classList.add('active');
        var btn = document.querySelector('[data-tab="' + tabId + '"]');
        if (btn) btn.classList.add('active');
        if (!tab) {
          var tab2 = document.getElementById('tab-' + tabId);
          if (tab2) tab2.classList.add('active');
        }
      }
    </script>
    """

    return f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Cross-Platform Verification Dashboard</title>
  <link rel="stylesheet" href="{root_prefix}dashboard.css">
  {styles}
</head>
<body>
  <main>
    <div class="page-header">
      <div class="eyebrow">Multi-Platform Verification</div>
      <h1>Cross-Platform Foundation DLL Audit</h1>
      <p>Aggregated verification status across {len(platforms)} platforms, {len(families)} capability families.</p>
      <div class="top-nav">
        <a href="{root_prefix}index.html">Platform Index</a>
      </div>
      <div class="tab-bar">
        <button class="tab-button active" data-tab="overview" onclick="switchTab('overview')">Overview</button>
        {plat_tab_buttons}
      </div>
    </div>
    {overview_html}
    {platform_tabs_html}
  </main>
  {scripts}
</body>
</html>"""


def write_cross_platform_outputs(audit_root: Path) -> dict[str, Any]:
    """Scan per-platform directories and write cross-platform aggregation.

    Args:
        audit_root: Root of per-platform audit directories
                    (e.g., docs/verification/foundation-dll-audit/).

    Returns:
        Dict with output paths.
    """
    matrix = build_cross_platform_matrix(audit_root)
    aggregate_root = audit_root / "aggregate"
    aggregate_root.mkdir(parents=True, exist_ok=True)

    # Write JSON
    matrix_path = aggregate_root / "cross-platform-matrix.json"
    with open(matrix_path, "w", encoding="utf-8") as f:
        json.dump(matrix, f, indent=2, ensure_ascii=False)
        f.write("\n")
    print(f"Cross-platform matrix written to {matrix_path}")

    # Write index HTML for the aggregate page
    dashboard_path = aggregate_root / "cross-platform-dashboard.html"
    root_prefix = "../../"  # from docs/verification/foundation-dll-audit/aggregate/ back to repo root
    dashboard_html = _render_cross_platform_dashboard(matrix, root_prefix=root_prefix)
    dashboard_path.write_text(dashboard_html, encoding="utf-8")
    print(f"Cross-platform dashboard written to {dashboard_path}")

    # Write INDEX.md with platform list
    index_lines = ["# Cross-Platform Verification Index", "", "## Platforms"]
    for plat in matrix.get("platforms", []):
        ps = dict(matrix.get("platformSummaries", {}).get(plat, {}))
        total = ps.get("totalFamilies", 0)
        closed = ps.get("closedFamilies", 0)
        index_lines.append(f"- [{plat}](../{plat}/dashboard.html) — {closed}/{total} families closed")
    index_lines.append("")
    index_lines.append("## Aggregated")
    index_lines.append(f"- [Cross-Platform Dashboard](./cross-platform-dashboard.html)")
    index_lines.append(f"- [Cross-Platform Matrix](./cross-platform-matrix.json)")
    index_path = aggregate_root / "INDEX.md"
    index_path.write_text("\n".join(index_lines).strip() + "\n", encoding="utf-8")
    print(f"Index written to {index_path}")

    return {
        "aggregateRoot": str(aggregate_root),
        "matrixPath": str(matrix_path),
        "dashboardPath": str(dashboard_path),
        "indexPath": str(index_path),
    }


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Generate cross-platform verification dashboard")
    parser.add_argument("--audit-root", type=Path, default=Path.cwd() / "docs" / "verification" / "foundation-dll-audit",
                        help="Root directory of per-platform audit outputs")
    args = parser.parse_args()
    write_cross_platform_outputs(args.audit_root.resolve())


if __name__ == "__main__":
    main()
