# AutoTestGenerator: System.Array

- Assembly: `System.Runtime`
- Type: `System.Array`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 100 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 57 |
| Benchmark-only (pure void / non-deterministic) | 43 |
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
