# AutoTestGenerator: System.Enum

- Assembly: `System.Runtime`
- Type: `System.Enum`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 41 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 41 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 69 |

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
