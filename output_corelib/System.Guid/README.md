# AutoTestGenerator: System.Guid

- Assembly: `System.Runtime`
- Type: `System.Guid`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 27 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 25 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 37 |

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
