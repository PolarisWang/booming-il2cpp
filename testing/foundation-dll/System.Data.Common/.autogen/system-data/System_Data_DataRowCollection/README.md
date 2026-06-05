# AutoTestGenerator: System.Data.DataRowCollection

- Assembly: `System.Data.Common`
- Type: `System.Data.DataRowCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 14 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 13 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 22 |

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

- `CopyTo (ICollection non-generic leak: DataRow[])` — contains generic parameters, needs manual handling
