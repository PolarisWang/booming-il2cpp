# AutoTestGenerator: System.Data.Common.DbDataRecord

- Assembly: `System.Data.Common`
- Type: `System.Data.Common.DbDataRecord`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 22 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 22 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 51 |

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
