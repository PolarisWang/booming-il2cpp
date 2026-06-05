# AutoTestGenerator: System.Runtime.Serialization.SurrogateSelector

- Assembly: `System.Runtime.Serialization.Formatters`
- Type: `System.Runtime.Serialization.SurrogateSelector`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 5 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 3 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 5 |

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
