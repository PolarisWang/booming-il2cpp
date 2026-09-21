# AutoTestGenerator: System.MathF

- Assembly: `System.Private.CoreLib`
- Type: `System.MathF`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 44 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 34 |
| Skipped (generic / unresolvable) | 10 |
| Total Subjects | 81 |

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

- `Acosh` — contains generic parameters, needs manual handling
- `Log` — contains generic parameters, needs manual handling
- `Log2` — contains generic parameters, needs manual handling
- `Log10` — contains generic parameters, needs manual handling
- `Abs` — contains generic parameters, needs manual handling
- `Log` — contains generic parameters, needs manual handling
- `Max` — contains generic parameters, needs manual handling
- `Min` — contains generic parameters, needs manual handling
- `ReciprocalEstimate` — contains generic parameters, needs manual handling
- `ReciprocalSqrtEstimate` — contains generic parameters, needs manual handling
