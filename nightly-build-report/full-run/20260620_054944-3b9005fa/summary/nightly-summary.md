## Nightly Build Report — 2026-06-20

### Overall

| Metric | Value |
|--------|-------|
| Assemblies | 28 |
| Chunks | 54 / 71 verified |
| Fact pass rate | 97.8%  |
| Benchmark methods | 18291 |
| Coverage gap | 473 methods (1.1%) |
| Total allocated | 603.8MB |
| Benchmark regression | **HARD** ❌ (hard=9, soft=7) |
| Assemblies with history | 28 |

### Regressions ⚠️ (22)

| Assembly | Chunk | Metric | Before | After | Δ |
|----------|-------|--------|--------|-------|---|
| System.Collections | global-ns | bench_duration | 4.2516ms | 6.4497ms | **+51.7** |
| System.Collections.Immutable | global-ns | bench_duration | 0.0294ms | 0.0399ms | **+35.5** |
| System.Collections.NonGeneric | global-ns | bench_duration | 4.2862ms | 7.1322ms | **+66.4** |
| System.ComponentModel.TypeConverter | component-model | bench_duration | 0.6103ms | 0.9478ms | **+55.3** |
| System.ComponentModel.TypeConverter | component-model | memory_alloc | 937.5KB | 1.8MB | **+100.0** |
| System.ComponentModel.TypeConverter | global-ns | bench_duration | 0.0302ms | 0.0445ms | **+47.2** |
| System.ComponentModel.TypeConverter | system-componentmodel-design | bench_duration | 0.0296ms | 0.0350ms | **+18.3** |
| System.IO.Pipelines | system-io-pipelines | bench_duration | 2.2524ms | 2.6803ms | **+19.0** |
| System.Linq.Expressions | global-ns | bench_duration | 0.4213ms | 0.8165ms | **+93.8** |
| System.Net.ServerSentEvents | global-ns | bench_duration | 5.0134ms | 5.9108ms | **+17.9** |
| System.Private.CoreLib | buffers | bench_duration | 0.4435ms | 0.5273ms | **+18.9** |
| System.Private.CoreLib | io | bench_duration | 0.1400ms | 0.1758ms | **+25.6** |
| System.Private.CoreLib | io-2 | bench_duration | 1.2607ms | 1.7045ms | **+35.2** |
| System.Private.CoreLib | numerics | bench_duration | 0.0272ms | 0.0407ms | **+49.6** |
| System.Private.CoreLib | runtime-intrinsics | memory_alloc | 49.9MB | 93.7MB | **+87.8** |
| System.Private.CoreLib | runtime-intrinsics-2 | memory_alloc | 49.9MB | 93.7MB | **+87.8** |
| System.Reflection.Metadata | system-reflection-metadata | bench_duration | 0.6855ms | 1.0673ms | **+55.7** |
| System.Reflection.Metadata | system-reflection-metadata | memory_alloc | 2.0MB | 2.4MB | **+23.1** |
| System.Security.Claims | global-ns | bench_duration | 0.0302ms | 0.0420ms | **+39.2** |
| System.Security.Cryptography | security-cryptography | memory_alloc | 2.9MB | 5.4MB | **+83.7** |
| System.Security.Cryptography | security-cryptography-2 | memory_alloc | 2.9MB | 5.6MB | **+91.3** |
| System.Security.Principal.Windows | global-ns | bench_duration | 0.0298ms | 0.0415ms | **+39.2** |

### Improvements ✅ (9)

| Assembly | Chunk | Metric | Before | After | Δ |
|----------|-------|--------|--------|-------|---|
| System.Collections.NonGeneric | global-ns | coverage_gap | 15 | 0 | **-15.0** |
| System.Collections.NonGeneric | global-ns | coverage_gap_pct | 22.4% | 0.0% | **-22.4** |
| System.IO.Pipelines | system-io-pipelines | memory_alloc | 568.2KB | 312.5KB | **-45.0** |
| System.Linq | global-ns | bench_duration | 0.2244ms | 0.1600ms | **-28.7** |
| System.Private.CoreLib | runtime-intrinsics | bench_duration | 3.5307ms | 2.9693ms | **-15.9** |
| System.Private.CoreLib | runtime-intrinsics-2 | bench_duration | 3.2611ms | 2.0969ms | **-35.7** |
| System.Private.CoreLib | threading-tasks | bench_duration | 0.1934ms | 0.1530ms | **-20.9** |
| System.Private.CoreLib | threading-tasks | memory_alloc | 1.2MB | 822.3KB | **-31.6** |
| System.Runtime.InteropServices | global-ns | bench_duration | 0.9979ms | 0.2185ms | **-78.1** |

### Stage Detail

#### Build Failures ❌ (10)

- System.Net.Sockets/global-ns (not_run)
- System.Private.CoreLib/enum-parsing (not_run)
- System.Private.CoreLib/global-ns (not_run)
- System.Private.CoreLib/system-data (not_run)
- System.Private.CoreLib/system-data-common (not_run)
- System.Private.Xml/system-xml-xsl-runtime (failed)
- System.Runtime.Intrinsics/runtime-intrinsics (failed)
- System.Xml.ReaderWriter/system-xml-schema (failed)
- System.Xml.ReaderWriter/system-xml-xsl (failed)
- System.Xml.ReaderWriter/xml (failed)

