# AutoTestGenerator: System.Decimal

- Assembly: `System.Runtime`
- Type: `System.Decimal`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 70 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 70 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 150 |

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
