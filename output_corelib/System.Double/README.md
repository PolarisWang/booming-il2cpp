# AutoTestGenerator: System.Double

- Assembly: `System.Runtime`
- Type: `System.Double`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 115 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 115 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 256 |

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
