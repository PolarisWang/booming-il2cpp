// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/nodes
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.Json.Serialization.Metadata;
using System.Threading;
using System.Threading.Tasks;
using Chaos.TestFramework;

public static partial class NodesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Nodes.JsonArray::Add{T}:System.Void(T)
    public static void CustomEntrySubject_0()
    {
        try { var arr = new JsonArray(); arr.Add(42); Assert.AreEqual(1, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_1()
    {
        try { var arr = new JsonArray(); arr.Add((JsonNode)JsonValue.Create(42)); Assert.AreEqual(1, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Nodes.JsonArray::Clear:System.Void()
    public static void CustomEntrySubject_2()
    {
        try { var arr = new JsonArray((JsonNode)42); arr.Clear(); Assert.AreEqual(0, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Nodes.JsonArray::Contains:System.Boolean(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_3()
    {
        try { var arr = new JsonArray((JsonNode)42); Assert.IsTrue(arr.Contains(arr[0]!)); }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Nodes.JsonArray::Create:System.Text.Json.Nodes.JsonArray(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_4()
    {
        try { var doc = JsonDocument.Parse("[42]"); var arr = JsonArray.Create(doc.RootElement); Assert.IsNotNull(arr); }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Count:System.Int32()
    public static void CustomEntrySubject_5()
    {
        try { var arr = new JsonArray((JsonNode)42, (JsonNode)43); Assert.AreEqual(2, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Nodes.JsonArray::GetEnumerator:System.Collections.Generic.IEnumerator{System.Text.Json.Nodes.JsonNode}()
    public static void CustomEntrySubject_6()
    {
        try { var arr = new JsonArray((JsonNode)42); var e = arr.GetEnumerator(); Assert.IsTrue(e.MoveNext()); }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues{T}:System.Collections.Generic.IEnumerable{T}()
    public static void CustomEntrySubject_7()
    {
        try { var arr = new JsonArray((JsonNode)42); var vals = arr.GetValues<int>(); Assert.AreEqual(42, vals.First()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Text.Json/System.Text.Json.Nodes.JsonArray::IndexOf:System.Int32(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_8()
    {
        try { var arr = new JsonArray((JsonNode)42); Assert.AreEqual(0, arr.IndexOf(arr[0]!)); }
        catch { _exitCode = 1; }
    }

    // [9] System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_9()
    {
        try { var arr = new JsonArray(); arr.Insert(0, (JsonNode)42); Assert.AreEqual(1, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [10] System.Text.Json/System.Text.Json.Nodes.JsonArray::Remove:System.Boolean(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_10()
    {
        try { var arr = new JsonArray((JsonNode)42); Assert.IsTrue(arr.Remove(arr[0]!)); }
        catch { _exitCode = 1; }
    }

    // [11] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAll:System.Int32(System.Func{System.Text.Json.Nodes.JsonNode,System.Boolean})
    public static void CustomEntrySubject_11()
    {
        try { var arr = new JsonArray((JsonNode)42, (JsonNode)43); Assert.AreEqual(2, arr.RemoveAll(n => (int)n! > 0)); }
        catch { _exitCode = 1; }
    }

    // [12] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)
    public static void CustomEntrySubject_12()
    {
        try { var arr = new JsonArray((JsonNode)42); arr.RemoveAt(0); Assert.AreEqual(0, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [13] System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveRange:System.Void(System.Int32,System.Int32)
    public static void CustomEntrySubject_13()
    {
        try { var arr = new JsonArray((JsonNode)0, (JsonNode)1, (JsonNode)2); arr.RemoveRange(0, 2); Assert.AreEqual(1, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [14] System.Text.Json/System.Text.Json.Nodes.JsonArray::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_14()
    {
        try { var arr = new JsonArray((JsonNode)42); using var ms = new MemoryStream(); using var w = new Utf8JsonWriter(ms); arr.WriteTo(w); w.Flush(); Assert.IsTrue(ms.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [15] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_15()
    {
        try { var arr = new JsonArray(default(JsonNodeOptions?)); Assert.IsNotNull(arr); }
        catch { _exitCode = 1; }
    }

    // [16] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNodeOptions,System.Text.Json.Nodes.JsonNode{})
    public static void CustomEntrySubject_16()
    {
        try { var arr = new JsonArray(default(JsonNodeOptions), new JsonNode?[] { (JsonNode)42 }); Assert.AreEqual(1, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [17] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNodeOptions,System.ReadOnlySpan{System.Text.Json.Nodes.JsonNode})
    public static void CustomEntrySubject_17()
    {
        try { ReadOnlySpan<JsonNode?> span = [(JsonNode)42]; var arr = new JsonArray(default(JsonNodeOptions), span); Assert.AreEqual(1, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [18] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.Text.Json.Nodes.JsonNode{})
    public static void CustomEntrySubject_18()
    {
        try { var arr = new JsonArray(new JsonNode[] { (JsonNode)42, (JsonNode)43 }); Assert.AreEqual(2, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [19] System.Text.Json/System.Text.Json.Nodes.JsonArray::.ctor:System.Void(System.ReadOnlySpan{System.Text.Json.Nodes.JsonNode})
    public static void CustomEntrySubject_19()
    {
        try { ReadOnlySpan<JsonNode> span = [(JsonNode)42]; var arr = new JsonArray(span); Assert.AreEqual(1, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [20] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsArray:System.Text.Json.Nodes.JsonArray()
    public static void CustomEntrySubject_20()
    {
        try { var arr = JsonNode.Parse("[42]")!.AsArray(); Assert.AreEqual(1, arr.Count); }
        catch { _exitCode = 1; }
    }

    // [21] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsObject:System.Text.Json.Nodes.JsonObject()
    public static void CustomEntrySubject_21()
    {
        try { var obj = JsonNode.Parse("{\"x\":42}")!.AsObject(); Assert.AreEqual(1, obj.Count); }
        catch { _exitCode = 1; }
    }

    // [22] System.Text.Json/System.Text.Json.Nodes.JsonNode::AsValue:System.Text.Json.Nodes.JsonValue()
    public static void CustomEntrySubject_22()
    {
        try { var v = JsonValue.Create(42)!.AsValue(); Assert.AreEqual(42, v.GetValue<int>()); }
        catch { _exitCode = 1; }
    }

    // [23] System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepClone:System.Text.Json.Nodes.JsonNode()
    public static void CustomEntrySubject_23()
    {
        try { var node = JsonNode.Parse("42")!; var clone = node.DeepClone(); Assert.AreEqual(42, (int)clone!); }
        catch { _exitCode = 1; }
    }

    // [24] System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_24()
    {
        try { Assert.IsTrue(JsonNode.DeepEquals(JsonNode.Parse("42")!, JsonNode.Parse("42")!)); }
        catch { _exitCode = 1; }
    }

    // [25] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode(System.Int32)
    public static void CustomEntrySubject_25()
    {
        try { var n = JsonNode.Parse("[42]")![0]; Assert.AreEqual(42, (int)n!); }
        catch { _exitCode = 1; }
    }

    // [26] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode(System.String)
    public static void CustomEntrySubject_26()
    {
        try { var n = JsonNode.Parse("{\"x\":42}")!["x"]; Assert.AreEqual(42, (int)n!); }
        catch { _exitCode = 1; }
    }

    // [27] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Options:System.Nullable{System.Text.Json.Nodes.JsonNodeOptions}()
    public static void CustomEntrySubject_27()
    {
        try { var o = JsonNode.Parse("42")!.Options; Assert.IsNotNull(o); }
        catch { _exitCode = 1; }
    }

    // [28] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Parent:System.Text.Json.Nodes.JsonNode()
    public static void CustomEntrySubject_28()
    {
        try { var arr = new JsonArray((JsonNode)42); var p = arr[0]!.Parent; Assert.IsNotNull(p); }
        catch { _exitCode = 1; }
    }

    // [29] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Root:System.Text.Json.Nodes.JsonNode()
    public static void CustomEntrySubject_29()
    {
        try { var arr = new JsonArray((JsonNode)42); var r = arr[0]!.Root; Assert.IsNotNull(r); }
        catch { _exitCode = 1; }
    }

    // [30] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetElementIndex:System.Int32()
    public static void CustomEntrySubject_30()
    {
        try { var arr = new JsonArray((JsonNode)42, (JsonNode)43); Assert.AreEqual(1, arr[1]!.GetElementIndex()); }
        catch { _exitCode = 1; }
    }

    // [31] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPath:System.String()
    public static void CustomEntrySubject_31()
    {
        try { var p = JsonNode.Parse("{\"x\":42}")!["x"]!.GetPath(); Assert.IsTrue(p.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [32] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPropertyName:System.String()
    public static void CustomEntrySubject_32()
    {
        try { var n = JsonNode.Parse("{\"x\":42}")!["x"]!; Assert.AreEqual("x", n.GetPropertyName()); }
        catch { _exitCode = 1; }
    }

    // [33] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue{T}:T()
    public static void CustomEntrySubject_33()
    {
        try { var v = JsonValue.Create(42)!.GetValue<int>(); Assert.AreEqual(42, v); }
        catch { _exitCode = 1; }
    }

    // [34] System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValueKind:System.Text.Json.JsonValueKind()
    public static void CustomEntrySubject_34()
    {
        try { var k = JsonNode.Parse("42")!.GetValueKind(); Assert.AreEqual((int)JsonValueKind.Number, (int)k); }
        catch { _exitCode = 1; }
    }

    // [35] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Boolean(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_35()
    {
        try { var v = (bool)JsonValue.Create(true)!; Assert.IsTrue(v); }
        catch { _exitCode = 1; }
    }

    // [36] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Boolean}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_36()
    {
        try { var v = (bool?)JsonValue.Create(true)!; Assert.IsTrue(v!.Value); }
        catch { _exitCode = 1; }
    }

    // [37] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Byte(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_37()
    {
        try { var v = (byte)JsonValue.Create((byte)42)!; Assert.AreEqual((byte)42, v); }
        catch { _exitCode = 1; }
    }

    // [38] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Byte}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_38()
    {
        try { var v = (byte?)JsonValue.Create((byte)42)!; Assert.AreEqual((byte)42, (byte)v!); }
        catch { _exitCode = 1; }
    }

    // [39] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Char(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_39()
    {
        try { var v = (char)JsonValue.Create('a')!; Assert.AreEqual('a', v); }
        catch { _exitCode = 1; }
    }

    // [40] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Char}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_40()
    {
        try { var v = (char?)JsonValue.Create('a')!; Assert.AreEqual('a', (char)v!); }
        catch { _exitCode = 1; }
    }

    // [41] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.DateTime(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_41()
    {
        try { var v = (DateTime)JsonValue.Create(new DateTime(2024, 1, 1))!; Assert.AreEqual(2024, v.Year); }
        catch { _exitCode = 1; }
    }

    // [42] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.DateTime}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_42()
    {
        try { var v = (DateTime?)JsonValue.Create(new DateTime(2024, 1, 1))!; Assert.AreEqual(2024, v!.Value.Year); }
        catch { _exitCode = 1; }
    }

    // [43] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.DateTimeOffset(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_43()
    {
        try { var v = (DateTimeOffset)JsonValue.Create(new DateTimeOffset(2024, 1, 1, 0, 0, 0, TimeSpan.Zero))!; Assert.AreEqual(2024, v.Year); }
        catch { _exitCode = 1; }
    }

    // [44] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.DateTimeOffset}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_44()
    {
        try { var v = (DateTimeOffset?)JsonValue.Create(new DateTimeOffset(2024, 1, 1, 0, 0, 0, TimeSpan.Zero))!; Assert.AreEqual(2024, v!.Value.Year); }
        catch { _exitCode = 1; }
    }

    // [45] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Decimal(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_45()
    {
        try { var v = (decimal)JsonValue.Create(42m)!; Assert.AreEqual(42m, v); }
        catch { _exitCode = 1; }
    }

    // [46] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Decimal}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_46()
    {
        try { var v = (decimal?)JsonValue.Create(42m)!; Assert.AreEqual(42m, (decimal)v!); }
        catch { _exitCode = 1; }
    }

    // [47] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Double(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_47()
    {
        try { var v = (double)JsonValue.Create(3.14)!; Assert.AreEqual(3.14, v); }
        catch { _exitCode = 1; }
    }

    // [48] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Double}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_48()
    {
        try { var v = (double?)JsonValue.Create(3.14)!; Assert.AreEqual(3.14, v!.Value); }
        catch { _exitCode = 1; }
    }

    // [49] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Guid(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_49()
    {
        try { var g = new Guid("00000001-0002-0003-0004-000000000005"); var v = (Guid)JsonValue.Create(g)!; Assert.IsTrue(g.Equals(v)); }
        catch { _exitCode = 1; }
    }

    // [50] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Guid}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_50()
    {
        try { var g = new Guid("00000001-0002-0003-0004-000000000005"); var v = (Guid?)JsonValue.Create(g)!; Assert.IsTrue(g.Equals((Guid)v!)); }
        catch { _exitCode = 1; }
    }

    // [51] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int16(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_51()
    {
        try { var v = (short)JsonValue.Create((short)42)!; Assert.AreEqual((short)42, v); }
        catch { _exitCode = 1; }
    }

    // [52] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int16}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_52()
    {
        try { var v = (short?)JsonValue.Create((short)42)!; Assert.AreEqual((short)42, (short)v!); }
        catch { _exitCode = 1; }
    }

    // [53] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int32(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_53()
    {
        try { var v = (int)JsonValue.Create(42)!; Assert.AreEqual(42, v); }
        catch { _exitCode = 1; }
    }

    // [54] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int32}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_54()
    {
        try { var v = (int?)JsonValue.Create(42)!; Assert.AreEqual(42, (int)v!); }
        catch { _exitCode = 1; }
    }

    // [55] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Int64(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_55()
    {
        try { var v = (long)JsonValue.Create(42L)!; Assert.AreEqual(42L, v); }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Int64}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_56()
    {
        try { var v = (long?)JsonValue.Create(42L)!; Assert.AreEqual(42L, (long)v!); }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.SByte(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_57()
    {
        try { var v = (sbyte)JsonValue.Create((sbyte)42)!; Assert.AreEqual((sbyte)42, v); }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.SByte}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_58()
    {
        try { var v = (sbyte?)JsonValue.Create((sbyte)42)!; Assert.AreEqual((sbyte)42, (sbyte)v!); }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Single(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_59()
    {
        try { var v = (float)JsonValue.Create(3.14f)!; Assert.AreEqual(3.14f, v); }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.Single}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_60()
    {
        try { var v = (float?)JsonValue.Create(3.14f)!; Assert.AreEqual(3.14f, (float)v!); }
        catch { _exitCode = 1; }
    }

    // [61] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.String(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_61()
    {
        try { var v = (string)JsonValue.Create("hello")!; Assert.AreEqual("hello", v); }
        catch { _exitCode = 1; }
    }

    // [62] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt16(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_62()
    {
        try { var v = (ushort)JsonValue.Create((ushort)42)!; Assert.AreEqual((ushort)42, v); }
        catch { _exitCode = 1; }
    }

    // [63] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt16}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_63()
    {
        try { var v = (ushort?)JsonValue.Create((ushort)42)!; Assert.AreEqual((ushort)42, (ushort)v!); }
        catch { _exitCode = 1; }
    }

    // [64] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt32(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_64()
    {
        try { var v = (uint)JsonValue.Create(42u)!; Assert.AreEqual(42u, v); }
        catch { _exitCode = 1; }
    }

    // [65] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt32}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_65()
    {
        try { var v = (uint?)JsonValue.Create(42u)!; Assert.AreEqual((int)42u, (int)v!); }
        catch { _exitCode = 1; }
    }

    // [66] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.UInt64(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_66()
    {
        try { var v = (ulong)JsonValue.Create(42UL)!; Assert.AreEqual(42UL, v); }
        catch { _exitCode = 1; }
    }

    // [67] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Explicit:System.Nullable{System.UInt64}(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_67()
    {
        try { var v = (ulong?)JsonValue.Create(42UL)!; Assert.AreEqual((long)42UL, (long)v!); }
        catch { _exitCode = 1; }
    }

    // [68] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Boolean)
    public static void CustomEntrySubject_68()
    {
        try { JsonNode node = true; Assert.IsTrue((bool)node); }
        catch { _exitCode = 1; }
    }

    // [69] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Boolean})
    public static void CustomEntrySubject_69()
    {
        try { JsonNode node = (bool?)true; Assert.IsTrue((bool)node); }
        catch { _exitCode = 1; }
    }

    // [70] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Byte)
    public static void CustomEntrySubject_70()
    {
        try { JsonNode node = (byte)42; Assert.AreEqual((byte)42, (byte)node); }
        catch { _exitCode = 1; }
    }

    // [71] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Byte})
    public static void CustomEntrySubject_71()
    {
        try { JsonNode node = (byte?)42; Assert.AreEqual((byte)42, (byte)node); }
        catch { _exitCode = 1; }
    }

    // [72] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Char)
    public static void CustomEntrySubject_72()
    {
        try { JsonNode node = 'a'; Assert.AreEqual('a', (char)node); }
        catch { _exitCode = 1; }
    }

    // [73] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Char})
    public static void CustomEntrySubject_73()
    {
        try { JsonNode node = (char?)'a'; Assert.AreEqual('a', (char)node); }
        catch { _exitCode = 1; }
    }

    // [74] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.DateTime)
    public static void CustomEntrySubject_74()
    {
        try { JsonNode node = new DateTime(2024, 1, 1); Assert.AreEqual(2024, ((DateTime)node).Year); }
        catch { _exitCode = 1; }
    }

    // [75] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.DateTime})
    public static void CustomEntrySubject_75()
    {
        try { JsonNode node = (DateTime?)new DateTime(2024, 1, 1); Assert.AreEqual(2024, ((DateTime)node).Year); }
        catch { _exitCode = 1; }
    }

    // [76] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.DateTimeOffset)
    public static void CustomEntrySubject_76()
    {
        try { JsonNode node = new DateTimeOffset(2024, 1, 1, 0, 0, 0, TimeSpan.Zero); Assert.AreEqual(2024, ((DateTimeOffset)node).Year); }
        catch { _exitCode = 1; }
    }

    // [77] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.DateTimeOffset})
    public static void CustomEntrySubject_77()
    {
        try { JsonNode node = (DateTimeOffset?)new DateTimeOffset(2024, 1, 1, 0, 0, 0, TimeSpan.Zero); Assert.AreEqual(2024, ((DateTimeOffset)node).Year); }
        catch { _exitCode = 1; }
    }

    // [78] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Decimal)
    public static void CustomEntrySubject_78()
    {
        try { JsonNode node = 42m; Assert.AreEqual(42m, (decimal)node); }
        catch { _exitCode = 1; }
    }

    // [79] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Decimal})
    public static void CustomEntrySubject_79()
    {
        try { JsonNode node = (decimal?)42m; Assert.AreEqual(42m, (decimal)node); }
        catch { _exitCode = 1; }
    }

    // [80] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Double)
    public static void CustomEntrySubject_80()
    {
        try { JsonNode node = 3.14; Assert.AreEqual(3.14, (double)node); }
        catch { _exitCode = 1; }
    }

    // [81] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Double})
    public static void CustomEntrySubject_81()
    {
        try { JsonNode node = (double?)3.14; Assert.AreEqual(3.14, (double)node); }
        catch { _exitCode = 1; }
    }

    // [82] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Guid)
    public static void CustomEntrySubject_82()
    {
        try { var g = new Guid("00000001-0002-0003-0004-000000000005"); JsonNode node = g; Assert.IsTrue(g.Equals((Guid)node)); }
        catch { _exitCode = 1; }
    }

    // [84] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int16)
    public static void CustomEntrySubject_84()
    {
        try { JsonNode node = (short)42; Assert.AreEqual((short)42, (short)node); }
        catch { _exitCode = 1; }
    }

    // [85] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int16})
    public static void CustomEntrySubject_85()
    {
        try { JsonNode node = (short?)42; Assert.AreEqual((short)42, (short)node); }
        catch { _exitCode = 1; }
    }

    // [86] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int32)
    public static void CustomEntrySubject_86()
    {
        try { JsonNode node = 42; Assert.AreEqual(42, (int)node); }
        catch { _exitCode = 1; }
    }

    // [87] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int32})
    public static void CustomEntrySubject_87()
    {
        try { JsonNode node = (int?)42; Assert.AreEqual(42, (int)node); }
        catch { _exitCode = 1; }
    }

    // [88] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Int64)
    public static void CustomEntrySubject_88()
    {
        try { JsonNode node = 42L; Assert.AreEqual(42L, (long)node); }
        catch { _exitCode = 1; }
    }

    // [89] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Int64})
    public static void CustomEntrySubject_89()
    {
        try { JsonNode node = (long?)42L; Assert.AreEqual(42L, (long)node); }
        catch { _exitCode = 1; }
    }

    // [90] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.SByte)
    public static void CustomEntrySubject_90()
    {
        try { JsonNode node = (sbyte)42; Assert.AreEqual((sbyte)42, (sbyte)node); }
        catch { _exitCode = 1; }
    }

    // [91] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.SByte})
    public static void CustomEntrySubject_91()
    {
        try { JsonNode node = (sbyte?)42; Assert.AreEqual((sbyte)42, (sbyte)node); }
        catch { _exitCode = 1; }
    }

    // [92] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Single)
    public static void CustomEntrySubject_92()
    {
        try { JsonNode node = 3.14f; Assert.AreEqual(3.14f, (float)node); }
        catch { _exitCode = 1; }
    }

    // [93] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.Single})
    public static void CustomEntrySubject_93()
    {
        try { JsonNode node = (float?)3.14f; Assert.AreEqual(3.14f, (float)node); }
        catch { _exitCode = 1; }
    }

    // [94] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.String)
    public static void CustomEntrySubject_94()
    {
        try { JsonNode node = "hello"; Assert.AreEqual("hello", (string)node); }
        catch { _exitCode = 1; }
    }

    // [95] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt16)
    public static void CustomEntrySubject_95()
    {
        try { JsonNode node = (ushort)42; Assert.AreEqual((ushort)42, (ushort)node); }
        catch { _exitCode = 1; }
    }

    // [96] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt16})
    public static void CustomEntrySubject_96()
    {
        try { JsonNode node = (ushort?)42; Assert.AreEqual((ushort)42, (ushort)node); }
        catch { _exitCode = 1; }
    }

    // [97] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt32)
    public static void CustomEntrySubject_97()
    {
        try { JsonNode node = 42u; Assert.AreEqual(42u, (uint)node); }
        catch { _exitCode = 1; }
    }

    // [98] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt32})
    public static void CustomEntrySubject_98()
    {
        try { JsonNode node = (uint?)42u; Assert.AreEqual(42u, (uint)node); }
        catch { _exitCode = 1; }
    }

    // [99] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.UInt64)
    public static void CustomEntrySubject_99()
    {
        try { JsonNode node = 42UL; Assert.AreEqual(42UL, (ulong)node); }
        catch { _exitCode = 1; }
    }

    // [100] System.Text.Json/System.Text.Json.Nodes.JsonNode::op_Implicit:System.Text.Json.Nodes.JsonNode(System.Nullable{System.UInt64})
    public static void CustomEntrySubject_100()
    {
        try { JsonNode node = (ulong?)42UL; Assert.AreEqual(42UL, (ulong)node); }
        catch { _exitCode = 1; }
    }

    // [101] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_101()
    {
        try { var bytes = Encoding.UTF8.GetBytes("42"); var reader = new Utf8JsonReader(bytes); var node = JsonNode.Parse(ref reader, default(JsonNodeOptions?)); Assert.IsNotNull(node); }
        catch { _exitCode = 1; }
    }

    // [102] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_102()
    {
        try { var node = JsonNode.Parse("42"); Assert.IsNotNull(node); }
        catch { _exitCode = 1; }
    }

    // [103] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.ReadOnlySpan{System.Byte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_103()
    {
        try { var node = JsonNode.Parse(Encoding.UTF8.GetBytes("42").AsSpan()); Assert.IsNotNull(node); }
        catch { _exitCode = 1; }
    }

    // [104] System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions)
    public static void CustomEntrySubject_104()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes("42")); var node = JsonNode.Parse(ms); Assert.IsNotNull(node); }
        catch { _exitCode = 1; }
    }

    // [105] System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task{System.Text.Json.Nodes.JsonNode}(System.IO.Stream,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions},System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken)
    public static void CustomEntrySubject_105()
    {
        try { using var ms = new MemoryStream(Encoding.UTF8.GetBytes("42")); var task = JsonNode.ParseAsync(ms); Assert.IsTrue(task.Status == TaskStatus.RanToCompletion || task.Status == TaskStatus.WaitingForActivation); }
        catch { _exitCode = 1; }
    }

    // [106] System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith{T}:System.Void(T)
    public static void CustomEntrySubject_106()
    {
        try { var node = JsonValue.Create(42)!; node.ReplaceWith(43); Assert.AreEqual(43, (int)node); }
        catch { _exitCode = 1; }
    }

    // [107] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_107()
    {
        try { var arr = new JsonArray((JsonNode)0); arr[0] = (JsonNode)42; Assert.AreEqual(42, (int)arr[0]!); }
        catch { _exitCode = 1; }
    }

    // [108] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.String,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_108()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; Assert.AreEqual(42, (int)obj["x"]!); }
        catch { _exitCode = 1; }
    }

    // [109] System.Text.Json/System.Text.Json.Nodes.JsonNode::ToJsonString:System.String(System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_109()
    {
        try { var node = JsonNode.Parse("42")!; var s = node.ToJsonString(); Assert.AreEqual("42", s); }
        catch { _exitCode = 1; }
    }

    // [110] System.Text.Json/System.Text.Json.Nodes.JsonNode::ToString:System.String()
    public static void CustomEntrySubject_110()
    {
        try { var node = JsonNode.Parse("42")!; var s = node.ToString(); Assert.IsTrue(s.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [111] System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_111()
    {
        try { var node = JsonNode.Parse("42")!; using var ms = new MemoryStream(); using var w = new Utf8JsonWriter(ms); node.WriteTo(w); w.Flush(); Assert.IsTrue(ms.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [112] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Parent:System.Void(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_112()
    {
        try { var node = JsonValue.Create(42)!; Assert.IsNull(node.Parent); }
        catch { _exitCode = 1; }
    }

    // [113] System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Item:System.Text.Json.Nodes.JsonNode()
    public static void CustomEntrySubject_113()
    {
        try { var node = JsonNode.Parse("42")!; Assert.AreEqual(42, (int)node); }
        catch { _exitCode = 1; }
    }

    // [114] System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Item:System.Void(System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_114()
    {
        try { var node = JsonValue.Create(42)!; Assert.AreEqual(42, (int)node); }
        catch { _exitCode = 1; }
    }

    // [115] System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_115()
    {
        try { var obj = new JsonObject(); obj.Add("x", (JsonNode)42); Assert.AreEqual(42, (int)obj["x"]!); }
        catch { _exitCode = 1; }
    }

    // [116] System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode})
    public static void CustomEntrySubject_116()
    {
        try { var obj = new JsonObject(); obj.Add(new KeyValuePair<string, JsonNode?>("x", (JsonNode)42)); Assert.AreEqual(42, (int)obj["x"]!); }
        catch { _exitCode = 1; }
    }

    // [117] System.Text.Json/System.Text.Json.Nodes.JsonObject::Clear:System.Void()
    public static void CustomEntrySubject_117()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; obj.Clear(); Assert.AreEqual(0, obj.Count); }
        catch { _exitCode = 1; }
    }

    // [118] System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)
    public static void CustomEntrySubject_118()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; Assert.IsTrue(obj.ContainsKey("x")); }
        catch { _exitCode = 1; }
    }

    // [119] System.Text.Json/System.Text.Json.Nodes.JsonObject::Create:System.Text.Json.Nodes.JsonObject(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_119()
    {
        try { var doc = JsonDocument.Parse("{\"x\":42}"); var obj = JsonObject.Create(doc.RootElement); Assert.IsNotNull(obj); }
        catch { _exitCode = 1; }
    }

    // [120] System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Count:System.Int32()
    public static void CustomEntrySubject_120()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; Assert.AreEqual(1, obj.Count); }
        catch { _exitCode = 1; }
    }

    // [121] System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}(System.Int32)
    public static void CustomEntrySubject_121()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; var kv = obj.GetAt(0); Assert.AreEqual("x", kv.Key); }
        catch { _exitCode = 1; }
    }

    // [122] System.Text.Json/System.Text.Json.Nodes.JsonObject::GetEnumerator:System.Collections.Generic.IEnumerator{System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}}()
    public static void CustomEntrySubject_122()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; var e = obj.GetEnumerator(); Assert.IsTrue(e.MoveNext()); }
        catch { _exitCode = 1; }
    }

    // [123] System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)
    public static void CustomEntrySubject_123()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; Assert.AreEqual(0, obj.IndexOf("x")); }
        catch { _exitCode = 1; }
    }

    // [124] System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_124()
    {
        try { var obj = new JsonObject(); obj.Insert(0, "x", (JsonNode)42); Assert.AreEqual(42, (int)obj["x"]!); }
        catch { _exitCode = 1; }
    }

    // [125] System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)
    public static void CustomEntrySubject_125()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; Assert.IsTrue(obj.Remove("x")); }
        catch { _exitCode = 1; }
    }

    // [126] System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)
    public static void CustomEntrySubject_126()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; obj.RemoveAt(0); Assert.AreEqual(0, obj.Count); }
        catch { _exitCode = 1; }
    }

    // [127] System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_127()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)0; obj.SetAt(0, "y", (JsonNode)42); Assert.AreEqual(42, (int)obj["y"]!); }
        catch { _exitCode = 1; }
    }

    // [128] System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_128()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)0; obj.SetAt(0, (JsonNode)42); Assert.AreEqual(42, (int)obj["x"]!); }
        catch { _exitCode = 1; }
    }

    // [129] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryAdd:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode)
    public static void CustomEntrySubject_129()
    {
        try { var obj = new JsonObject(); Assert.IsTrue(obj.TryAdd("x", (JsonNode)42)); }
        catch { _exitCode = 1; }
    }

    // [130] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryAdd:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode,System.Int32&)
    public static void CustomEntrySubject_130()
    {
        try { var obj = new JsonObject(); Assert.IsTrue(obj.TryAdd("x", (JsonNode)42, out int idx)); Assert.AreEqual(0, idx); }
        catch { _exitCode = 1; }
    }

    // [131] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode&)
    public static void CustomEntrySubject_131()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; Assert.IsTrue(obj.TryGetPropertyValue("x", out var n)); Assert.AreEqual(42, (int)n!); }
        catch { _exitCode = 1; }
    }

    // [132] System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode&,System.Int32&)
    public static void CustomEntrySubject_132()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; Assert.IsTrue(obj.TryGetPropertyValue("x", out var n2, out int idx2)); Assert.AreEqual(42, (int)n2!); }
        catch { _exitCode = 1; }
    }

    // [133] System.Text.Json/System.Text.Json.Nodes.JsonObject::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_133()
    {
        try { var obj = new JsonObject(); obj["x"] = (JsonNode)42; using var ms = new MemoryStream(); using var w = new Utf8JsonWriter(ms); obj.WriteTo(w); w.Flush(); Assert.IsTrue(ms.Length > 0); }
        catch { _exitCode = 1; }
    }

    // [134] System.Text.Json/System.Text.Json.Nodes.JsonObject::.ctor:System.Void(System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_134()
    {
        try { var obj = new JsonObject(default(JsonNodeOptions?)); Assert.IsNotNull(obj); }
        catch { _exitCode = 1; }
    }

    // [135] System.Text.Json/System.Text.Json.Nodes.JsonObject::.ctor:System.Void(System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{System.String,System.Text.Json.Nodes.JsonNode}},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_135()
    {
        try { var obj = new JsonObject(new Dictionary<string, JsonNode?> { ["x"] = (JsonNode)42 }); Assert.AreEqual(1, obj.Count); }
        catch { _exitCode = 1; }
    }

    // [136] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Boolean,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_136()
    {
        try { var v = JsonValue.Create(true); Assert.AreEqual(true, (bool)v!); }
        catch { _exitCode = 1; }
    }

    // [137] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Boolean},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_137()
    {
        try { var v = JsonValue.Create((bool?)true); Assert.AreEqual(true, (bool)v!); }
        catch { _exitCode = 1; }
    }

    // [138] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Byte,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_138()
    {
        try { var v = JsonValue.Create((byte)42); Assert.AreEqual((byte)42, (byte)v!); }
        catch { _exitCode = 1; }
    }

    // [139] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Byte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_139()
    {
        try { var v = JsonValue.Create((byte?)42); Assert.AreEqual((byte)42, (byte)v!); }
        catch { _exitCode = 1; }
    }

    // [140] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Char,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_140()
    {
        try { var v = JsonValue.Create('a'); Assert.AreEqual('a', (char)v!); }
        catch { _exitCode = 1; }
    }

    // [141] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Char},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_141()
    {
        try { var v = JsonValue.Create((char?)'a'); Assert.AreEqual('a', (char)v!); }
        catch { _exitCode = 1; }
    }

    // [142] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.DateTime,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_142()
    {
        try { var v = JsonValue.Create(new DateTime(2024, 1, 1)); Assert.AreEqual(2024, ((DateTime)v!).Year); }
        catch { _exitCode = 1; }
    }

    // [143] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.DateTime},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_143()
    {
        try { var v = JsonValue.Create((DateTime?)new DateTime(2024, 1, 1)); Assert.AreEqual(2024, ((DateTime)v!).Year); }
        catch { _exitCode = 1; }
    }

    // [144] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.DateTimeOffset,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_144()
    {
        try { var v = JsonValue.Create(new DateTimeOffset(2024, 1, 1, 0, 0, 0, TimeSpan.Zero)); Assert.AreEqual(2024, ((DateTimeOffset)v!).Year); }
        catch { _exitCode = 1; }
    }

    // [145] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.DateTimeOffset},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_145()
    {
        try { var v = JsonValue.Create((DateTimeOffset?)new DateTimeOffset(2024, 1, 1, 0, 0, 0, TimeSpan.Zero)); Assert.AreEqual(2024, ((DateTimeOffset)v!).Year); }
        catch { _exitCode = 1; }
    }

    // [146] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Decimal,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_146()
    {
        try { var v = JsonValue.Create(42m); Assert.AreEqual(42m, (decimal)v!); }
        catch { _exitCode = 1; }
    }

    // [147] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Decimal},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_147()
    {
        try { var v = JsonValue.Create((decimal?)42m); Assert.AreEqual(42m, (decimal)v!); }
        catch { _exitCode = 1; }
    }

    // [148] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Double,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_148()
    {
        try { var v = JsonValue.Create(3.14); Assert.AreEqual(3.14, (double)v!); }
        catch { _exitCode = 1; }
    }

    // [149] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Double},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_149()
    {
        try { var v = JsonValue.Create((double?)3.14); Assert.AreEqual(3.14, (double)v!); }
        catch { _exitCode = 1; }
    }

    // [150] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Guid,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_150()
    {
        try { var g = new Guid("00000001-0002-0003-0004-000000000005"); var v = JsonValue.Create(g); Assert.IsTrue(g.Equals((Guid)v!)); }
        catch { _exitCode = 1; }
    }

    // [151] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Guid},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_151()
    {
        try { var g = new Guid("00000001-0002-0003-0004-000000000005"); var v = JsonValue.Create((Guid?)g); Assert.IsTrue(g.Equals((Guid)v!)); }
        catch { _exitCode = 1; }
    }

    // [152] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int16,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_152()
    {
        try { var v = JsonValue.Create((short)42); Assert.AreEqual((short)42, (short)v!); }
        catch { _exitCode = 1; }
    }

    // [153] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int16},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_153()
    {
        try { var v = JsonValue.Create((short?)42); Assert.AreEqual((short)42, (short)v!); }
        catch { _exitCode = 1; }
    }

    // [154] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int32,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_154()
    {
        try { var v = JsonValue.Create(42); Assert.AreEqual(42, (int)v!); }
        catch { _exitCode = 1; }
    }

    // [155] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int32},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_155()
    {
        try { var v = JsonValue.Create((int?)42); Assert.AreEqual(42, (int)v!); }
        catch { _exitCode = 1; }
    }

    // [156] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Int64,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_156()
    {
        try { var v = JsonValue.Create(42L); Assert.AreEqual(42L, (long)v!); }
        catch { _exitCode = 1; }
    }

    // [157] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Int64},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_157()
    {
        try { var v = JsonValue.Create((long?)42L); Assert.AreEqual(42L, (long)v!); }
        catch { _exitCode = 1; }
    }

    // [158] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.SByte,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_158()
    {
        try { var v = JsonValue.Create((sbyte)42); Assert.AreEqual((sbyte)42, (sbyte)v!); }
        catch { _exitCode = 1; }
    }

    // [159] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.SByte},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_159()
    {
        try { var v = JsonValue.Create((sbyte?)42); Assert.AreEqual((sbyte)42, (sbyte)v!); }
        catch { _exitCode = 1; }
    }

    // [160] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Single,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_160()
    {
        try { var v = JsonValue.Create(3.14f); Assert.AreEqual(3.14f, (float)v!); }
        catch { _exitCode = 1; }
    }

    // [161] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Single},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_161()
    {
        try { var v = JsonValue.Create((float?)3.14f); Assert.AreEqual(3.14f, (float)v!); }
        catch { _exitCode = 1; }
    }

    // [162] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.String,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_162()
    {
        try { var v = JsonValue.Create("hello"); Assert.AreEqual("hello", (string)v!); }
        catch { _exitCode = 1; }
    }

    // [163] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt16,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_163()
    {
        try { var v = JsonValue.Create((ushort)42); Assert.AreEqual((ushort)42, (ushort)v!); }
        catch { _exitCode = 1; }
    }

    // [164] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt16},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_164()
    {
        try { var v = JsonValue.Create((ushort?)42); Assert.AreEqual((ushort)42, (ushort)v!); }
        catch { _exitCode = 1; }
    }

    // [165] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt32,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_165()
    {
        try { var v = JsonValue.Create(42u); Assert.AreEqual(42u, (uint)v!); }
        catch { _exitCode = 1; }
    }

    // [166] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt32},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_166()
    {
        try { var v = JsonValue.Create((uint?)42u); Assert.AreEqual(42u, (uint)v!); }
        catch { _exitCode = 1; }
    }

    // [167] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.UInt64,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_167()
    {
        try { var v = JsonValue.Create(42UL); Assert.AreEqual(42UL, (ulong)v!); }
        catch { _exitCode = 1; }
    }

    // [168] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.UInt64},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_168()
    {
        try { var v = JsonValue.Create((ulong?)42UL); Assert.AreEqual(42UL, (ulong)v!); }
        catch { _exitCode = 1; }
    }

    // [169] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Text.Json.JsonElement,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_169()
    {
        try { var doc = JsonDocument.Parse("42"); var v = JsonValue.Create(doc.RootElement); Assert.IsNotNull(v); }
        catch { _exitCode = 1; }
    }

    // [170] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create:System.Text.Json.Nodes.JsonValue(System.Nullable{System.Text.Json.JsonElement},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_170()
    {
        try { var doc = JsonDocument.Parse("42"); var v = JsonValue.Create((JsonElement?)doc.RootElement); Assert.IsNotNull(v); }
        catch { _exitCode = 1; }
    }

    // [171] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_171()
    {
        try { var v = JsonValue.Create<int>(42); Assert.AreEqual(42, v!.GetValue<int>()); }
        catch { _exitCode = 1; }
    }

    // [172] System.Text.Json/System.Text.Json.Nodes.JsonValue::Create{T}:System.Text.Json.Nodes.JsonValue(T,System.Text.Json.Serialization.Metadata.JsonTypeInfo{T},System.Nullable{System.Text.Json.Nodes.JsonNodeOptions})
    public static void CustomEntrySubject_172()
    {
        try { var v = JsonValue.Create<int>(42, null!); Assert.AreEqual(42, v!.GetValue<int>()); }
        catch { _exitCode = 1; }
    }

    // [173] System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue{T}:System.Boolean(T&)
    public static void CustomEntrySubject_173()
    {
        try { var v = JsonValue.Create(42); Assert.IsTrue(v!.TryGetValue<int>(out int r)); Assert.AreEqual(42, r); }
        catch { _exitCode = 1; }
    }

}
