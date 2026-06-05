# AutoTestGenerator: System.Data.DataRowView

- Assembly: `System.Data.Common`
- Type: `System.Data.DataRowView`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 8 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 12 |

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
