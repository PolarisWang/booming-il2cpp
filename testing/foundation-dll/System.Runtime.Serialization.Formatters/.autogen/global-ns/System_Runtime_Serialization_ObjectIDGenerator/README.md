# AutoTestGenerator: System.Runtime.Serialization.ObjectIDGenerator

- Assembly: `System.Runtime.Serialization.Formatters`
- Type: `System.Runtime.Serialization.ObjectIDGenerator`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 2 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 4 |

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
