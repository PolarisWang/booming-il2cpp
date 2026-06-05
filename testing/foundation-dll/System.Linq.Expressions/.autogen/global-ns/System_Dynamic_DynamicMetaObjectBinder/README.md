# AutoTestGenerator: System.Dynamic.DynamicMetaObjectBinder

- Assembly: `System.Linq.Expressions`
- Type: `System.Dynamic.DynamicMetaObjectBinder`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 5 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 5 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 13 |

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
