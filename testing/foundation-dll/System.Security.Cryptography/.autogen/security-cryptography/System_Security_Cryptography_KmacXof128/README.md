# AutoTestGenerator: System.Security.Cryptography.KmacXof128

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.KmacXof128`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 17 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 10 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 7 |
| Total Subjects | 26 |

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

- `AppendData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `GetHashAndReset (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `GetCurrentHash (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `HashData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
