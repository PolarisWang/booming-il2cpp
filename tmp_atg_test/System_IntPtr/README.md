# AutoTestGenerator: System.IntPtr

- Assembly: `System.Private.CoreLib`
- Type: `System.IntPtr`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 50 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 33 |
| Skipped (generic / unresolvable) | 17 |
| Total Subjects | 92 |

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
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Log2` — contains generic parameters, needs manual handling
- `Clamp` — contains generic parameters, needs manual handling
- `Max` — contains generic parameters, needs manual handling
- `Min` — contains generic parameters, needs manual handling
- `Abs` — contains generic parameters, needs manual handling
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryParse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
