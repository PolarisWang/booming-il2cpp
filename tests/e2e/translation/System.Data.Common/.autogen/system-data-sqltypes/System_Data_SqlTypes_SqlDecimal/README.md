# AutoTestGenerator: System.Data.SqlTypes.SqlDecimal

- Assembly: `System.Data.Common`
- Type: `System.Data.SqlTypes.SqlDecimal`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 33 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 31 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 39 |

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

- `WriteTdsValue (ref struct parameter: System.Span<System.UInt32>)` — contains generic parameters, needs manual handling
- `Abs` — contains generic parameters, needs manual handling
