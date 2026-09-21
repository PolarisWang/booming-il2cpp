# AutoTestGenerator: System.Single

- Assembly: `System.Private.CoreLib`
- Type: `System.Single`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 115 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 88 |
| Skipped (generic / unresolvable) | 27 |
| Total Subjects | 216 |

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

- `TryFormat (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Log2` — contains generic parameters, needs manual handling
- `Ieee754Remainder` — contains generic parameters, needs manual handling
- `Lerp` — contains generic parameters, needs manual handling
- `ReciprocalEstimate` — contains generic parameters, needs manual handling
- `ReciprocalSqrtEstimate` — contains generic parameters, needs manual handling
- `Acosh` — contains generic parameters, needs manual handling
- `Log` — contains generic parameters, needs manual handling
- `Log` — contains generic parameters, needs manual handling
- `Log10` — contains generic parameters, needs manual handling
- `Clamp` — contains generic parameters, needs manual handling
- `Max` — contains generic parameters, needs manual handling
- `Min` — contains generic parameters, needs manual handling
- `Abs` — contains generic parameters, needs manual handling
- `MultiplyAddEstimate` — contains generic parameters, needs manual handling
- `RootN` — contains generic parameters, needs manual handling
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
