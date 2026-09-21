# AutoTestGenerator: System.GC

- Assembly: `System.Private.CoreLib`
- Type: `System.GC`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 38 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 16 |
| Benchmark-only (pure void / non-deterministic) | 22 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 74 |

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
