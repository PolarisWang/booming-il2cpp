# AutoTestGenerator: System.Security.Cryptography.ECDsaCng

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.ECDsaCng`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 24 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 14 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 10 |
| Total Subjects | 36 |

## Build

```bash
# Build with verification (Assert enabled)
dotnet build -p:DefineConstants=VERIFY

# Build for benchmark (Assert eliminated)
dotnet build
```

## Run

```bash
# Run with verification
dotnet run --project .
```

## Skipped Methods

- `ImportPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TrySignHash (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `VerifyHash (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportPkcs8PrivateKey (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
