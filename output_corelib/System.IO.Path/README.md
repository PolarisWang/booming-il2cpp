# AutoTestGenerator: System.IO.Path

- Assembly: `System.Runtime`
- Type: `System.IO.Path`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 45 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 43 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 79 |

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
