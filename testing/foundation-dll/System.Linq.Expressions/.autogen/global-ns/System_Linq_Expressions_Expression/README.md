# AutoTestGenerator: System.Linq.Expressions.Expression

- Assembly: `System.Linq.Expressions`
- Type: `System.Linq.Expressions.Expression`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 312 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 310 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 482 |

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

- `Invoke` — contains generic parameters, needs manual handling
- `Invoke` — contains generic parameters, needs manual handling
