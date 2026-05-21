// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/nodes
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.Json.Serialization.Metadata;
using System.Threading;
using System.Threading.Tasks;

public static partial class NodesNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Nodes.JsonArray::Add{T}:System.Void(T)

    // [1] System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(System.Text.Json.Nodes.JsonNode)
    public static void Method1()
    {
        try { JsonArray.Add(null!); }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Nodes.JsonArray::Clear:System.Void()
    public static void Method2()
    {
        try { JsonArray.Clear(); }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Nodes.JsonArray::Contains:System.Boolean(System.Text.Json.Nodes.JsonNode)
    public static void Method3()
    {
        try { if (((JsonArray.Contains(null!)) ? 1 : 0) != ((JsonArray.Contains(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Nodes.JsonArray::Create:System.Text.Json.Nodes.JsonArray(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method4()
    {
        try { if ((int)(JsonArray.Create(null!, null!)) != (int)(JsonArray.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Count:System.Int32()
    public static void Method5()
    {
        try { if (JsonArray.Count != JsonArray.Count) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Nodes.JsonArray::GetEnumerator:System.Collections.Generic.IEnumerator{System.Text.Json.Nodes.JsonNode}()
    public static void Method6()
    {
        try { if ((int)(JsonArray.GetEnumerator()) != (int)(JsonArray.GetEnumerator())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues{T}:System.Collections.Generic.IEnumerable{T}()

    // [8] System.Text.Json/System.Text.Json.Nodes.JsonArray::IndexOf:System.Int32(System.Text.Json.Nodes.JsonNode)
    public static void Method8()
    {
        try { if (JsonArray.IndexOf(null!) != JsonArray.IndexOf(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)
    public static void Method9()
    {
        try { JsonArray.Insert(42, null!); }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.Nodes.JsonArray::Remove:System.Boolean(System.Text.Json.Nodes.JsonNode)
    public static void Method10()
    {
        try { if (((JsonArray.Remove(null!)) ? 1 : 0) != ((JsonArray.Remove(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAll:System.Int32(System.Func{System.Text.Json.Nodes.JsonNode,System.Boolean})

    // [12] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)
    public static void Method12()
    {
        try { JsonArray.RemoveAt(42); }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveRange:System.Void(System.Int32,System.Int32)
    public static void Method13()
    {
        try { JsonArray.RemoveRange(42, 42); }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.Nodes.JsonArray::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)
    public static void Method14()
    {
        try { JsonArray.WriteTo(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method15()
    {
        try { new JsonArray(null!); }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNodeOptions,System.Text.Json.Nodes.JsonNode{})
    public static void Method16()
    {
        try { new JsonArray(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNodeOptions,System.ReadOnlySpan{System.Text.Json.Nodes.JsonNode})
    public static void Method17()
    {
        try { new JsonArray(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNode{})
    public static void Method18()
    {
        try { new JsonArray(null!); }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.ReadOnlySpan{System.Text.Json.Nodes.JsonNode})
    public static void Method19()
    {
        try { new JsonArray(null!); }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsArray:System.Text.Json.Nodes.JsonArray()
    public static void Method20()
    {
        try { if ((int)(JsonNode.AsArray()) != (int)(JsonNode.AsArray())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsObject:System.Text.Json.Nodes.JsonObject()
    public static void Method21()
    {
        try { if ((int)(JsonNode.AsObject()) != (int)(JsonNode.AsObject())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsValue:System.Text.Json.Nodes.JsonValue()
    public static void Method22()
    {
        try { if ((int)(JsonNode.AsValue()) != (int)(JsonNode.AsValue())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepClone:System.Text.Json.Nodes.JsonNode()
    public static void Method23()
    {
        try { if ((int)(JsonNode.DeepClone()) != (int)(JsonNode.DeepClone())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)
    public static void Method24()
    {
        try { if (((JsonNode.DeepEquals(null!, null!)) ? 1 : 0) != ((JsonNode.DeepEquals(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode(System.Int32)
    public static void Method25()
    {
        try { if ((int)(JsonNode[42]) != (int)(JsonNode[42])) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode(System.String)
    public static void Method26()
    {
        try { if ((int)(JsonNode["hello"]) != (int)(JsonNode["hello"])) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Options:System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}()
    public static void Method27()
    {
        try { if ((int)(JsonNode.Options) != (int)(JsonNode.Options)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Parent:System.Text.Json.Nodes.JsonNode()
    public static void Method28()
    {
        try { if ((int)(JsonNode.Parent) != (int)(JsonNode.Parent)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Root:System.Text.Json.Nodes.JsonNode()
    public static void Method29()
    {
        try { if ((int)(JsonNode.Root) != (int)(JsonNode.Root)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetElementIndex:System.Int32()
    public static void Method30()
    {
        try { if (JsonNode.GetElementIndex() != JsonNode.GetElementIndex()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPath:System.String()
    public static void Method31()
    {
        try { if (((JsonNode.GetPath()).Length) != ((JsonNode.GetPath()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPropertyName:System.String()
    public static void Method32()
    {
        try { if (((JsonNode.GetPropertyName()).Length) != ((JsonNode.GetPropertyName()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue{T}:T()
    public static void Method33()
    {
        try { if ((int)(JsonNode.GetValue{T}()) != (int)(JsonNode.GetValue{T}())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValueKind:System.Text.Json.JsonValueKind()

    // [35] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Boolean(System.Text.Json.Nodes.JsonNode)
    public static void Method35()
    {
        try { if (((JsonNode.op_Explicit(null!)) ? 1 : 0) != ((JsonNode.op_Explicit(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Boolean}(System.Text.Json.Nodes.JsonNode)
    public static void Method36()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Byte(System.Text.Json.Nodes.JsonNode)
    public static void Method37()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Byte}(System.Text.Json.Nodes.JsonNode)
    public static void Method38()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Char(System.Text.Json.Nodes.JsonNode)
    public static void Method39()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Char}(System.Text.Json.Nodes.JsonNode)
    public static void Method40()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.DateTime(System.Text.Json.Nodes.JsonNode)
    public static void Method41()
    {
        try { if (((JsonNode.op_Explicit(null!)).GetHashCode()) != ((JsonNode.op_Explicit(null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.DateTime}(System.Text.Json.Nodes.JsonNode)
    public static void Method42()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.DateTimeOffset(System.Text.Json.Nodes.JsonNode)
    public static void Method43()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.DateTimeOffset}(System.Text.Json.Nodes.JsonNode)
    public static void Method44()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Decimal(System.Text.Json.Nodes.JsonNode)
    public static void Method45()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Decimal}(System.Text.Json.Nodes.JsonNode)
    public static void Method46()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Double(System.Text.Json.Nodes.JsonNode)
    public static void Method47()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Double}(System.Text.Json.Nodes.JsonNode)
    public static void Method48()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Guid(System.Text.Json.Nodes.JsonNode)
    public static void Method49()
    {
        try { if (((JsonNode.op_Explicit(null!)).GetHashCode()) != ((JsonNode.op_Explicit(null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Guid}(System.Text.Json.Nodes.JsonNode)
    public static void Method50()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int16(System.Text.Json.Nodes.JsonNode)
    public static void Method51()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int16}(System.Text.Json.Nodes.JsonNode)
    public static void Method52()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int32(System.Text.Json.Nodes.JsonNode)
    public static void Method53()
    {
        try { if (JsonNode.op_Explicit(null!) != JsonNode.op_Explicit(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int32}(System.Text.Json.Nodes.JsonNode)
    public static void Method54()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int64(System.Text.Json.Nodes.JsonNode)
    public static void Method55()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int64}(System.Text.Json.Nodes.JsonNode)
    public static void Method56()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.SByte(System.Text.Json.Nodes.JsonNode)
    public static void Method57()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.SByte}(System.Text.Json.Nodes.JsonNode)
    public static void Method58()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Single(System.Text.Json.Nodes.JsonNode)
    public static void Method59()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Single}(System.Text.Json.Nodes.JsonNode)
    public static void Method60()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [61] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.String(System.Text.Json.Nodes.JsonNode)
    public static void Method61()
    {
        try { if (((JsonNode.op_Explicit(null!)).Length) != ((JsonNode.op_Explicit(null!)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [62] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt16(System.Text.Json.Nodes.JsonNode)
    public static void Method62()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [63] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt16}(System.Text.Json.Nodes.JsonNode)
    public static void Method63()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [64] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt32(System.Text.Json.Nodes.JsonNode)
    public static void Method64()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [65] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt32}(System.Text.Json.Nodes.JsonNode)
    public static void Method65()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [66] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt64(System.Text.Json.Nodes.JsonNode)
    public static void Method66()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [67] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt64}(System.Text.Json.Nodes.JsonNode)
    public static void Method67()
    {
        try { if ((int)(JsonNode.op_Explicit(null!)) != (int)(JsonNode.op_Explicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [68] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Boolean)
    public static void Method68()
    {
        try { if ((int)(JsonNode.op_Implicit(true)) != (int)(JsonNode.op_Implicit(true))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [69] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Boolean})
    public static void Method69()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [70] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Byte)
    public static void Method70()
    {
        try { if ((int)(JsonNode.op_Implicit((byte)42)) != (int)(JsonNode.op_Implicit((byte)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [71] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Byte})
    public static void Method71()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [72] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Char)
    public static void Method72()
    {
        try { if ((int)(JsonNode.op_Implicit('A')) != (int)(JsonNode.op_Implicit('A'))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [73] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Char})
    public static void Method73()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [74] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.DateTime)
    public static void Method74()
    {
        try { if ((int)(JsonNode.op_Implicit(DateTime.UtcNow)) != (int)(JsonNode.op_Implicit(DateTime.UtcNow))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [75] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.DateTime})
    public static void Method75()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [76] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.DateTimeOffset)
    public static void Method76()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [77] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.DateTimeOffset})
    public static void Method77()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [78] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Decimal)
    public static void Method78()
    {
        try { if ((int)(JsonNode.op_Implicit(42m)) != (int)(JsonNode.op_Implicit(42m))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [79] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Decimal})
    public static void Method79()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [80] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Double)
    public static void Method80()
    {
        try { if ((int)(JsonNode.op_Implicit(42.0)) != (int)(JsonNode.op_Implicit(42.0))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [81] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Double})
    public static void Method81()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [82] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Guid)
    public static void Method82()
    {
        try { if ((int)(JsonNode.op_Implicit(Guid.Empty)) != (int)(JsonNode.op_Implicit(Guid.Empty))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [83] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Guid})
    public static void Method83()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [84] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int16)
    public static void Method84()
    {
        try { if ((int)(JsonNode.op_Implicit((short)42)) != (int)(JsonNode.op_Implicit((short)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [85] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int16})
    public static void Method85()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [86] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int32)
    public static void Method86()
    {
        try { if ((int)(JsonNode.op_Implicit(42)) != (int)(JsonNode.op_Implicit(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [87] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int32})
    public static void Method87()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [88] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int64)
    public static void Method88()
    {
        try { if ((int)(JsonNode.op_Implicit(42L)) != (int)(JsonNode.op_Implicit(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [89] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int64})
    public static void Method89()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [90] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.SByte)
    public static void Method90()
    {
        try { if ((int)(JsonNode.op_Implicit((sbyte)42)) != (int)(JsonNode.op_Implicit((sbyte)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [91] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.SByte})
    public static void Method91()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [92] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Single)
    public static void Method92()
    {
        try { if ((int)(JsonNode.op_Implicit(42.0f)) != (int)(JsonNode.op_Implicit(42.0f))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [93] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Single})
    public static void Method93()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [94] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.String)
    public static void Method94()
    {
        try { if ((int)(JsonNode.op_Implicit("hello")) != (int)(JsonNode.op_Implicit("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [95] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt16)
    public static void Method95()
    {
        try { if ((int)(JsonNode.op_Implicit((ushort)42)) != (int)(JsonNode.op_Implicit((ushort)42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [96] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt16})
    public static void Method96()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [97] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt32)
    public static void Method97()
    {
        try { if ((int)(JsonNode.op_Implicit(42u)) != (int)(JsonNode.op_Implicit(42u))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [98] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt32})
    public static void Method98()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [99] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt64)
    public static void Method99()
    {
        try { if ((int)(JsonNode.op_Implicit(42uL)) != (int)(JsonNode.op_Implicit(42uL))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [100] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt64})
    public static void Method100()
    {
        try { if ((int)(JsonNode.op_Implicit(null!)) != (int)(JsonNode.op_Implicit(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [101] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method101()
    {
    var refLocal_0 = null!;
        try { if ((int)(JsonNode.Parse(ref refLocal_0, null!)) != (int)(JsonNode.Parse(ref refLocal_0, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [102] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)
    public static void Method102()
    {
        try { if ((int)(JsonNode.Parse("hello", null!, null!)) != (int)(JsonNode.Parse("hello", null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [103] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.ReadOnlySpan{System.Byte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)
    public static void Method103()
    {
        try { if ((int)(JsonNode.Parse(null!, null!, null!)) != (int)(JsonNode.Parse(null!, null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [104] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)
    public static void Method104()
    {
        try { if ((int)(JsonNode.Parse(null!, null!, null!)) != (int)(JsonNode.Parse(null!, null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [105] System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task{System.Text.Json.Nodes.JsonNode}(System.IO.Stream,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken)
    public static void Method105()
    {
        try { if (((JsonNode.ParseAsync(null!, null!, null!, null!)).GetHashCode()) != ((JsonNode.ParseAsync(null!, null!, null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [106] System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith{T}:System.Void(T)
    public static void Method106()
    {
        // TODO: System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith{T}:System.Void(T) could not be auto-generated
    }

    // [107] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)

    // [108] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.String,System.Text.Json.Nodes.JsonNode)
    public static void Method108()
    {
        try { JsonNode["hello", null!]; }
        catch { _exitCode = 1; }
    }

    // [109] System.Text.Json/System.Text.Json.Nodes.JsonNode::ToJsonString:System.String(System.Text.Json.JsonSerializerOptions)
    public static void Method109()
    {
        try { if (((JsonNode.ToJsonString(null!)).Length) != ((JsonNode.ToJsonString(null!)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [110] System.Text.Json/System.Text.Json.Nodes.JsonNode::ToString:System.String()
    public static void Method110()
    {
        try { if (((JsonNode.ToString()).Length) != ((JsonNode.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [111] System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)
    public static void Method111()
    {
        try { JsonNode.WriteTo(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [112] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Parent:System.Void(System.Text.Json.Nodes.JsonNode)
    public static void Method112()
    {
        try { JsonNode.Parent = null!; }
        catch { _exitCode = 1; }
    }

    // [113] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode()
    public static void Method113()
    {
        try { if ((int)(JsonNode[]) != (int)(JsonNode[])) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [114] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.Text.Json.Nodes.JsonNode)
    public static void Method114()
    {
        try { JsonNode[null!]; }
        catch { _exitCode = 1; }
    }

    // [115] System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)
    public static void Method115()
    {
        try { JsonObject.Add("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [116] System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode})
    public static void Method116()
    {
        try { JsonObject.Add(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [117] System.Text.Json/System.Text.Json.Nodes.JsonObject::Clear:System.Void()
    public static void Method117()
    {
        try { JsonObject.Clear(); }
        catch { _exitCode = 1; }
    }

    // [118] System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)
    public static void Method118()
    {
        try { if (((JsonObject.ContainsKey("hello")) ? 1 : 0) != ((JsonObject.ContainsKey("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [119] System.Text.Json/System.Text.Json.Nodes.JsonObject::Create:System.Text.Json.Nodes.JsonObject(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method119()
    {
        try { if ((int)(JsonObject.Create(null!, null!)) != (int)(JsonObject.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [120] System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Count:System.Int32()
    public static void Method120()
    {
        try { if (JsonObject.Count != JsonObject.Count) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [121] System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}(System.Int32)
    public static void Method121()
    {
        try { if ((int)(JsonObject.GetAt(42)) != (int)(JsonObject.GetAt(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [122] System.Text.Json/System.Text.Json.Nodes.JsonObject::GetEnumerator:System.Collections.Generic.IEnumerator{System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}}()
    public static void Method122()
    {
        try { if ((int)(JsonObject.GetEnumerator()) != (int)(JsonObject.GetEnumerator())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [123] System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)
    public static void Method123()
    {
        try { if (JsonObject.IndexOf("hello") != JsonObject.IndexOf("hello")) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [124] System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)
    public static void Method124()
    {
        try { JsonObject.Insert(42, "hello", null!); }
        catch { _exitCode = 1; }
    }

    // [125] System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)
    public static void Method125()
    {
        try { if (((JsonObject.Remove("hello")) ? 1 : 0) != ((JsonObject.Remove("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [126] System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)
    public static void Method126()
    {
        try { JsonObject.RemoveAt(42); }
        catch { _exitCode = 1; }
    }

    // [127] System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)
    public static void Method127()
    {
        try { JsonObject.SetAt(42, "hello", null!); }
        catch { _exitCode = 1; }
    }

    // [128] System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)
    public static void Method128()
    {
        try { JsonObject.SetAt(42, null!); }
        catch { _exitCode = 1; }
    }

    // [129] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryAdd:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode)
    public static void Method129()
    {
        try { if (((JsonObject.TryAdd("hello", null!)) ? 1 : 0) != ((JsonObject.TryAdd("hello", null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [130] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryAdd:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode,System.Int32&)
    public static void Method130()
    {
    var refLocal_2 = 42;
        try { if (((JsonObject.TryAdd("hello", null!, ref refLocal_2)) ? 1 : 0) != ((JsonObject.TryAdd("hello", null!, ref refLocal_2)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [131] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode&)
    public static void Method131()
    {
    var refLocal_1 = null!;
        try { if (((JsonObject.TryGetPropertyValue("hello", ref refLocal_1)) ? 1 : 0) != ((JsonObject.TryGetPropertyValue("hello", ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [132] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode&,System.Int32&)
    public static void Method132()
    {
    var refLocal_1 = null!;
    var refLocal_2 = 42;
        try { if (((JsonObject.TryGetPropertyValue("hello", ref refLocal_1, ref refLocal_2)) ? 1 : 0) != ((JsonObject.TryGetPropertyValue("hello", ref refLocal_1, ref refLocal_2)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [133] System.Text.Json/System.Text.Json.Nodes.JsonObject::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)
    public static void Method133()
    {
        try { JsonObject.WriteTo(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [134] System.Text.Json/System.Text.Json.Nodes.JsonObject::.ctor:System.Void(System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method134()
    {
        try { new JsonObject(null!); }
        catch { _exitCode = 1; }
    }

    // [135] System.Text.Json/System.Text.Json.Nodes.JsonObject::.ctor:System.Void(System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method135()
    {
        try { new JsonObject(null!, null!, null!); }
        catch { _exitCode = 1; }
    }

    // [136] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Boolean,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method136()
    {
        try { if ((int)(JsonValue.Create(true, null!)) != (int)(JsonValue.Create(true, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [137] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Boolean},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method137()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [138] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Byte,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method138()
    {
        try { if ((int)(JsonValue.Create((byte)42, null!)) != (int)(JsonValue.Create((byte)42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [139] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Byte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method139()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [140] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Char,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method140()
    {
        try { if ((int)(JsonValue.Create('A', null!)) != (int)(JsonValue.Create('A', null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [141] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Char},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method141()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [142] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.DateTime,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method142()
    {
        try { if ((int)(JsonValue.Create(DateTime.UtcNow, null!)) != (int)(JsonValue.Create(DateTime.UtcNow, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [143] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.DateTime},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method143()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [144] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.DateTimeOffset,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method144()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [145] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.DateTimeOffset},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method145()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [146] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Decimal,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method146()
    {
        try { if ((int)(JsonValue.Create(42m, null!)) != (int)(JsonValue.Create(42m, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [147] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Decimal},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method147()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [148] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Double,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method148()
    {
        try { if ((int)(JsonValue.Create(42.0, null!)) != (int)(JsonValue.Create(42.0, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [149] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Double},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method149()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [150] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Guid,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method150()
    {
        try { if ((int)(JsonValue.Create(Guid.Empty, null!)) != (int)(JsonValue.Create(Guid.Empty, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [151] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Guid},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method151()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [152] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int16,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method152()
    {
        try { if ((int)(JsonValue.Create((short)42, null!)) != (int)(JsonValue.Create((short)42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [153] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int16},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method153()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [154] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int32,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method154()
    {
        try { if ((int)(JsonValue.Create(42, null!)) != (int)(JsonValue.Create(42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [155] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int32},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method155()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [156] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int64,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method156()
    {
        try { if ((int)(JsonValue.Create(42L, null!)) != (int)(JsonValue.Create(42L, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [157] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int64},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method157()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [158] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.SByte,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method158()
    {
        try { if ((int)(JsonValue.Create((sbyte)42, null!)) != (int)(JsonValue.Create((sbyte)42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [159] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.SByte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method159()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [160] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Single,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method160()
    {
        try { if ((int)(JsonValue.Create(42.0f, null!)) != (int)(JsonValue.Create(42.0f, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [161] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Single},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method161()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [162] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.String,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method162()
    {
        try { if ((int)(JsonValue.Create("hello", null!)) != (int)(JsonValue.Create("hello", null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [163] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt16,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method163()
    {
        try { if ((int)(JsonValue.Create((ushort)42, null!)) != (int)(JsonValue.Create((ushort)42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [164] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt16},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method164()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [165] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt32,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method165()
    {
        try { if ((int)(JsonValue.Create(42u, null!)) != (int)(JsonValue.Create(42u, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [166] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt32},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method166()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [167] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt64,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method167()
    {
        try { if ((int)(JsonValue.Create(42uL, null!)) != (int)(JsonValue.Create(42uL, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [168] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt64},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method168()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [169] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method169()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [170] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Text.Json.JsonElement},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method170()
    {
        try { if ((int)(JsonValue.Create(null!, null!)) != (int)(JsonValue.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [171] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method171()
    {
        // TODO: System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) could not be auto-generated
    }

    // [172] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Text.Json.Serialization.Metadata.JsonTypeInfo{T},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void Method172()
    {
        // TODO: System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Text.Json.Serialization.Metadata.JsonTypeInfo{T},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}) could not be auto-generated
    }

    // [173] System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue{T}:System.Boolean(T&)
    public static void Method173()
    {
        // TODO: System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue{T}:System.Boolean(T&) could not be auto-generated
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: CustomEntryMethod0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: CustomEntryMethod7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: CustomEntryMethod11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: Method19(); break;
                case 20: Method20(); break;
                case 21: Method21(); break;
                case 22: Method22(); break;
                case 23: Method23(); break;
                case 24: Method24(); break;
                case 25: Method25(); break;
                case 26: Method26(); break;
                case 27: Method27(); break;
                case 28: Method28(); break;
                case 29: Method29(); break;
                case 30: Method30(); break;
                case 31: Method31(); break;
                case 32: Method32(); break;
                case 33: Method33(); break;
                case 34: CustomEntryMethod34(); break;
                case 35: Method35(); break;
                case 36: Method36(); break;
                case 37: Method37(); break;
                case 38: Method38(); break;
                case 39: Method39(); break;
                case 40: Method40(); break;
                case 41: Method41(); break;
                case 42: Method42(); break;
                case 43: Method43(); break;
                case 44: Method44(); break;
                case 45: Method45(); break;
                case 46: Method46(); break;
                case 47: Method47(); break;
                case 48: Method48(); break;
                case 49: Method49(); break;
                case 50: Method50(); break;
                case 51: Method51(); break;
                case 52: Method52(); break;
                case 53: Method53(); break;
                case 54: Method54(); break;
                case 55: Method55(); break;
                case 56: Method56(); break;
                case 57: Method57(); break;
                case 58: Method58(); break;
                case 59: Method59(); break;
                case 60: Method60(); break;
                case 61: Method61(); break;
                case 62: Method62(); break;
                case 63: Method63(); break;
                case 64: Method64(); break;
                case 65: Method65(); break;
                case 66: Method66(); break;
                case 67: Method67(); break;
                case 68: Method68(); break;
                case 69: Method69(); break;
                case 70: Method70(); break;
                case 71: Method71(); break;
                case 72: Method72(); break;
                case 73: Method73(); break;
                case 74: Method74(); break;
                case 75: Method75(); break;
                case 76: Method76(); break;
                case 77: Method77(); break;
                case 78: Method78(); break;
                case 79: Method79(); break;
                case 80: Method80(); break;
                case 81: Method81(); break;
                case 82: Method82(); break;
                case 83: Method83(); break;
                case 84: Method84(); break;
                case 85: Method85(); break;
                case 86: Method86(); break;
                case 87: Method87(); break;
                case 88: Method88(); break;
                case 89: Method89(); break;
                case 90: Method90(); break;
                case 91: Method91(); break;
                case 92: Method92(); break;
                case 93: Method93(); break;
                case 94: Method94(); break;
                case 95: Method95(); break;
                case 96: Method96(); break;
                case 97: Method97(); break;
                case 98: Method98(); break;
                case 99: Method99(); break;
                case 100: Method100(); break;
                case 101: Method101(); break;
                case 102: Method102(); break;
                case 103: Method103(); break;
                case 104: Method104(); break;
                case 105: Method105(); break;
                case 106: Method106(); break;
                case 107: CustomEntryMethod107(); break;
                case 108: Method108(); break;
                case 109: Method109(); break;
                case 110: Method110(); break;
                case 111: Method111(); break;
                case 112: Method112(); break;
                case 113: Method113(); break;
                case 114: Method114(); break;
                case 115: Method115(); break;
                case 116: Method116(); break;
                case 117: Method117(); break;
                case 118: Method118(); break;
                case 119: Method119(); break;
                case 120: Method120(); break;
                case 121: Method121(); break;
                case 122: Method122(); break;
                case 123: Method123(); break;
                case 124: Method124(); break;
                case 125: Method125(); break;
                case 126: Method126(); break;
                case 127: Method127(); break;
                case 128: Method128(); break;
                case 129: Method129(); break;
                case 130: Method130(); break;
                case 131: Method131(); break;
                case 132: Method132(); break;
                case 133: Method133(); break;
                case 134: Method134(); break;
                case 135: Method135(); break;
                case 136: Method136(); break;
                case 137: Method137(); break;
                case 138: Method138(); break;
                case 139: Method139(); break;
                case 140: Method140(); break;
                case 141: Method141(); break;
                case 142: Method142(); break;
                case 143: Method143(); break;
                case 144: Method144(); break;
                case 145: Method145(); break;
                case 146: Method146(); break;
                case 147: Method147(); break;
                case 148: Method148(); break;
                case 149: Method149(); break;
                case 150: Method150(); break;
                case 151: Method151(); break;
                case 152: Method152(); break;
                case 153: Method153(); break;
                case 154: Method154(); break;
                case 155: Method155(); break;
                case 156: Method156(); break;
                case 157: Method157(); break;
                case 158: Method158(); break;
                case 159: Method159(); break;
                case 160: Method160(); break;
                case 161: Method161(); break;
                case 162: Method162(); break;
                case 163: Method163(); break;
                case 164: Method164(); break;
                case 165: Method165(); break;
                case 166: Method166(); break;
                case 167: Method167(); break;
                case 168: Method168(); break;
                case 169: Method169(); break;
                case 170: Method170(); break;
                case 171: Method171(); break;
                case 172: Method172(); break;
                case 173: Method173(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}