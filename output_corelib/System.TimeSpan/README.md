# AutoTestGenerator: System.TimeSpan

- Assembly: `System.Runtime`
- Type: `System.TimeSpan`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 51 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 51 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 102 |

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
