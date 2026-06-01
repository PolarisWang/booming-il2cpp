# AutoTestGenerator: System.Convert

- Assembly: `System.Runtime`
- Type: `System.Convert`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 283 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 227 |
| Benchmark-only (pure void / non-deterministic) | 56 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 550 |

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
