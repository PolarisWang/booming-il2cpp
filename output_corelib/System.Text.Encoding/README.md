# AutoTestGenerator: System.Text.Encoding

- Assembly: `System.Runtime`
- Type: `System.Text.Encoding`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 46 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 45 |
| Benchmark-only (pure void / non-deterministic) | 1 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 92 |

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
