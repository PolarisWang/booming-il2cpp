# AutoTestGenerator: System.Security.Cryptography.DSA

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.DSA`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 44 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 21 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 23 |
| Total Subjects | 62 |

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

- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `TryCreateSignature (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryCreateSignature (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TrySignData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TrySignData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `VerifyData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `VerifyData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `VerifySignature (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `VerifySignature (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportPkcs8PrivateKey (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportSubjectPublicKeyInfo (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportSubjectPublicKeyInfo (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportFromPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportFromEncryptedPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportFromEncryptedPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
