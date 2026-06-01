# AutoTestGenerator: System.Convert

- Assembly: `System.Runtime`
- Type: `System.Convert`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 294 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 294 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 552 |

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
