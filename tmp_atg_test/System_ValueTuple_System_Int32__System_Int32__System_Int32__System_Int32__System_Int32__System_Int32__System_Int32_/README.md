# AutoTestGenerator: System.ValueTuple<System.Int32, System.Int32, System.Int32, System.Int32, System.Int32, System.Int32, System.Int32>

- Assembly: `System.Private.CoreLib`
- Type: `System.ValueTuple<System.Int32, System.Int32, System.Int32, System.Int32, System.Int32, System.Int32, System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 1 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 1 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 1 |

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
