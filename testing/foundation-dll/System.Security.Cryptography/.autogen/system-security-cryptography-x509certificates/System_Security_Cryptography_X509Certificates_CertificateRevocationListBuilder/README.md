# AutoTestGenerator: System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.CertificateRevocationListBuilder`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 12 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 4 |
| Total Subjects | 21 |

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

- `AddEntry (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `RemoveEntry (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Load (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `LoadPem (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
