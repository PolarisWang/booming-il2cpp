## Nightly Build Report — 2026-06-20

### Overall

| Metric | Value |
|--------|-------|
| Assemblies | 28 |
| Chunks | 55 / 71 verified |
| Fact pass rate | 95.4%  |
| Benchmark methods | 18239 |
| Coverage gap | 497 methods (1.1%) |
| Total allocated | 510.0MB |
| Benchmark regression | **NONE** ✅ (hard=0, soft=0) |
| Assemblies with history | 28 |

### Regressions ⚠️ (3)

| Assembly | Chunk | Metric | Before | After | Δ |
|----------|-------|--------|--------|-------|---|
| System.IO.Pipelines | system-io-pipelines | bench_duration | 2.2518ms | 2.4927ms | **+10.7** |
| System.Runtime.InteropServices | global-ns | coverage_gap | 0 | 9 | **+9.0** |
| System.Runtime.InteropServices | global-ns | coverage_gap_pct | 0.0% | 23.7% | **+23.7** |

### Improvements ✅ (2)

| Assembly | Chunk | Metric | Before | After | Δ |
|----------|-------|--------|--------|-------|---|
| System.IO.Compression.ZipFile | global-ns | bench_duration | 0.0395ms | 0.0298ms | **-24.5** |
| System.Private.CoreLib | threading-tasks | bench_duration | 0.1934ms | 0.1688ms | **-12.7** |

### Stage Detail

#### Build Failures ❌ (10)

- System.Net.Sockets/global-ns (not_run)
- System.Private.CoreLib/enum-parsing (not_run)
- System.Private.CoreLib/global-ns (not_run)
- System.Private.CoreLib/system-data (not_run)
- System.Private.CoreLib/system-data-common (not_run)
- System.Private.Xml/system-xml-xsl-runtime (not_run)
- System.Runtime.Intrinsics/runtime-intrinsics (not_run)
- System.Xml.ReaderWriter/system-xml-schema (not_run)
- System.Xml.ReaderWriter/system-xml-xsl (not_run)
- System.Xml.ReaderWriter/xml (not_run)

#### No Fact Data ⚠️ (16)

- System.Net.Sockets/global-ns (no fact data)
- System.Private.CoreLib/enum-parsing (no fact data)
- System.Private.CoreLib/global-ns (no fact data)
- System.Private.CoreLib/runtime-intrinsics (no fact data)
- System.Private.CoreLib/system (no fact data)
- System.Private.CoreLib/system-2 (no fact data)
- System.Private.CoreLib/system-3 (no fact data)
- System.Private.CoreLib/system-4 (no fact data)
- System.Private.CoreLib/system-5 (no fact data)
- System.Private.CoreLib/system-data (no fact data)
- System.Private.CoreLib/system-data-common (no fact data)
- System.Private.Xml/system-xml-xsl-runtime (no fact data)
- System.Runtime.Intrinsics/runtime-intrinsics (no fact data)
- System.Xml.ReaderWriter/system-xml-schema (no fact data)
- System.Xml.ReaderWriter/system-xml-xsl (no fact data)
- System.Xml.ReaderWriter/xml (no fact data)

#### Large Coverage Gaps ⚠️ (6)

- System.Collections.NonGeneric/global-ns gap=15 (22%)
- System.Diagnostics.DiagnosticSource/global-ns gap=57 (33%)
- System.Private.CoreLib/diagnostics gap=165 (34%)
- System.Private.CoreLib/numerics gap=95 (73%)
- System.Private.CoreLib/runtime-compiler gap=78 (10%)
- System.Reflection.Metadata/system-reflection-metadata-ecma335 gap=78 (18%)

### Per-Assembly Detail

