# AutoTestGenerator: System.String

- Assembly: `System.Private.CoreLib`
- Type: `System.String`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 160 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 107 |
| Benchmark-only (pure void / non-deterministic) | 33 |
| Skipped (generic / unresolvable) | 20 |
| Total Subjects | 328 |

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

- `Create (ref struct)` — contains generic parameters, needs manual handling
- `Create (ref struct)` — contains generic parameters, needs manual handling
- `CopyTo (ref struct)` — contains generic parameters, needs manual handling
- `TryCopyTo (ref struct)` — contains generic parameters, needs manual handling
- `Concat (ref struct)` — contains generic parameters, needs manual handling
- `Concat (ref struct)` — contains generic parameters, needs manual handling
- `Concat (ref struct)` — contains generic parameters, needs manual handling
- `Concat (ref struct)` — contains generic parameters, needs manual handling
- `Concat (ref struct)` — contains generic parameters, needs manual handling
- `Format (ref struct)` — contains generic parameters, needs manual handling
- `Format (ref struct)` — contains generic parameters, needs manual handling
- `Format (ref struct)` — contains generic parameters, needs manual handling
- `Join (ref struct)` — contains generic parameters, needs manual handling
- `Join (ref struct)` — contains generic parameters, needs manual handling
- `Join (ref struct)` — contains generic parameters, needs manual handling
- `Join (ref struct)` — contains generic parameters, needs manual handling
- `Split (ref struct)` — contains generic parameters, needs manual handling
- `Trim (ref struct)` — contains generic parameters, needs manual handling
- `TrimStart (ref struct)` — contains generic parameters, needs manual handling
- `TrimEnd (ref struct)` — contains generic parameters, needs manual handling
