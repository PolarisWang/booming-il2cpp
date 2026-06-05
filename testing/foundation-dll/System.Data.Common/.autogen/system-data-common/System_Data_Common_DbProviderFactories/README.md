# AutoTestGenerator: System.Data.Common.DbProviderFactories

- Assembly: `System.Data.Common`
- Type: `System.Data.Common.DbProviderFactories`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 10 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 10 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 17 |

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
