# AutoTestGenerator: System.Net.Sockets.Socket

- Assembly: `System.Net.Sockets`
- Type: `System.Net.Sockets.Socket`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 157 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 141 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 16 |
| Total Subjects | 302 |

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
- `SendFile (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `SendTo (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `SendTo (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `SendTo (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Receive (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `Receive (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `Receive (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `ReceiveMessageFrom (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `ReceiveFrom (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `ReceiveFrom (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `ReceiveFrom (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `SetRawSocketOption (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `GetRawSocketOption (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
