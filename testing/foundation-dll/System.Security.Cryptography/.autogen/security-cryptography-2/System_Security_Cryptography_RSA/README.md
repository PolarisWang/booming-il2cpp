# AutoTestGenerator: System.Security.Cryptography.RSA

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.RSA`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 56 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 21 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 35 |
| Total Subjects | 51 |

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
- `TryDecrypt (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryEncrypt (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Encrypt (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Encrypt (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Decrypt (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Decrypt (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TrySignHash (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `VerifyHash (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TrySignData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `SignData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `SignData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `SignHash (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `SignHash (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `VerifyData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportRSAPrivateKey (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportRSAPublicKey (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportSubjectPublicKeyInfo (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportPkcs8PrivateKey (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportSubjectPublicKeyInfo (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportRSAPublicKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportRSAPrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportFromPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportFromEncryptedPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportFromEncryptedPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportRSAPrivateKeyPem (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportRSAPublicKeyPem (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
