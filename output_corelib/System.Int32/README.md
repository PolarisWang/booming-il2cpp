# AutoTestGenerator: System.Int32

- Assembly: `System.Runtime`
- Type: `System.Int32`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 46 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 46 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 96 |

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
