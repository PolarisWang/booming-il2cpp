# AutoTestGenerator: System.DateTimeOffset

- Assembly: `System.Private.CoreLib`
- Type: `System.DateTimeOffset`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 47 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 35 |
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

- `Parse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ParseExact (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `ParseExact (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryFormat (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParseExact (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
