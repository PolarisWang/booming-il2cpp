# AutoTestGenerator: System.Array

- Assembly: `System.Private.CoreLib`
- Type: `System.Array`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 100 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 99 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 191 |

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
