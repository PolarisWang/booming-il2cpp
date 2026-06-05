# AutoTestGenerator: System.Security.Cryptography.X509Certificates.PublicKey

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.PublicKey`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 6 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 4 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 4 |

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

- `TryExportSubjectPublicKeyInfo (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `CreateFromSubjectPublicKeyInfo (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
