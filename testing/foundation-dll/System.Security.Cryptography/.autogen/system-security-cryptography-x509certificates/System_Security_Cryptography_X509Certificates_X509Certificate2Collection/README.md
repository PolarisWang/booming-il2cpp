# AutoTestGenerator: System.Security.Cryptography.X509Certificates.X509Certificate2Collection

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.X509Certificate2Collection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 26 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 19 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 7 |
| Total Subjects | 35 |

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

- `Import (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Import (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Import (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Import (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ImportFromPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportPkcs7Pem (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryExportCertificatePems (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
