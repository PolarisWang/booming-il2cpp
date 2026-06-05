# AutoTestGenerator: System.Security.Cryptography.Rfc2898DeriveBytes

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.Rfc2898DeriveBytes`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 9 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 4 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 5 |
| Total Subjects | 17 |

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

- `Reset (enumerator method projected by MLC)` — contains generic parameters, needs manual handling
- `Pbkdf2 (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Pbkdf2 (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Pbkdf2 (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Pbkdf2 (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
