# AutoTestGenerator: System.Data.Common.DataAdapter

- Assembly: `System.Data.Common`
- Type: `System.Data.Common.DataAdapter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 7 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 7 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 7 |

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
