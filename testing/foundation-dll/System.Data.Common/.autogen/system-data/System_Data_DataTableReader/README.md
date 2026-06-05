# AutoTestGenerator: System.Data.DataTableReader

- Assembly: `System.Data.Common`
- Type: `System.Data.DataTableReader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 29 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 29 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 61 |

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
