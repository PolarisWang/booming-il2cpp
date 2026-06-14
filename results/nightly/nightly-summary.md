## Nightly Build Report — 2026-06-14

### Overall

| Metric | Value |
|--------|-------|
| Assemblies | 22 |
| Chunks | 35 / 41 verified |
| Fact pass rate | 99.9%  |
| Benchmark methods | 5505 |
| Coverage gap | 4058 methods unverified |
| Assemblies with history | 22 |

### Regressions ⚠️ (0)

_No regressions detected._

### Improvements ✅ (0)

_No improvements detected._

### Stage Detail

#### Build Failures ❌ (41)

- System.Collections.Immutable/global-ns (not_run)
- System.Collections/global-ns (not_run)
- System.ComponentModel.TypeConverter/component-model (not_run)
- System.ComponentModel.TypeConverter/global-ns (not_run)
- System.ComponentModel.TypeConverter/system-componentmodel-design (not_run)
- System.Data.Common/global-ns (not_run)
- System.Data.Common/system-data (not_run)
- System.Data.Common/system-data-common (not_run)
- System.Data.Common/system-data-sqltypes (not_run)
- System.Diagnostics.DiagnosticSource/diagnostics (not_run)
- System.Diagnostics.DiagnosticSource/global-ns (not_run)
- System.Formats.Asn1/global-ns (not_run)
- System.Formats.Asn1/system-formats-asn1 (not_run)
- System.IO.Compression.Brotli/global-ns (not_run)
- System.IO.Pipelines/global-ns (not_run)
- System.IO.Pipelines/system-io-pipelines (not_run)
- System.Linq.Expressions/global-ns (not_run)
- System.Linq/global-ns (not_run)
- System.Net.Http/global-ns (not_run)
- System.Net.Http/system-net-http (not_run)

#### No Fact Data ⚠️ (3)

- System.Security.Cryptography/security-cryptography (metaTotal=1307)
- System.Security.Cryptography/security-cryptography-2 (metaTotal=1307)
- System.Security.Cryptography/system-security-cryptography-x509certificates (metaTotal=276)

#### Large Coverage Gaps ⚠️ (21)

- System.ComponentModel.TypeConverter/component-model gap=29 (5%)
- System.ComponentModel.TypeConverter/system-componentmodel-design gap=14 (37%)
- System.Data.Common/system-data gap=56 (9%)
- System.Data.Common/system-data-common gap=105 (24%)
- System.Data.Common/system-data-sqltypes gap=105 (30%)
- System.Diagnostics.DiagnosticSource/diagnostics gap=13 (9%)
- System.Formats.Asn1/system-formats-asn1 gap=40 (52%)
- System.IO.Compression.Brotli/global-ns gap=16 (33%)
- System.IO.Pipelines/system-io-pipelines gap=15 (45%)
- System.Linq.Expressions/global-ns gap=15 (2%)
- System.Net.Sockets/system-net-sockets gap=22 (4%)
- System.Private.Xml/system-xml-serialization gap=18 (9%)
- System.Private.Xml/system-xml-xsl gap=22 (11%)
- System.Private.Xml/xml gap=109 (11%)
- System.Reflection.Metadata/system-reflection-metadata gap=162 (40%)
- System.Reflection.Metadata/system-reflection-metadata-ecma335 gap=79 (18%)
- System.Runtime.Serialization.Formatters/global-ns gap=44 (52%)
- System.Security.Claims/global-ns gap=12 (26%)
- System.Security.Principal.Windows/global-ns gap=13 (31%)
- System.Text.Json/system-text-json-serialization-converters gap=44 (52%)

### Per-Assembly Detail

| Assembly | Build | Fact | ΔFact | Bench(ms) | ΔBench | Coverage | Profile | MBench | HU |
|----------|-------|------|-------|-----------|--------|----------|---------|--------|-----|
| System.Collections | 0/1 | 100.0% | = | — | = | 6 | — | — | ✅ |
| System.Collections.Immutable | 0/1 | 100.0% | = | — | — | 5 | — | — | ✅ |
| System.ComponentModel.TypeConverter | 0/3 | 100.0% | = | — | — | 49 | — | — | ✅ |
| System.Data.Common | 0/4 | 99.8% | = | — | = | 266 | — | — | ✅ |
| System.Diagnostics.DiagnosticSource | 0/2 | 100.0% | = | — | = | 16 | — | — | ❌ |
| System.Formats.Asn1 | 0/2 | 100.0% | = | — | — | 40 | — | — | ❌ |
| System.IO.Compression.Brotli | 0/1 | 100.0% | = | — | — | 16 | — | — | ❌ |
| System.IO.Pipelines | 0/2 | 100.0% | = | — | — | 15 | — | — | ❌ |
| System.Linq | 0/1 | 100.0% | = | — | = | 2 | — | — | ✅ |
| System.Linq.Expressions | 0/1 | 100.0% | = | — | — | 15 | — | — | ❌ |
| System.Net.Http | 0/2 | 100.0% | = | — | = | 17 | — | — | ✅ |
| System.Net.ServerSentEvents | 0/1 | 100.0% | = | — | — | 1 | — | — | ❌ |
| System.Net.Sockets | 0/1 | 100.0% | = | — | = | 22 | — | — | ✅ |
| System.Private.Xml | 0/5 | 100.0% | = | — | — | 157 | — | — | ✅ |
| System.Reflection.Metadata | 0/3 | 99.2% | = | — | = | 247 | — | — | ✅ |
| System.Runtime.InteropServices | 0/1 | 100.0% | = | — | — | 7 | — | — | ❌ |
| System.Runtime.Serialization.Formatters | 0/1 | 100.0% | = | — | — | 44 | — | — | ❌ |
| System.Security.Claims | 0/1 | 100.0% | = | — | — | 12 | — | — | ❌ |
| System.Security.Cryptography | 0/3 | — | — | — | = | 2890 | — | — | ✅ |
| System.Security.Principal.Windows | 0/1 | 100.0% | = | — | — | 13 | — | — | ✅ |
| System.Text.Json | 0/3 | 100.0% | = | — | = | 216 | — | — | ✅ |
| System.Threading.Tasks.Parallel | 0/1 | 100.0% | = | — | — | 2 | — | — | ✅ |

---
_Generated at 2026-06-14 11:53:11 UTC_
