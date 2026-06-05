# AutoTestGenerator: System.Security.Cryptography.X509Certificates.X509CertificateLoader

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.X509CertificateLoader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 11 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 6 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 5 |
| Total Subjects | 19 |

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

- `LoadCertificate (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `LoadPkcs12 (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `LoadPkcs12FromFile (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `LoadPkcs12Collection (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `LoadPkcs12CollectionFromFile (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