#### No Fact Data ⚠️ (17)

- System.Net.Sockets/global-ns (no fact data)
- System.Private.CoreLib/enum-parsing (no fact data)
- System.Private.CoreLib/global-ns (no fact data)
- System.Private.CoreLib/runtime-intrinsics (no fact data)
- System.Private.CoreLib/runtime-intrinsics-2 (no fact data)
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

#### Large Coverage Gaps ⚠️ (5)

- System.Diagnostics.DiagnosticSource/global-ns gap=57 (33%)
- System.Private.CoreLib/diagnostics gap=165 (34%)
- System.Private.CoreLib/numerics gap=95 (73%)
- System.Private.CoreLib/runtime-compiler gap=78 (10%)
- System.Reflection.Metadata/system-reflection-metadata-ecma335 gap=78 (18%)

### Per-Assembly Detail

| Assembly | Build | Fact | ΔFact | Bench(ms) | ΔBench | Coverage | Alloc | Profile | MBench | HU |
|----------|-------|------|-------|-----------|--------|----------|-------|---------|--------|-----|
| System.Collections | 1/1 | 100.0% | = | — | +51.7% | 0 (0.0%) | — | — | — | ✅ |
| System.Collections.Immutable | 1/1 | 100.0% | = | — | +35.5% | 0 (0.0%) | — | — | — | ✅ |
| System.Collections.NonGeneric | 1/1 | 100.0% | = | — | +66.4% | 0 (0.0%) | — | — | — | ❌ |
| System.ComponentModel.TypeConverter | 3/3 | 100.0% | = | — | +40.3% | 0 (0.0%) | 1.8MB | — | — | ✅ |
| System.Data.Common | 4/4 | 98.0% | = | — | = | 0 (0.0%) | 5.3MB | — | — | ✅ |
| System.Diagnostics.DiagnosticSource | 2/2 | 100.0% | = | — | = | 57 (9.7%) | 40.7MB | — | — | ✅ |
| System.Formats.Asn1 | 1/1 | 30.0% | = | — | = | 0 (0.0%) | 2.1MB | — | — | ✅ |
| System.IO.Compression.Brotli | 1/1 | 24.7% | = | — | -4.0% | 0 (0.0%) | 1.9MB | — | — | ✅ |
| System.IO.Compression.ZipFile | 1/1 | 100.0% | = | — | = | 0 (0.0%) | — | — | — | ✅ |
| System.IO.Pipelines | 1/1 | 80.0% | = | — | +19.0% | 0 (0.0%) | 312.5KB | — | — | ✅ |
| System.Linq | 1/1 | 100.0% | = | — | -28.7% | 0 (0.0%) | — | — | — | ✅ |
| System.Linq.Expressions | 1/1 | 88.0% | = | — | +93.8% | 0 (0.0%) | — | — | — | ✅ |
| System.Net.Http | 2/2 | 100.0% | = | — | +4.2% | 0 (0.0%) | — | — | — | ✅ |
| System.Net.ServerSentEvents | 1/1 | 60.0% | = | — | +17.9% | 0 (0.0%) | — | — | — | ✅ |
| System.Net.Sockets | 1/2 | 100.0% | = | — | +1.4% | 0 (0.0%) | — | — | — | ✅ |
| System.ObjectModel | 1/1 | 100.0% | = | — | +6.4% | 0 (0.0%) | — | — | — | ✅ |
| System.Private.CoreLib | 20/24 | 98.7% | = | — | +2.6% | 338 (2.0%) | 518.8MB | — | — | ✅ |
| System.Private.Xml | 4/5 | 96.0% | = | — | +1.6% | 0 (0.0%) | 4.6MB | — | — | ✅ |
| System.Reflection.Metadata | 3/3 | 99.0% | = | — | +18.9% | 78 (4.6%) | 14.6MB | — | — | ✅ |
| System.Runtime.InteropServices | 1/1 | 100.0% | = | — | -78.1% | 0 (0.0%) | — | — | — | ❌ |
| System.Runtime.Intrinsics | 0/1 | — | — | — | — | 0 | — | — | — | ✅ |
| System.Runtime.Serialization.Formatters | 1/1 | 96.6% | = | — | -1.1% | 0 (0.0%) | — | — | — | ✅ |
| System.Security.Claims | 1/1 | 100.0% | = | — | +39.2% | 0 (0.0%) | — | — | — | ✅ |
| System.Security.Cryptography | 3/3 | 99.8% | = | — | +2.8% | 0 (0.0%) | 11.0MB | — | — | ❌ |
| System.Security.Principal.Windows | 1/1 | 100.0% | = | — | +39.2% | 0 (0.0%) | — | — | — | ✅ |
| System.Text.Json | 3/3 | 99.9% | = | — | +0.2% | 0 (0.0%) | 703.1KB | — | — | ✅ |
| System.Threading.Tasks.Parallel | 1/1 | 100.0% | = | — | -0.1% | 0 (0.0%) | 1.8MB | — | — | ✅ |
| System.Xml.ReaderWriter | 0/3 | — | — | — | — | 0 | — | — | — | ✅ |

---
_Generated at 2026-06-20 05:49:44 UTC_
