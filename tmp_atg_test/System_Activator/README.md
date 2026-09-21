# AutoTestGenerator: System.Activator

- Assembly: `System.Private.CoreLib`
- Type: `System.Activator`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 13 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 13 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 56 |

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
