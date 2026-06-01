# AutoTestGenerator: System.Guid

- Assembly: `System.Private.CoreLib`
- Type: `System.Guid`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 27 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 7 |
| Benchmark-only (pure void / non-deterministic) | 6 |
| Skipped (generic / unresolvable) | 14 |
| Total Subjects | 22 |

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
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `ParseExact (ref struct)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryWriteBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
