# AutoTestGenerator: System.Data.DataSet

- Assembly: `System.Data.Common`
- Type: `System.Data.DataSet`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 60 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 58 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 106 |

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

- `GetObjectData` — contains generic parameters, needs manual handling
- `Reset (enumerator method projected by MLC)` — contains generic parameters, needs manual handling
