# AutoTestGenerator: System.Net.Sockets.UdpClient

- Assembly: `System.Net.Sockets`
- Type: `System.Net.Sockets.UdpClient`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 33 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 30 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 3 |
| Total Subjects | 69 |

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

- `Send (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Send (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Send (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
