# AutoTestGenerator: System.String

- Assembly: `System.Private.CoreLib`
- Type: `System.String`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 160 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 136 |
| Skipped (generic / unresolvable) | 23 |
| Total Subjects | 409 |

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

- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
- `CopyTo (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `TryCopyTo (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `Normalize` — contains generic parameters, needs manual handling
- `Normalize` — contains generic parameters, needs manual handling
- `Concat (ref struct parameter: System.ReadOnlySpan<System.Object>)` — contains generic parameters, needs manual handling
- `Concat (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Concat (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Concat (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Concat (ref struct parameter: System.ReadOnlySpan<System.String>)` — contains generic parameters, needs manual handling
- `Format (ref struct parameter: System.ReadOnlySpan<System.Object>)` — contains generic parameters, needs manual handling
- `Format (ref struct parameter: System.ReadOnlySpan<System.Object>)` — contains generic parameters, needs manual handling
- `Format (ref struct parameter: System.ReadOnlySpan<System.Object>)` — contains generic parameters, needs manual handling
- `Join (ref struct parameter: System.ReadOnlySpan<System.String>)` — contains generic parameters, needs manual handling
- `Join (ref struct parameter: System.ReadOnlySpan<System.String>)` — contains generic parameters, needs manual handling
- `Join (ref struct parameter: System.ReadOnlySpan<System.Object>)` — contains generic parameters, needs manual handling
- `Join (ref struct parameter: System.ReadOnlySpan<System.Object>)` — contains generic parameters, needs manual handling
- `Split (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Trim (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimStart (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
- `TrimEnd (ReadOnlySpan<char> overload)` — contains generic parameters, needs manual handling
