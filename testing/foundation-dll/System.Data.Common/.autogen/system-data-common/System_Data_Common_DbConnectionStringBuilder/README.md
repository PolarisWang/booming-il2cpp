# AutoTestGenerator: System.Data.Common.DbConnectionStringBuilder

- Assembly: `System.Data.Common`
- Type: `System.Data.Common.DbConnectionStringBuilder`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 9 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 9 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 18 |

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
