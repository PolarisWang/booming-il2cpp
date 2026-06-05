# AutoTestGenerator: System.Text.Json.JsonElement

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.JsonElement`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 51 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 41 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 10 |
| Total Subjects | 57 |

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

- `GetProperty (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `GetProperty (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `TryGetProperty (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `TryGetProperty (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ValueEquals (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `ValueEquals (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `EnumerateArray (ref struct return type: JsonElement.ArrayEnumerator)` — contains generic parameters, needs manual handling
- `EnumerateObject (ref struct return type: JsonElement.ObjectEnumerator)` — contains generic parameters, needs manual handling
- `ParseValue (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `TryParseValue (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
