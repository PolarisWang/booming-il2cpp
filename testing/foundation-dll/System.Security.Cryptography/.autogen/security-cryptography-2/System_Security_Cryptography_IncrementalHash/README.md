# AutoTestGenerator: System.Security.Cryptography.IncrementalHash

- Assembly: `System.Security.Cryptography`
- Type: `System.Security.Cryptography.IncrementalHash`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 14 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 6 |
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

- `AppendData (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `GetHashAndReset (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryGetHashAndReset (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `GetCurrentHash (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryGetCurrentHash (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `CreateHMAC (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
