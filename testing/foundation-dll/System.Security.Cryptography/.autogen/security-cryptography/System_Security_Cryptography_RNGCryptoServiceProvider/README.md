# AutoTestGenerator: System.Security.Cryptography.RNGCryptoServiceProvider

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.RNGCryptoServiceProvider`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 5 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 3 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 11 |

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

- `GetBytes (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `GetNonZeroBytes (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
