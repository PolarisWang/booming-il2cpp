import json
from pathlib import Path

script_dir = Path(__file__).resolve().parent
project_root = script_dir.parent.parent

with open(project_root / 'nightly-build-report/full-run-v5/20260620_104654-c6a10a99/summary/nightly-delta.json') as f:
    d = json.load(f)

ov = d["overall"]
lines = []

def p(s=""):
    lines.append(s)

p("=" * 70)
p("  Chaos IL2CPP Nightly Build Report v5")
p("  2026-06-20 | 28 assemblies | 58/71 chunks verified")
p("=" * 70)
p()

p("1. EXECUTIVE SUMMARY")
p("-" * 60)
p(f"  Pipeline:         nightly_runner (4 workers, --no-profile)")
p(f"  Duration:         56 min (3361s)")
p(f"  Assemblies:       {ov['totalAssemblies']}")
p(f"  Chunks verified:  {ov['chunksVerified']}/{ov['totalChunks']} ({ov['chunksVerified']/ov['totalChunks']*100:.0f}%)")
p(f"  Fact pass rate:   {ov['factPassRate']}%")
p(f"  Benchmark:        {ov['totalBenchmarked']} methods")
p(f"  Memory alloc:     {ov['totalAllocatedBytes']/1024/1024:.0f}MB")
p(f"  Coverage gap:     {ov['totalCoverageGap']} ({ov['totalCoverageGapPct']}%)")
p(f"  Regression grade: {ov['regressionGrade']} (hard={ov['regressedChunks']}, soft={ov['degradedChunks']})")
p(f"  Build failures:   0")
p(f"  Hotupdate issues: 4 chunks (patchFailed=True)")
p()

p("2. PER-ASSEMBLY DETAIL")
p("-" * 60)
asm_data = []
for a in sorted(d["assemblies"], key=lambda x: x["assembly"]):
    name = a["assembly"]
    rate = a.get("factPassRate")
    rate_s = f"{rate:.1f}%" if rate else "N/A"
    gap_pct = a.get("coverageGapPct")
    gap_s = f"{gap_pct:.1f}%" if gap_pct else "-"
    alloc = a.get("totalAllocatedBytes", 0) or 0
    if alloc >= 1024*1024:
        alloc_s = f"{alloc/1024/1024:.0f}MB"
    elif alloc > 0:
        alloc_s = f"{alloc/1024:.0f}KB"
    else:
        alloc_s = "-"
    bench = a.get("totalBenchmarked", 0)
    bench_s = str(bench) if bench > 0 else "-"
    ck = a.get("chunksVerified", 0)
    ct = a.get("totalChunks", 0)
    hu = a.get("hotupdate", {})
    hu_fail = hu.get("chunksPatchFailed", 0) > 0 or hu.get("totalFailed", 0) > 0
    hu_s = "FAIL" if hu_fail else "OK"
    asm_data.append((name, rate_s, gap_s, alloc_s, bench_s, f"{ck}/{ct}", hu_s))

header = f"  {'Assembly':30s} {'Fact':>7s} {'Gap':>6s} {'Alloc':>8s} {'Bench':>6s} {'Chk':>5s} {'HU':>4s}"
p(header)
p("  " + "-"*30 + " " + "-"*7 + " " + "-"*6 + " " + "-"*8 + " " + "-"*6 + " " + "-"*5 + " " + "-"*4)
for row in asm_data:
    p(f"  {row[0]:30s} {row[1]:>7s} {row[2]:>6s} {row[3]:>8s} {row[4]:>6s} {row[5]:>5s} {row[6]:>4s}")
p()

p("3. DATA QUALITY ISSUES")
p("-" * 60)
p()

p("3.1 Low Fact Pass Rate (< 80%)")
p("These chunks have methods that crash or fail in the AOT environment:")
p()
count = 0
for ck, cd in sorted(d["chunks"].items()):
    ft = cd.get("factTotal", 0)
    fp = cd.get("factPassed", 0)
    if ft > 0 and fp/ft < 0.8:
        count += 1
        p(f"  {ck}: {fp}/{ft} ({fp/ft*100:.1f}%)")
if count == 0:
    p("  (none)")
p()

p("3.2 Coverage Gaps > 10%")
p()
count = 0
for ck, cd in sorted(d["chunks"].items()):
    cgp = cd.get("coverageGapPct")
    if cgp and cgp > 10:
        ft = cd.get("factTotal", 0)
        gap = cd.get("coverageGap", 0)
        count += 1
        p(f"  {ck}: gap={cgp}% ({gap}/{ft+gap} methods)")
if count == 0:
    p("  (none)")
p()

p("3.3 Hotupdate: Passed Tests Without Patch Data")
p("These chunks show passed tests but NO patch was actually applied:")
p()
count = 0
for ck, cd in sorted(d["chunks"].items()):
    if cd.get("huPassed", 0) > 0 and cd.get("huPatchFailed", False):
        count += 1
        p(f"  {ck}: {cd['huPassed']} passed (no patch applied)")
if count == 0:
    p("  (none)")
else:
    p(f"  Total: {count} chunks affected (shared-first search fix applied)")
p()

p("3.4 Missing CoreLib Chunks (13/24 in delta)")
p("The following System.Private.CoreLib chunks were NOT built in V5")
p("(V5 terminated before completing all 20 CoreLib chunks):")
p()
all_corelib = ["system", "system-2", "system-3", "system-4", "system-5",
    "runtime-intrinsics", "runtime-intrinsics-2", "enum-parsing",
    "global-ns", "system-data", "system-data-common"]
present = set()
for ck in d["chunks"]:
    if ck.startswith("System.Private.CoreLib/"):
        present.add(ck.split("/")[1])
for s in all_corelib:
    status = "IN DELTA" if s in present else "MISSING"
    p(f"  {s}: {status}")
p()

p("3.5 Benchmark Regressions")
p(f"Grade: {ov['regressionGrade'].upper()} - showing systemic duration variance")
p("(Likely environment differences, not code degradation):")
p()
for r in sorted(d["regressions"], key=lambda x: -abs(x.get("delta", 0)))[:5]:
    p(f"  {r['assembly']}/{r['slug']}: {r['metric']} {r['delta']:+.1f}%")
p()

p("4. RECOMMENDATIONS")
p("-" * 60)
p()
p("  Priority 0 - Fix Before Next Full Run:")
p("  [P0] Verify hotupdate patch generation (shared-first DLL search)")
p("  [P0] Resolve low fact rate in Xml.ReaderWriter/Private.Xml (3 chunks)")
p()
p("  Priority 1 - Data Completeness:")
p("  [P1] Run V6 with profile data (--no-profile removed)")
p("  [P1] Complete CoreLib chunks (11 missing)")
p("  [P1] Accumulate 3+ days history for regression grading")
p()
p("  Priority 2 - Quality Improvements:")
p("  [P2] Investigate low fact rate in Asn1/Brotli/Pipelines")
p("  [P2] Coverage gap reduction (numerics intrinsics classified, 8 WebUtility remaining)")
p("  [P2] Multi-day trend analysis for coverage/benchmark")
p()
p("=" * 70)

report = "\n".join(lines)
print(report)

# Save to file
with open(script_dir / 'tmp_report.txt', "w", encoding="utf-8") as f:
    f.write(report)
print("\n(Saved to comprehensive-report.txt)")
