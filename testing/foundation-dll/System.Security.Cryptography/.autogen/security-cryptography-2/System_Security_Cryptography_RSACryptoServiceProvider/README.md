# AutoTestGenerator: System.Security.Cryptography.RSACryptoServiceProvider

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.RSACryptoServiceProvider`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 20 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 18 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 55 |

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

- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ImportEncryptedPkcs8PrivateKey (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
