# AutoTestGenerator: System.TupleExtensions

- Assembly: `System.Private.CoreLib`
- Type: `System.TupleExtensions`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 63 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 21 |
| Benchmark-only (pure void / non-deterministic) | 42 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 104 |

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
