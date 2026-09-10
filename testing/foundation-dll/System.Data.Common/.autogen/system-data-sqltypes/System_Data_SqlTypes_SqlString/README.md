# AutoTestGenerator: System.Data.SqlTypes.SqlString

- Assembly: `System.Data.Common`
- Type: `System.Data.SqlTypes.SqlString`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 25 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 25 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 25 |

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
