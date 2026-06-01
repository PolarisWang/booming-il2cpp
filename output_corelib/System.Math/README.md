# AutoTestGenerator: System.Math

- Assembly: `System.Private.CoreLib`
- Type: `System.Math`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 120 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 93 |
| Benchmark-only (pure void / non-deterministic) | 27 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 328 |

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
