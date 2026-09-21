# AutoTestGenerator: System.ModuleHandle

- Assembly: `System.Private.CoreLib`
- Type: `System.ModuleHandle`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 9 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 9 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 24 |

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
