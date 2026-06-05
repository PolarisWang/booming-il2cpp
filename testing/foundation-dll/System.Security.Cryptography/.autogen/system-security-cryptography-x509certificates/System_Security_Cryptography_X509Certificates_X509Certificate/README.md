# AutoTestGenerator: System.Security.Cryptography.X509Certificates.X509Certificate

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.X509Certificate`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 32 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 30 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 44 |

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
- `TryGetCertHash (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
