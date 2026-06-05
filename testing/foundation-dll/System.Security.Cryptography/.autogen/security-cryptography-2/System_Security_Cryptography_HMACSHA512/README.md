# AutoTestGenerator: System.Security.Cryptography.HMACSHA512

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.HMACSHA512`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 11 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 6 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 5 |
| Total Subjects | 14 |

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

- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryHashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
