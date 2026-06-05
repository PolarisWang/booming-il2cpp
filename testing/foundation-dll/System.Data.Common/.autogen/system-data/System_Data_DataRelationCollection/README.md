# AutoTestGenerator: System.Data.DataRelationCollection

- Assembly: `System.Data.Common`
- Type: `System.Data.DataRelationCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 17 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 16 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 32 |

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

- `CopyTo (ICollection non-generic leak: DataRelation[])` — contains generic parameters, needs manual handling
