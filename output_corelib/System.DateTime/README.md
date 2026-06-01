# AutoTestGenerator: System.DateTime

- Assembly: `System.Runtime`
- Type: `System.DateTime`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 62 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 60 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 101 |

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
