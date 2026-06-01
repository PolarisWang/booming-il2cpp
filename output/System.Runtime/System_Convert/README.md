# AutoTestGenerator: System.Convert

- Assembly: `System.Runtime`
- Type: `System.Convert`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 294 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 236 |
| Benchmark-only (pure void / non-deterministic) | 58 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 566 |

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
