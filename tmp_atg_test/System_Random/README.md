# AutoTestGenerator: System.Random

- Assembly: `System.Private.CoreLib`
- Type: `System.Random`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 18 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 10 |
| Skipped (generic / unresolvable) | 6 |
| Total Subjects | 27 |

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

- `NextBytes (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `GetItems (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `GetItems (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `Shuffle (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
- `GetString (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `GetHexString (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
