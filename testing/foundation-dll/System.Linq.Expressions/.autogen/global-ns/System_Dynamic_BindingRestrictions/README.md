# AutoTestGenerator: System.Dynamic.BindingRestrictions

- Assembly: `System.Linq.Expressions`
- Type: `System.Dynamic.BindingRestrictions`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 6 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 6 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 7 |

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
