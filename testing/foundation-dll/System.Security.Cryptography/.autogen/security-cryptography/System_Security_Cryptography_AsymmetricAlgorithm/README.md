# AutoTestGenerator: System.Security.Cryptography.AsymmetricAlgorithm

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.AsymmetricAlgorithm`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 29 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 21 |
| Total Subjects | 10 |

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
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportSubjectPublicKeyInfo (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportPkcs8PrivateKey (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportSubjectPublicKeyInfo (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportFromEncryptedPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportFromEncryptedPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportFromPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ExportEncryptedPkcs8PrivateKeyPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ExportEncryptedPkcs8PrivateKeyPem (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportSubjectPublicKeyInfoPem (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportPkcs8PrivateKeyPem (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKeyPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKeyPem (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
