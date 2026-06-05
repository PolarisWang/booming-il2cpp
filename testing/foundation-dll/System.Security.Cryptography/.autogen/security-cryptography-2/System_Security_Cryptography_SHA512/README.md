# AutoTestGenerator: System.Security.Cryptography.SHA512

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.SHA512`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 10 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 4 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 6 |
| Total Subjects | 9 |

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
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryHashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
