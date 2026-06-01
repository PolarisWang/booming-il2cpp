# AutoTestGenerator: System.IO.File

- Assembly: `System.Runtime`
- Type: `System.IO.File`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 105 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 62 |
| Benchmark-only (pure void / non-deterministic) | 43 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 232 |

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
