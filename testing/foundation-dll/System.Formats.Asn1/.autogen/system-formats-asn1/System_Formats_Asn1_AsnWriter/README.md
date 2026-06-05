# AutoTestGenerator: System.Formats.Asn1.AsnWriter

- Assembly: `System.Formats.Asn1`
- Type: `System.Formats.Asn1.AsnWriter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 35 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 19 |
| Skipped (generic / unresolvable) | 14 |
| Total Subjects | 31 |

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

- `Reset (enumerator method projected by MLC)` — contains generic parameters, needs manual handling
- `TryEncode (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `Encode (ref struct parameter: System.Span<System.Byte>)` — contains generic parameters, needs manual handling
- `EncodedValueEquals (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `WriteEncodedValue (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `WriteBitString (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `WriteInteger (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `WriteIntegerUnsigned (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `PushOctetString (ref struct return type: AsnWriter.Scope)` — contains generic parameters, needs manual handling
- `WriteOctetString (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `WriteObjectIdentifier (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `PushSequence (ref struct return type: AsnWriter.Scope)` — contains generic parameters, needs manual handling
- `PushSetOf (ref struct return type: AsnWriter.Scope)` — contains generic parameters, needs manual handling
- `WriteCharacterString (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
