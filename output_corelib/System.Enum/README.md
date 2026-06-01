# AutoTestGenerator: System.Enum

- Assembly: `System.Private.CoreLib`
- Type: `System.Enum`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 41 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 9 |
| Benchmark-only (pure void / non-deterministic) | 23 |
| Skipped (generic / unresolvable) | 9 |
| Total Subjects | 55 |

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
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
