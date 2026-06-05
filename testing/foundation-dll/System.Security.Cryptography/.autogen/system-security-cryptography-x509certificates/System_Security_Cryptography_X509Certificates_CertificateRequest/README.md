# AutoTestGenerator: System.Security.Cryptography.X509Certificates.CertificateRequest

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.CertificateRequest`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 13 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 7 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 6 |
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
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `LoadSigningRequestPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `LoadSigningRequest (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
