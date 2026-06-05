# AutoTestGenerator: System.Security.Cryptography.X509Certificates.X509ChainElementCollection

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.X509Certificates.X509ChainElementCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 2 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 1 |

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

- `CopyTo (ICollection non-generic leak: X509ChainElement[])` — contains generic parameters, needs manual handling
