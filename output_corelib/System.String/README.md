# AutoTestGenerator: System.String

- Assembly: `System.Runtime`
- Type: `System.String`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 160 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 158 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 354 |

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
