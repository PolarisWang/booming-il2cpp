# AutoTestGenerator: System.Data.Common.DbDataAdapter

- Assembly: `System.Data.Common`
- Type: `System.Data.Common.DbDataAdapter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 13 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 13 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 24 |

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
