# AutoTestGenerator: System.TimeSpan

- Assembly: `System.Private.CoreLib`
- Type: `System.TimeSpan`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 51 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 32 |
| Benchmark-only (pure void / non-deterministic) | 8 |
| Skipped (generic / unresolvable) | 11 |
| Total Subjects | 91 |

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

- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `ParseExact (ref struct)` — contains generic parameters, needs manual handling
- `ParseExact (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
