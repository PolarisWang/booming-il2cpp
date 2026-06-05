# AutoTestGenerator: System.Runtime.Serialization.SerializationBinder

- Assembly: `System.Runtime.Serialization.Formatters`
- Type: `System.Runtime.Serialization.SerializationBinder`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 2 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 1 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 6 |

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
