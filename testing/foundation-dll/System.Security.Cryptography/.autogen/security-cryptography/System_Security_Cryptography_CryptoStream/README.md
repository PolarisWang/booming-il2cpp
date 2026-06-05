# AutoTestGenerator: System.Security.Cryptography.CryptoStream

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.CryptoStream`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 23 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 22 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 54 |

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

- `Write (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
