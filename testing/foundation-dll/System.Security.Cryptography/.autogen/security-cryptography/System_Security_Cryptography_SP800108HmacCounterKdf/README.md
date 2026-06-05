# AutoTestGenerator: System.Security.Cryptography.SP800108HmacCounterKdf

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.SP800108HmacCounterKdf`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 13 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 8 |
| Total Subjects | 16 |

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

- `DeriveBytes (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DeriveBytes (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DeriveBytes (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DeriveBytes (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DeriveKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DeriveKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DeriveKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `DeriveKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
