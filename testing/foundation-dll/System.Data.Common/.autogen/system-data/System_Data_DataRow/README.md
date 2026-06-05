# AutoTestGenerator: System.Data.DataRow

- Assembly: `System.Data.Common`
- Type: `System.Data.DataRow`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 35 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 35 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 50 |

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
