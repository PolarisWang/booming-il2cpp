# AutoTestGenerator: System.Attribute

- Assembly: `System.Private.CoreLib`
- Type: `System.Attribute`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 34 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 34 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 82 |

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
