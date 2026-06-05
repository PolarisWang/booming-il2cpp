# AutoTestGenerator: System.Security.Cryptography.SymmetricAlgorithm

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.SymmetricAlgorithm`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 38 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 18 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 20 |
| Total Subjects | 39 |

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
- `DecryptEcb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DecryptEcb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryDecryptEcb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `EncryptEcb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `EncryptEcb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryEncryptEcb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DecryptCbc (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DecryptCbc (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryDecryptCbc (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `EncryptCbc (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `EncryptCbc (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryEncryptCbc (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DecryptCfb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DecryptCfb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryDecryptCfb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `EncryptCfb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `EncryptCfb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryEncryptCfb (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
