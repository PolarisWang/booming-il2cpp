# AutoTestGenerator: System.Security.Cryptography.X509Certificates.X509Certificate2

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.X509Certificate2`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 23 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 16 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 7 |
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

- `Reset (enumerator method projected by MLC)` — contains generic parameters, needs manual handling
- `GetCertContentType (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `CreateFromEncryptedPemFile (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `CreateFromPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `CreateFromEncryptedPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `CreateFromPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportCertificatePem (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
