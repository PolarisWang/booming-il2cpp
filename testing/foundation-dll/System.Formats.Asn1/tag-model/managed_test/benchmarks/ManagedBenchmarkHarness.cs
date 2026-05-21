// Auto-generated managed benchmark harness
// Family: tag-model, Assembly: System.Formats.Asn1
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.Json;

class ManagedBenchmarkHarness
{
    static volatile int _g;  // volatile side-effect prevents JIT DCE

    struct MethodResult
    {
        public int MethodIndex { get; set; }
        public string MethodSubjectId { get; set; }
        public double ElapsedMilliseconds { get; set; }
        public int Iterations { get; set; }
        public bool IsBodyReal { get; set; }
        public bool IsException { get; set; }
    }



    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::AsConstructed:System.Formats.Asn1.Asn1Tag() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::AsConstructed:System.Formats.Asn1.Asn1Tag()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::AsPrimitive:System.Formats.Asn1.Asn1Tag() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::AsPrimitive:System.Formats.Asn1.Asn1Tag()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::CalculateEncodedSize:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::CalculateEncodedSize:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Decode:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Decode:System.Formats.Asn1.Asn1Tag(System.ReadOnlySpan{System.Byte},System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Encode:System.Int32(System.Span{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Encode:System.Int32(System.Span{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Equals:System.Boolean(System.Formats.Asn1.Asn1Tag) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Equals:System.Boolean(System.Formats.Asn1.Asn1Tag)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Equals:System.Boolean(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Equals:System.Boolean(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_IsConstructed:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_IsConstructed:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_TagClass:System.Formats.Asn1.TagClass() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_TagClass:System.Formats.Asn1.TagClass()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_TagValue:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_TagValue:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::GetHashCode:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::GetHashCode:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::HasSameClassAndValue:System.Boolean(System.Formats.Asn1.Asn1Tag) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::HasSameClassAndValue:System.Boolean(System.Formats.Asn1.Asn1Tag)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::op_Equality:System.Boolean(System.Formats.Asn1.Asn1Tag,System.Formats.Asn1.Asn1Tag) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::op_Equality:System.Boolean(System.Formats.Asn1.Asn1Tag,System.Formats.Asn1.Asn1Tag)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::op_Inequality:System.Boolean(System.Formats.Asn1.Asn1Tag,System.Formats.Asn1.Asn1Tag) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::op_Inequality:System.Boolean(System.Formats.Asn1.Asn1Tag,System.Formats.Asn1.Asn1Tag)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::ToString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::ToString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.Asn1Tag&,System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::TryDecode:System.Boolean(System.ReadOnlySpan{System.Byte},System.Formats.Asn1.Asn1Tag&,System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::TryEncode:System.Boolean(System.Span{System.Byte},System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::TryEncode:System.Boolean(System.Span{System.Byte},System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::.ctor:System.Void(System.Formats.Asn1.UniversalTagNumber,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::.ctor:System.Void(System.Formats.Asn1.UniversalTagNumber,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::.ctor:System.Void(System.Formats.Asn1.TagClass,System.Int32,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::.ctor:System.Void(System.Formats.Asn1.TagClass,System.Int32,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}
