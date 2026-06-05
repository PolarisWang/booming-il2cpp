# AutoTestGenerator: System.Net.Sockets.NetworkStream

- Assembly: `System.Net.Sockets`
- Type: `System.Net.Sockets.NetworkStream`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 20 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 18 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 48 |

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

- `Read (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `Write (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
