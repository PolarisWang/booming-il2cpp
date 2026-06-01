# AutoTestGenerator: System.BitConverter

- Assembly: `System.Runtime`
- Type: `System.BitConverter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 64 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 64 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 120 |

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
