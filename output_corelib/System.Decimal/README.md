# AutoTestGenerator: System.Decimal

- Assembly: `System.Private.CoreLib`
- Type: `System.Decimal`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 70 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 50 |
| Benchmark-only (pure void / non-deterministic) | 6 |
| Skipped (generic / unresolvable) | 14 |
| Total Subjects | 130 |

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

- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `GetBits (ref struct)` — contains generic parameters, needs manual handling
- `TryGetBits (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
