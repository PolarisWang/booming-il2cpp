# AutoTestGenerator: System.ComponentModel.TypeConverter

- Assembly: `System.ComponentModel.TypeConverter`
- Type: `System.ComponentModel.TypeConverter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 35 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 35 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 44 |

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
