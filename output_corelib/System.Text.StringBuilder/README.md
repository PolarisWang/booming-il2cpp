# AutoTestGenerator: System.Text.StringBuilder

- Assembly: `System.Private.CoreLib`
- Type: `System.Text.StringBuilder`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 86 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 54 |
| Benchmark-only (pure void / non-deterministic) | 15 |
| Skipped (generic / unresolvable) | 17 |
| Total Subjects | 161 |

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

- `GetChunks (ref struct)` — contains generic parameters, needs manual handling
- `CopyTo (ref struct)` — contains generic parameters, needs manual handling
- `Append (ref struct)` — contains generic parameters, needs manual handling
- `Append (ref struct)` — contains generic parameters, needs manual handling
- `Append (ref struct)` — contains generic parameters, needs manual handling
- `AppendLine (ref struct)` — contains generic parameters, needs manual handling
- `AppendLine (ref struct)` — contains generic parameters, needs manual handling
- `AppendJoin (ref struct)` — contains generic parameters, needs manual handling
- `AppendJoin (ref struct)` — contains generic parameters, needs manual handling
- `AppendJoin (ref struct)` — contains generic parameters, needs manual handling
- `AppendJoin (ref struct)` — contains generic parameters, needs manual handling
- `Insert (ref struct)` — contains generic parameters, needs manual handling
- `AppendFormat (ref struct)` — contains generic parameters, needs manual handling
- `AppendFormat (ref struct)` — contains generic parameters, needs manual handling
- `AppendFormat (ref struct)` — contains generic parameters, needs manual handling
- `Replace (ref struct)` — contains generic parameters, needs manual handling
- `Replace (ref struct)` — contains generic parameters, needs manual handling
