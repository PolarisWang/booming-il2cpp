# AutoTestGenerator: System.Text.Encoding

- Assembly: `System.Private.CoreLib`
- Type: `System.Text.Encoding`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 46 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 12 |
| Benchmark-only (pure void / non-deterministic) | 27 |
| Skipped (generic / unresolvable) | 7 |
| Total Subjects | 85 |

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

- `GetByteCount (ref struct)` — contains generic parameters, needs manual handling
- `GetBytes (ref struct)` — contains generic parameters, needs manual handling
- `TryGetBytes (ref struct)` — contains generic parameters, needs manual handling
- `GetCharCount (ref struct)` — contains generic parameters, needs manual handling
- `GetChars (ref struct)` — contains generic parameters, needs manual handling
- `TryGetChars (ref struct)` — contains generic parameters, needs manual handling
- `GetString (ref struct)` — contains generic parameters, needs manual handling