| Assembly | Build | Fact | ΔFact | Bench(ms) | ΔBench | Coverage | Alloc | Profile | MBench | HU |
|----------|-------|------|-------|-----------|--------|----------|-------|---------|--------|-----|
| System.Collections | 1/1 | 100.0% | = | — | +4.4% | 0 (0.0%) | — | — | — | ✅ |
| System.Collections.Immutable | 1/1 | 100.0% | = | — | +7.0% | 0 (0.0%) | — | — | — | ✅ |
| System.Collections.NonGeneric | 1/1 | 100.0% | = | — | = | 15 (22.4%) | — | — | — | ❌ |
| System.ComponentModel.TypeConverter | 3/3 | 100.0% | = | — | = | 0 (0.0%) | 937.5KB | — | — | ✅ |
| System.Data.Common | 4/4 | 98.0% | = | — | = | 0 (0.0%) | 5.3MB | — | — | ✅ |
| System.Diagnostics.DiagnosticSource | 2/2 | 100.0% | = | — | -1.0% | 57 (9.7%) | 40.7MB | — | — | ✅ |
| System.Formats.Asn1 | 1/1 | 30.0% | = | — | +4.2% | 0 (0.0%) | 2.1MB | — | — | ✅ |
| System.IO.Compression.Brotli | 1/1 | 24.7% | = | — | -4.0% | 0 (0.0%) | 1.9MB | — | — | ✅ |
| System.IO.Compression.ZipFile | 1/1 | 100.0% | = | — | -24.5% | 0 (0.0%) | — | — | — | ✅ |
| System.IO.Pipelines | 1/1 | 80.0% | = | — | +10.7% | 0 (0.0%) | 625.0KB | — | — | ✅ |
| System.Linq | 1/1 | 100.0% | = | — | -1.9% | 0 (0.0%) | — | — | — | ✅ |
| System.Linq.Expressions | 1/1 | 88.0% | = | — | +2.3% | 0 (0.0%) | — | — | — | ✅ |
| System.Net.Http | 2/2 | 100.0% | = | — | +2.5% | 0 (0.0%) | — | — | — | ✅ |
| System.Net.ServerSentEvents | 1/1 | 60.0% | = | — | -3.6% | 0 (0.0%) | — | — | — | ✅ |
| System.Net.Sockets | 1/2 | 100.0% | = | — | = | 0 (0.0%) | — | — | — | ✅ |
| System.ObjectModel | 1/1 | 100.0% | = | — | +0.6% | 0 (0.0%) | — | — | — | ✅ |
| System.Private.CoreLib | 20/24 | 92.8% | -5.9% | — | -0.6% | 338 (1.9%) | 431.2MB | — | — | ✅ |
| System.Private.Xml | 4/5 | 96.0% | = | — | = | 0 (0.0%) | 4.6MB | — | — | ✅ |
| System.Reflection.Metadata | 3/3 | 99.0% | = | — | = | 78 (4.6%) | 14.2MB | — | — | ✅ |
| System.Runtime.InteropServices | 1/1 | 100.0% | = | — | +0.9% | 9 (23.7%) | — | — | — | ✅ |
| System.Runtime.Intrinsics | 0/1 | — | — | — | — | 0 | — | — | — | ✅ |
| System.Runtime.Serialization.Formatters | 1/1 | 96.6% | = | — | -1.0% | 0 (0.0%) | — | — | — | ✅ |
| System.Security.Claims | 1/1 | 100.0% | = | — | +2.2% | 0 (0.0%) | — | — | — | ✅ |
| System.Security.Cryptography | 3/3 | 99.8% | = | — | = | 0 (0.0%) | 5.8MB | — | — | ❌ |
| System.Security.Principal.Windows | 1/1 | 100.0% | = | — | -2.3% | 0 (0.0%) | — | — | — | ✅ |
| System.Text.Json | 3/3 | 99.9% | = | — | = | 0 (0.0%) | 703.1KB | — | — | ✅ |
| System.Threading.Tasks.Parallel | 1/1 | 100.0% | = | — | -0.1% | 0 (0.0%) | 1.8MB | — | — | ✅ |
| System.Xml.ReaderWriter | 0/3 | — | — | — | — | 0 | — | — | — | ✅ |

---
_Generated at 2026-06-20 05:13:43 UTC_
