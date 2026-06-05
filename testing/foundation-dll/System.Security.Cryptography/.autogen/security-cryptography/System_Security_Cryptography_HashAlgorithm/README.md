# AutoTestGenerator: System.Security.Cryptography.HashAlgorithm

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.HashAlgorithm`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 12 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 9 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 3 |
| Total Subjects | 25 |

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
- `TryComputeHash (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
