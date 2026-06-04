# AutoTestGenerator: System.Linq.Lookup<System.Int32, System.Int32>

- Assembly: `System.Linq`
- Type: `System.Linq.Lookup<System.Int32, System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 3 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 3 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 5 |

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
