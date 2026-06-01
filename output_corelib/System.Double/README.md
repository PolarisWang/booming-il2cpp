# AutoTestGenerator: System.Double

- Assembly: `System.Private.CoreLib`
- Type: `System.Double`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 115 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 90 |
| Benchmark-only (pure void / non-deterministic) | 13 |
| Skipped (generic / unresolvable) | 12 |
| Total Subjects | 240 |

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
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
