# AutoTestGenerator: System.Convert

- Assembly: `System.Private.CoreLib`
- Type: `System.Convert`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 294 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 277 |
| Skipped (generic / unresolvable) | 17 |
| Total Subjects | 579 |

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

- `ToBase64String (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryToBase64Chars (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryFromBase64String (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryFromBase64Chars (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `FromHexString (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ToHexString (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryToHexString (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryToHexString (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ToHexStringLower` — contains generic parameters, needs manual handling
- `ToHexStringLower` — contains generic parameters, needs manual handling
- `ToHexStringLower` — contains generic parameters, needs manual handling
- `TryToHexStringLower` — contains generic parameters, needs manual handling
- `TryToHexStringLower` — contains generic parameters, needs manual handling
