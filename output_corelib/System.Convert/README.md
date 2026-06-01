# AutoTestGenerator: System.Convert

- Assembly: `System.Private.CoreLib`
- Type: `System.Convert`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 294 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 221 |
| Benchmark-only (pure void / non-deterministic) | 58 |
| Skipped (generic / unresolvable) | 15 |
| Total Subjects | 534 |

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

- `ToBase64String (ref struct)` — contains generic parameters, needs manual handling
- `TryToBase64Chars (ref struct)` — contains generic parameters, needs manual handling
- `TryFromBase64String (ref struct)` — contains generic parameters, needs manual handling
- `TryFromBase64Chars (ref struct)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct)` — contains generic parameters, needs manual handling
- `ToHexString (ref struct)` — contains generic parameters, needs manual handling
- `TryToHexString (ref struct)` — contains generic parameters, needs manual handling
- `TryToHexString (ref struct)` — contains generic parameters, needs manual handling
- `ToHexStringLower (ref struct)` — contains generic parameters, needs manual handling
- `TryToHexStringLower (ref struct)` — contains generic parameters, needs manual handling
- `TryToHexStringLower (ref struct)` — contains generic parameters, needs manual handling
