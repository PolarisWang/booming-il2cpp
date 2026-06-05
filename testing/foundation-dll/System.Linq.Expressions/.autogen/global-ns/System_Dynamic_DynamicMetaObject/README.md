# AutoTestGenerator: System.Dynamic.DynamicMetaObject

- Assembly: `System.Linq.Expressions`
- Type: `System.Dynamic.DynamicMetaObject`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 14 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 13 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 25 |

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

## Skipped Methods

- `Create` — contains generic parameters, needs manual handling
