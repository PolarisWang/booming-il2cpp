# AutoTestGenerator: System.Security.Cryptography.RSACng

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.RSACng`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 18 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 6 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 12 |
| Total Subjects | 15 |

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

- `TryEncrypt (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryDecrypt (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
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
