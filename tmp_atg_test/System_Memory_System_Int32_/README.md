# AutoTestGenerator: System.Memory<System.Int32>

- Assembly: `System.Private.CoreLib`
- Type: `System.Memory<System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 6 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 5 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 11 |

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
