# AutoTestGenerator: System.Data.DataView

- Assembly: `System.Data.Common`
- Type: `System.Data.DataView`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 14 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 14 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 28 |

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
