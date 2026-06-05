# AutoTestGenerator: System.Formats.Asn1.Asn1Tag

- Assembly: `System.Formats.Asn1`
- Type: `System.Formats.Asn1.Asn1Tag`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 8 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 4 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 4 |
| Total Subjects | 4 |

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

- `TryDecode (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Decode (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryEncode (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `Encode (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
