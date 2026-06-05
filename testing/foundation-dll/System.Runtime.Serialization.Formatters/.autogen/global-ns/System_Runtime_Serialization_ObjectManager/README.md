# AutoTestGenerator: System.Runtime.Serialization.ObjectManager

- Assembly: `System.Runtime.Serialization.Formatters`
- Type: `System.Runtime.Serialization.ObjectManager`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 12 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 11 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 31 |

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
