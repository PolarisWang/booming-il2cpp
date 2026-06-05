# AutoTestGenerator: System.Formats.Asn1.AsnReader

- Assembly: `System.Formats.Asn1`
- Type: `System.Formats.Asn1.AsnReader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 37 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 31 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 4 |
| Total Subjects | 41 |

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

- `TryReadBitString (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryReadOctetString (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryReadCharacterStringBytes (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `TryReadCharacterString (ref struct parameter: System.Span<System.Char>)` — contains generic parameters, needs manual handling
