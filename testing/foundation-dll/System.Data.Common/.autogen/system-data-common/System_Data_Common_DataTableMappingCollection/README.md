# AutoTestGenerator: System.Data.Common.DataTableMappingCollection

- Assembly: `System.Data.Common`
- Type: `System.Data.Common.DataTableMappingCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 21 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 20 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 35 |

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

- `CopyTo (ICollection non-generic leak: DataTableMapping[])` — contains generic parameters, needs manual handling
