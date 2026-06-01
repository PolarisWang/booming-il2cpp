# AutoTestGenerator: System.DateTime

- Assembly: `System.Private.CoreLib`
- Type: `System.DateTime`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 62 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 37 |
| Benchmark-only (pure void / non-deterministic) | 14 |
| Skipped (generic / unresolvable) | 11 |
| Total Subjects | 88 |

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
- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
