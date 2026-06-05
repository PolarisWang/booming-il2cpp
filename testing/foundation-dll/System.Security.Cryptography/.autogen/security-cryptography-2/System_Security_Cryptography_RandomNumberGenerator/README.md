# AutoTestGenerator: System.Security.Cryptography.RandomNumberGenerator

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.RandomNumberGenerator`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 18 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 10 |
| Total Subjects | 22 |

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
- `GetBytes (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `GetNonZeroBytes (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `Fill (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `GetItems (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `GetItems (ref struct parameter: System.ReadOnlySpan<System.Int32>)` — contains generic parameters, needs manual handling
- `GetString (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `GetHexString (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
- `Shuffle (ref struct parameter: System.Span<System.Int32>)` — contains generic parameters, needs manual handling
