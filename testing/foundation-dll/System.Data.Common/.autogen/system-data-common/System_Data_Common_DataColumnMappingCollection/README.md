# AutoTestGenerator: System.Data.Common.DataColumnMappingCollection

- Assembly: `System.Data.Common`
- Type: `System.Data.Common.DataColumnMappingCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 22 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 21 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 36 |

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

- `CopyTo (ICollection non-generic leak: DataColumnMapping[])` — contains generic parameters, needs manual handling
