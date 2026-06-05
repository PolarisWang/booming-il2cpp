# AutoTestGenerator: System.Runtime.InteropServices.RuntimeEnvironment

- Assembly: `System.Runtime.InteropServices`
- Type: `System.Runtime.InteropServices.RuntimeEnvironment`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 5 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 5 |

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
