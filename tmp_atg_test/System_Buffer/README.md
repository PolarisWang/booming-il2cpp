# AutoTestGenerator: System.Buffer

- Assembly: `System.Private.CoreLib`
- Type: `System.Buffer`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 6 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 4 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 16 |

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
