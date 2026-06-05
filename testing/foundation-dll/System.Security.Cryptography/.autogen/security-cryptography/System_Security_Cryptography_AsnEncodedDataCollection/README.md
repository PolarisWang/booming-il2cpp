# AutoTestGenerator: System.Security.Cryptography.AsnEncodedDataCollection

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.AsnEncodedDataCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 4 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 3 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 3 |

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

- `CopyTo (ICollection non-generic leak: AsnEncodedData[])` — contains generic parameters, needs manual handling
