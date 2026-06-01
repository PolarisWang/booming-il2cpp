# AutoTestGenerator: System.Text.StringBuilder

- Assembly: `System.Runtime`
- Type: `System.Text.StringBuilder`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 86 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 84 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 188 |

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
