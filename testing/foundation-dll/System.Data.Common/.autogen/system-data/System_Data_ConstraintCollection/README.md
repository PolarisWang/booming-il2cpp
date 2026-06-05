# AutoTestGenerator: System.Data.ConstraintCollection

- Assembly: `System.Data.Common`
- Type: `System.Data.ConstraintCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 15 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 14 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 30 |

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

- `CopyTo (ICollection non-generic leak: Constraint[])` — contains generic parameters, needs manual handling
