# AutoTestGenerator: System.Runtime.Serialization.FormatterConverter

- Assembly: `System.Runtime.Serialization.Formatters`
- Type: `System.Runtime.Serialization.FormatterConverter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 16 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 16 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 17 |

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
