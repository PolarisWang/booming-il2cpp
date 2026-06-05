# AutoTestGenerator: System.Data.SqlTypes.SqlByte

- Assembly: `System.Data.Common`
- Type: `System.Data.SqlTypes.SqlByte`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 28 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 28 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 29 |

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
