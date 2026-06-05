# AutoTestGenerator: System.Data.Common.DbConnection

- Assembly: `System.Data.Common`
- Type: `System.Data.Common.DbConnection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 21 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 21 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 31 |

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
