# CI 全量回归验证命令

## 1. 完整构建工具链

```bash
# 确保没有残留进程锁
taskkill /F /IM dotnet.exe 2>/dev/null || true
taskkill /F /IM VBCSCompiler.exe 2>/dev/null || true

# 全量构建
cd <repo-root>
rm -rf src/managed/Chaos.IL2CPP.Generator/obj/
dotnet build src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/ -c Debug --no-incremental
```

## 2. 全量 chunks 回归

```bash
# 方式一: nightly build (全量, 包含 benchmark)
python testing/foundation-dll/verification/nightly_build.py --max-workers 4 --bench-workers 2

# 方式二: 逐 assembly 验证 (适合 CI 并行)
for asm in \
  System.ObjectModel System.Linq System.Collections \
  System.Data.Common System.Net.Http System.Threading.Tasks.Parallel \
  System.Linq.Expressions System.Text.Json System.Security.Claims \
  System.Collections.Immutable System.Collections.NonGeneric \
  System.Diagnostics.DiagnosticSource System.Formats.Asn1 \
  System.IO.Compression.Brotli System.IO.Compression.ZipFile \
  System.IO.Pipelines System.Net.ServerSentEvents System.Net.Sockets \
  System.Reflection.Metadata System.Runtime System.Runtime.InteropServices \
  System.Runtime.Serialization.Formatters System.Security.Cryptography \
  System.Xml.ReaderWriter; do
  rm -rf testing/foundation-dll/$asm/.hephaestus-cache/
  python testing/foundation-dll/verification/__main__.py \
    --assembly "$asm" --chunk global-ns --stages build,fact,benchmark
done
```

## 3. System.Private.CoreLib 42 chunks

```bash
# CoreLib 需要逐个 chunk 运行 (不能在 --all-chunks 下并行)
python testing/foundation-dll/verification/__main__.py \
  --assembly System.Private.CoreLib --chunk system --stages build,fact
python testing/foundation-dll/verification/__main__.py \
  --assembly System.Private.CoreLib --chunk system-2 --stages build,fact
# ... 42 chunks total
```

## 4. 预期结果

| Family | 预期状态 | 备注 |
|--------|---------|------|
| System.ObjectModel | ✅ | 已验证 |
| System.Linq | ✅ AOT 157/157, JIT 157/157 | 已验证 |
| System.Data.Common | ✅ | 已验证 |
| System.Net.Http | ✅ | 已验证 |
| System.Threading.Tasks.Parallel | ✅ | 已验证 |
| System.Collections | ✅ | 已验证 |
| System.Linq.Expressions | ✅ | 已验证 |
| System.Text.Json | ✅ | 已验证 |
| System.Security.Claims | ✅ | 已验证 |
| System.Private.CoreLib 42 chunks | ⏳ 待验证 | 之前被 array_stubs.h 阻塞 |
| 其余 ~30 family | ⏳ 待验证 | 大部分应通过 |
