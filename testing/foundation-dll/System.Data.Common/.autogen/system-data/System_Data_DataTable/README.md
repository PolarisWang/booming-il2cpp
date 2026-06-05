# AutoTestGenerator: System.Data.DataTable

- Assembly: `System.Data.Common`
- Type: `System.Data.DataTable`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 63 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 61 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 111 |

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
