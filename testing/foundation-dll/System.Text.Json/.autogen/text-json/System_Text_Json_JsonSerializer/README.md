# AutoTestGenerator: System.Text.Json.JsonSerializer

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.JsonSerializer`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 94 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 64 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 30 |
| Total Subjects | 108 |

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

- `Deserialize (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Deserialize (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Deserialize (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `SerializeToDocument (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `SerializeToElement (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `SerializeToNode (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
- `DeserializeAsync (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Deserialize (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Deserialize (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `Deserialize (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `SerializeToUtf8Bytes (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `SerializeAsync (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Serialize (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Serialize (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `SerializeAsync (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `Serialize (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
