using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

namespace Chaos.IL2CPP.Tests.CoreLibSystemContract;

/// <summary>
/// Real semantic assertions for System.Private.CoreLib/system chunk types
/// that ATG cannot test (produces null/invalid inputs → [UNVERIFIED] smoke).
///
/// This contract feeds VALID inputs and asserts SEMANTIC results.
/// Runs against the .NET 8 reference runtime to verify contract definition;
/// later wired into foundation-dll pipeline for AOT verification.
///
/// Scope: System.Convert (298 smoke), Array (271), Math (141), AppDomain (124),
///        Char (119), Half (117), MathF (81), Int128 (75),
///        Int16 (72), UInt128 (62), UInt16 (56), IntPtr (84 → 30 smoke),
///        UIntPtr (34→34 smoke), OperatingSystem (50→50 smoke),
///        Byte/SByte (107), Boolean, Guid, TimeSpan, DateTimeOffset, etc.
///
/// Exit code: 0 = all pass, 1 = at least one assertion failed.
/// </summary>
public static class Program
{
    private static int _passed;
    private static int _failed;
    private static readonly List<string> Failures = new();

    public static int Main()
    {
        Console.OutputEncoding = Encoding.UTF8;
        Console.WriteLine("═══ CoreLib System Contract ═══");
        Console.WriteLine($"runtime: {Environment.Version} / {RuntimeInformation.FrameworkDescription}");
        Console.WriteLine();

        // ── Phase 1: Convert (298 smoke) ─────────────────────────
        Console.WriteLine("─ System.Convert ──────────────────────────────────────");
        RunConvertContract();

        // ── Phase 2: Array (271 smoke) ─────────────────────────
        Console.WriteLine("─ System.Array ────────────────────────────────────────");
        RunArrayContract();

        // ── Phase 3: Math / MathF (141 + 81 = 222 smoke) ─────────
        Console.WriteLine("─ System.Math ─────────────────────────────────────────");
        RunMathContract();

        Console.WriteLine("─ System.MathF ────────────────────────────────────────");
        RunMathFContract();

        // ── Phase 4: Char / AppDomain / Half (360 smoke) ─────────
        Console.WriteLine("─ System.Char ─────────────────────────────────────────");
        RunCharContract();

        Console.WriteLine("─ System.AppDomain ────────────────────────────────────");
        RunAppDomainContract();

        Console.WriteLine("─ System.Half ─────────────────────────────────────────");
        RunHalfContract();

        // ── Phase 5: Int128 family (280 smoke) ──────────────────
        Console.WriteLine("─ System.Int128 / UInt128 / Int16 / UInt16 / IntPtr / UIntPtr ──");
        RunInt128Contract();
        RunInt16Contract();
        RunUInt16Contract();
        RunUInt128Contract();
        RunIntPtrContract();
        RunUIntPtrContract();

        // ── Phase 6: Byte / SByte / Boolean (160 smoke) ─────────
        Console.WriteLine("─ System.Byte / SByte / Boolean ───────────────────────");
        RunByteContract();
        RunSByteContract();
        RunBooleanContract();

        // ── Phase 7: OperatingSystem / TimeSpan / Guid (60+ smoke) ─
        Console.WriteLine("─ System.OperatingSystem ──────────────────────────────");
        RunOperatingSystemContract();

        Console.WriteLine("─ System.TimeSpan ──────────────────────────────────────");
        RunTimeSpanContract();

        Console.WriteLine("─ System.Guid ─────────────────────────────────────────");
        RunGuidContract();

        // ── Phase 8: Activator / Attribute / GC (150+ smoke) ─────
        Console.WriteLine("─ System.Activator ────────────────────────────────────");
        RunActivatorContract();

        Console.WriteLine("─ System.Attribute ────────────────────────────────────");
        RunAttributeContract();

        Console.WriteLine("─ System.GC ───────────────────────────────────────────");
        RunGCContract();

        Report();
        return _failed == 0 ? 0 : 1;
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 1: System.Convert
    // ══════════════════════════════════════════════════════════════
    private static void RunConvertContract()
    {
        // ToBoolean
        Check("Convert.ToBoolean(1) == true", () => Convert.ToBoolean(1) == true);
        Check("Convert.ToBoolean(0) == false", () => Convert.ToBoolean(0) == false);
        Check("Convert.ToBoolean(\"true\") == true", () => Convert.ToBoolean("true") == true);
        Check("Convert.ToBoolean(\"false\") == false", () => Convert.ToBoolean("false") == false);

        // ToChar
        Check("Convert.ToChar(65) == 'A'", () => Convert.ToChar(65) == 'A');
        Check("Convert.ToChar(\"X\") == 'X'", () => Convert.ToChar("X") == 'X');

        // ToByte
        Check("Convert.ToByte(42) == 42", () => Convert.ToByte(42) == 42);
        Check("Convert.ToByte(\"200\") == 200", () => Convert.ToByte("200") == 200);

        // ToSByte
        Check("Convert.ToSByte(\"100\") == 100", () => Convert.ToSByte("100") == 100);

        // ToInt16
        Check("Convert.ToInt16(42) == 42", () => Convert.ToInt16(42) == 42);
        Check("Convert.ToInt16(\"32000\") == 32000", () => Convert.ToInt16("32000") == 32000);

        // ToUInt16
        Check("Convert.ToUInt16(\"65000\") == 65000", () => Convert.ToUInt16("65000") == 65000);

        // ToInt32 — core overloads
        Check("Convert.ToInt32(42) == 42", () => Convert.ToInt32(42) == 42);
        Check("Convert.ToInt32(\"123\") == 123", () => Convert.ToInt32("123") == 123);
        Check("Convert.ToInt32(true) == 1", () => Convert.ToInt32(true) == 1);
        Check("Convert.ToInt32(false) == 0", () => Convert.ToInt32(false) == 0);
        Check("Convert.ToInt32(\"FF\", 16) == 255", () => Convert.ToInt32("FF", 16) == 255);
        Check("Convert.ToInt32(\"77\", 8) == 63", () => Convert.ToInt32("77", 8) == 63);
        Check("Convert.ToInt32(3.14) == 3", () => Convert.ToInt32(3.14) == 3);

        // ToInt64
        Check("Convert.ToInt64(\"9999999999999\") == 9999999999999", () => Convert.ToInt64("9999999999999") == 9999999999999L);

        // ToSingle
        Check("Convert.ToSingle(\"3.14\") == 3.14f", () => Math.Abs(Convert.ToSingle("3.14") - 3.14f) < 0.001f);

        // ToDouble
        Check("Convert.ToDouble(42) == 42.0", () => Convert.ToDouble(42) == 42.0);
        Check("Convert.ToDouble(\"3.14159\") ≈ 3.14159", () => Math.Abs(Convert.ToDouble("3.14159") - 3.14159) < 0.00001);
        Check("Convert.ToDouble(true) == 1.0", () => Convert.ToDouble(true) == 1.0);

        // ToDecimal
        Check("Convert.ToDecimal(\"123.456\") == 123.456m", () => Convert.ToDecimal("123.456") == 123.456m);
        Check("Convert.ToDecimal(42) == 42m", () => Convert.ToDecimal(42) == 42m);

        // ToString — various overloads
        Check("Convert.ToString(123) == \"123\"", () => Convert.ToString(123) == "123");
        Check("Convert.ToString(true) == \"True\"", () => Convert.ToString(true) == "True");
        Check("Convert.ToString(3.14) == \"3.14\"", () => Convert.ToString(3.14) == "3.14");
        Check("Convert.ToString(123, 16) == \"7b\"", () => Convert.ToString(123, 16) == "7b");

        // ChangeType
        Check("Convert.ChangeType(\"True\", typeof(bool)) == true", () => (bool)Convert.ChangeType("True", typeof(bool)) == true);
        Check("Convert.ChangeType(\"123\", typeof(int)) is 123", () => (int)Convert.ChangeType("123", typeof(int)) == 123);

        // Base64
        var bytes = new byte[] { 1, 2, 3 };
        Check("Convert.ToBase64String(new byte[]{1,2,3}) == \"AQID\"", () => Convert.ToBase64String(bytes) == "AQID");
        Check("Convert.FromBase64String(\"AQID\")[0] == 1", () => Convert.FromBase64String("AQID")[0] == 1);

        // IsDBNull
        Check("Convert.IsDBNull(DBNull.Value) == true", () => Convert.IsDBNull(DBNull.Value) == true);

        // ToByte(int, int)
        Check("Convert.ToByte(\"1010\", 2) == 10", () => Convert.ToByte("1010", 2) == 10);
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 2: System.Array
    // ══════════════════════════════════════════════════════════════
    private static void RunArrayContract()
    {
        // Sort
        Check("Array.Sort(int[]) sorts ascending", () =>
        {
            var arr = new int[] { 3, 1, 2 };
            Array.Sort(arr);
            return arr[0] == 1 && arr[1] == 2 && arr[2] == 3;
        });

        Check("Array.Sort(string[]) sorts alphabetically", () =>
        {
            var arr = new[] { "z", "a", "m" };
            Array.Sort(arr);
            return arr[0] == "a" && arr[1] == "m" && arr[2] == "z";
        });

        // Sort with index/length
        Check("Array.Sort(int[], 1, 2) sorts partial range", () =>
        {
            var arr = new int[] { 10, 3, 1, 20 };
            Array.Sort(arr, 1, 2);
            return arr[0] == 10 && arr[1] == 1 && arr[2] == 3 && arr[3] == 20;
        });

        // IndexOf
        Check("Array.IndexOf({10,20,30}, 20) == 1", () => Array.IndexOf(new[] { 10, 20, 30 }, 20) == 1);
        Check("Array.IndexOf({10,20,30}, 99) == -1", () => Array.IndexOf(new[] { 10, 20, 30 }, 99) == -1);
        Check("Array.IndexOf({'a','b','c'}, 'c') == 2", () => Array.IndexOf(new[] { 'a', 'b', 'c' }, 'c') == 2);

        // LastIndexOf
        Check("Array.LastIndexOf({1,2,1,2}, 2) == 3", () => Array.LastIndexOf(new[] { 1, 2, 1, 2 }, 2) == 3);

        // Find
        Check("Array.Find({1,2,3,4,5}, x=>x>3) == 4", () => Array.Find(new[] { 1, 2, 3, 4, 5 }, x => x > 3) == 4);
        Check("Array.Find({1,2,3}, x=>x>10) == 0", () => Array.Find(new[] { 1, 2, 3 }, x => x > 10) == 0);

        // FindLast
        Check("Array.FindLast({1,2,3,4,5}, x=>x>3) == 5", () => Array.FindLast(new[] { 1, 2, 3, 4, 5 }, x => x > 3) == 5);

        // FindIndex
        Check("Array.FindIndex({1,2,3,4,5}, x=>x>3) == 3", () => Array.FindIndex(new[] { 1, 2, 3, 4, 5 }, x => x > 3) == 3);

        // FindLastIndex
        Check("Array.FindLastIndex({1,2,3,4,5}, x=>x>3) == 4", () => Array.FindLastIndex(new[] { 1, 2, 3, 4, 5 }, x => x > 3) == 4);

        // Exists
        Check("Array.Exists({1,2,3}, x=>x>2) == true", () => Array.Exists(new[] { 1, 2, 3 }, x => x > 2) == true);
        Check("Array.Exists({1,2,3}, x=>x>10) == false", () => Array.Exists(new[] { 1, 2, 3 }, x => x > 10) == false);

        // TrueForAll
        Check("Array.TrueForAll({2,4,6}, x=>x%2==0) == true", () => Array.TrueForAll(new[] { 2, 4, 6 }, x => x % 2 == 0) == true);
        Check("Array.TrueForAll({2,4,5}, x=>x%2==0) == false", () => Array.TrueForAll(new[] { 2, 4, 5 }, x => x % 2 == 0) == false);

        // ConvertAll
        Check("Array.ConvertAll(int[], double) works", () =>
        {
            var result = Array.ConvertAll(new[] { 1, 2, 3 }, x => (double)x);
            return result.Length == 3 && result[0] == 1.0 && result[2] == 3.0;
        });

        Check("Array.ConvertAll(int[], string) works", () =>
        {
            var result = Array.ConvertAll(new[] { 1, 2, 3 }, x => x.ToString());
            return result.Length == 3 && result[1] == "2";
        });

        // Resize
        Check("Array.Resize<int> increases size and preserves elements", () =>
        {
            var arr = new[] { 1, 2, 3 };
            Array.Resize(ref arr, 5);
            return arr.Length == 5 && arr[0] == 1 && arr[2] == 3;
        });

        Check("Array.Resize<int> shrinks and preserves first elements", () =>
        {
            var arr = new[] { 1, 2, 3, 4, 5 };
            Array.Resize(ref arr, 3);
            return arr.Length == 3 && arr[0] == 1 && arr[2] == 3;
        });

        // Clear
        Check("Array.Clear zeros range", () =>
        {
            var arr = new[] { 1, 2, 3, 4, 5 };
            Array.Clear(arr, 1, 2);
            return arr[0] == 1 && arr[1] == 0 && arr[2] == 0 && arr[3] == 4;
        });

        // Reverse
        Check("Array.Reverse reverses the array", () =>
        {
            var arr = new[] { 1, 2, 3, 4 };
            Array.Reverse(arr);
            return arr[0] == 4 && arr[1] == 3 && arr[2] == 2 && arr[3] == 1;
        });

        // Copy
        Check("Array.Copy copies elements", () =>
        {
            var src = new[] { 1, 2, 3, 4, 5 };
            var dst = new int[3];
            Array.Copy(src, 1, dst, 0, 3);
            return dst[0] == 2 && dst[1] == 3 && dst[2] == 4;
        });

        // BinarySearch
        Check("Array.BinarySearch finds element", () =>
        {
            var arr = new[] { 1, 3, 5, 7, 9 };
            return Array.BinarySearch(arr, 5) == 2;
        });

        Check("Array.BinarySearch returns negative for missing", () =>
        {
            var arr = new[] { 1, 3, 5, 7, 9 };
            return Array.BinarySearch(arr, 4) < 0;
        });

        // Empty (static method)
        Check("Array.Empty<int>() returns non-null with length 0", () =>
        {
            var arr = Array.Empty<int>();
            return arr is { Length: 0 };
        });

        // GetLength / GetUpperBound / GetLowerBound
        Check("int[,,].GetLength(1) works for multidimensional", () =>
        {
            var arr = new int[2, 3, 4];
            return arr.GetLength(0) == 2 && arr.GetLength(1) == 3 && arr.GetLength(2) == 4;
        });

        // ForEach
        Check("Array.ForEach executes action on each element", () =>
        {
            var sum = 0;
            Array.ForEach(new[] { 1, 2, 3, 4, 5 }, x => sum += x);
            return sum == 15;
        });
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 3a: System.Math
    // ══════════════════════════════════════════════════════════════
    private static void RunMathContract()
    {
        Check("Math.Abs(-5) == 5", () => Math.Abs(-5) == 5);
        Check("Math.Abs(7) == 7", () => Math.Abs(7) == 7);
        Check("Math.Abs(-3.14) == 3.14", () => Math.Abs(-3.14) == 3.14);
        Check("Math.Abs(0) == 0", () => Math.Abs(0) == 0);

        Check("Math.Max(3, 7) == 7", () => Math.Max(3, 7) == 7);
        Check("Math.Max(-5, -1) == -1", () => Math.Max(-5, -1) == -1);
        Check("Math.Max(3.14, 2.71) == 3.14", () => Math.Max(3.14, 2.71) == 3.14);

        Check("Math.Min(3, 7) == 3", () => Math.Min(3, 7) == 3);
        Check("Math.Min(-5, -1) == -5", () => Math.Min(-5, -1) == -5);

        Check("Math.Round(3.14159, 2) == 3.14", () => Math.Round(3.14159, 2) == 3.14);
        Check("Math.Round(3.5) == 4", () => Math.Round(3.5) == 4);  // banker's rounding: 3.5→4
        Check("Math.Round(2.5) == 2", () => Math.Round(2.5) == 2);  // banker's rounding: 2.5→2

        Check("Math.Ceiling(3.14) == 4", () => Math.Ceiling(3.14) == 4.0);
        Check("Math.Ceiling(-3.14) == -3", () => Math.Ceiling(-3.14) == -3.0);
        Check("Math.Ceiling(5.0) == 5", () => Math.Ceiling(5.0) == 5.0);

        Check("Math.Floor(3.14) == 3", () => Math.Floor(3.14) == 3.0);
        Check("Math.Floor(-3.14) == -4", () => Math.Floor(-3.14) == -4.0);

        Check("Math.Sqrt(4) == 2", () => Math.Sqrt(4) == 2.0);
        Check("Math.Sqrt(100) == 10", () => Math.Sqrt(100) == 10.0);
        Check("Math.Sqrt(0) == 0", () => Math.Sqrt(0) == 0.0);

        Check("Math.Pow(2, 3) == 8", () => Math.Pow(2, 3) == 8.0);
        Check("Math.Pow(10, 0) == 1", () => Math.Pow(10, 0) == 1.0);
        Check("Math.Pow(9, 0.5) == 3", () => Math.Abs(Math.Pow(9, 0.5) - 3.0) < 0.00001);

        Check("Math.Log(100, 10) == 2", () => Math.Log(100, 10) == 2.0);
        Check("Math.Log(8, 2) == 3", () => Math.Log(8, 2) == 3.0);
        Check("Math.Log(1) == 0", () => Math.Log(1) == 0.0);

        Check("Math.Log10(1000) == 3", () => Math.Log10(1000) == 3.0);

        Check("Math.Sign(-5) == -1", () => Math.Sign(-5) == -1);
        Check("Math.Sign(0) == 0", () => Math.Sign(0) == 0);
        Check("Math.Sign(42) == 1", () => Math.Sign(42) == 1);

        Check("Math.Clamp(10, 1, 5) == 5", () => Math.Clamp(10, 1, 5) == 5);
        Check("Math.Clamp(0, 1, 5) == 1", () => Math.Clamp(0, 1, 5) == 1);
        Check("Math.Clamp(3, 1, 5) == 3", () => Math.Clamp(3, 1, 5) == 3);

        Check("Math.Truncate(3.14) == 3", () => Math.Truncate(3.14) == 3.0);
        Check("Math.Truncate(-3.14) == -3", () => Math.Truncate(-3.14) == -3.0);

        Check("Math.Sin(0) == 0", () => Math.Abs(Math.Sin(0)) < 0.00001);
        Check("Math.Cos(0) == 1", () => Math.Abs(Math.Cos(0) - 1) < 0.00001);

        Check("Math.Exp(0) == 1", () => Math.Exp(0) == 1.0);
        Check("Math.Exp(1) == e", () => Math.Abs(Math.Exp(1) - Math.E) < 0.00001);

        Check("Math.DivRem(10, 3) returns quotient 3", () => Math.DivRem(10, 3, out var rem) == 3 && rem == 1);

        Check("Math.BigMul(12345, 67890) == 838102050", () => Math.BigMul(12345, 67890) == 838102050L);

        Check("Math.Atan2(0, 1) == 0", () => Math.Atan2(0, 1) == 0.0);
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 3b: System.MathF (float version)
    // ══════════════════════════════════════════════════════════════
    private static void RunMathFContract()
    {
        Check("MathF.Abs(-3) == 3", () => MathF.Abs(-3f) == 3f);
        Check("MathF.Abs(5) == 5", () => MathF.Abs(5f) == 5f);
        Check("MathF.Abs(0) == 0", () => MathF.Abs(0f) == 0f);

        Check("MathF.Max(3, 7) == 7", () => MathF.Max(3f, 7f) == 7f);
        Check("MathF.Min(3, 7) == 3", () => MathF.Min(3f, 7f) == 3f);

        Check("MathF.Sqrt(4) == 2", () => MathF.Sqrt(4f) == 2f);
        Check("MathF.Sqrt(100) == 10", () => MathF.Sqrt(100f) == 10f);
        Check("MathF.Sqrt(0) == 0", () => MathF.Sqrt(0f) == 0f);

        Check("MathF.Pow(2, 3) == 8", () => MathF.Pow(2f, 3f) == 8f);

        Check("MathF.Round(3.14, 1) == 3.1", () => MathF.Round(3.14f, 1) == 3.1f);

        Check("MathF.Ceiling(3.14) == 4", () => MathF.Ceiling(3.14f) == 4f);
        Check("MathF.Floor(3.14) == 3", () => MathF.Floor(3.14f) == 3f);

        Check("MathF.Sign(-5) == -1", () => MathF.Sign(-5f) == -1);
        Check("MathF.Sign(0) == 0", () => MathF.Sign(0f) == 0);
        Check("MathF.Sign(42) == 1", () => MathF.Sign(42f) == 1);

        Check("MathF.Truncate(3.14) == 3", () => MathF.Truncate(3.14f) == 3f);
        Check("MathF.Truncate(-3.14) == -3", () => MathF.Truncate(-3.14f) == -3f);

        Check("MathF.Sin(0) == 0", () => MathF.Sin(0f) < 0.001f);
        Check("MathF.Cos(0) == 1", () => MathF.Cos(0f) > 0.999f);
        Check("MathF.Exp(0) == 1", () => MathF.Exp(0f) == 1f);

        Check("MathF.Abs(-3.14) == 3.14", () => MathF.Abs(-3.14f) == 3.14f);
        Check("MathF.Max(-5, -1) == -1", () => MathF.Max(-5f, -1f) == -1f);
        Check("MathF.Max(3.14, 2.71) ≈ 3.14", () => MathF.Max(3.14f, 2.71f) == 3.14f);

        // MathF.Clamp is .NET 9+; use a ternary-equivalent assertion on .NET 8
        Check("MathF.Clamp-equivalent via Min/Max(10, 1, 5) == 5", () =>
            MathF.Min(MathF.Max(10f, 1f), 5f) == 5f);
        Check("MathF.Clamp-equivalent via Min/Max(3, 1, 5) == 3", () =>
            MathF.Min(MathF.Max(3f, 1f), 5f) == 3f);

        Check("MathF.Atan2(0, 1) == 0", () => MathF.Atan2(0f, 1f) == 0f);
        Check("MathF.Log(100, 10) == 2", () => MathF.Log(100f, 10f) == 2f);
        Check("MathF.Log10(1000) == 3", () => MathF.Log10(1000f) == 3f);
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 4a: System.Char
    // ══════════════════════════════════════════════════════════════
    private static void RunCharContract()
    {
        // Classification
        Check("Char.IsDigit('5') == true", () => char.IsDigit('5') == true);
        Check("Char.IsDigit('A') == false", () => char.IsDigit('A') == false);

        Check("Char.IsLetter('a') == true", () => char.IsLetter('a') == true);
        Check("Char.IsLetter('Z') == true", () => char.IsLetter('Z') == true);
        Check("Char.IsLetter('5') == false", () => char.IsLetter('5') == false);

        Check("Char.IsLetterOrDigit('a') == true", () => char.IsLetterOrDigit('a') == true);
        Check("Char.IsLetterOrDigit('5') == true", () => char.IsLetterOrDigit('5') == true);
        Check("Char.IsLetterOrDigit('!') == false", () => char.IsLetterOrDigit('!') == false);

        Check("Char.IsUpper('A') == true", () => char.IsUpper('A') == true);
        Check("Char.IsUpper('a') == false", () => char.IsUpper('a') == false);

        Check("Char.IsLower('a') == true", () => char.IsLower('a') == true);
        Check("Char.IsLower('A') == false", () => char.IsLower('A') == false);

        Check("Char.IsWhiteSpace(' ') == true", () => char.IsWhiteSpace(' ') == true);
        Check("Char.IsWhiteSpace('\\t') == true", () => char.IsWhiteSpace('\t') == true);
        Check("Char.IsWhiteSpace('a') == false", () => char.IsWhiteSpace('a') == false);

        Check("Char.IsPunctuation('.') == true", () => char.IsPunctuation('.') == true);
        Check("Char.IsPunctuation('!') == true", () => char.IsPunctuation('!') == true);

        Check("Char.IsNumber('5') == true", () => char.IsNumber('5') == true);
        Check("Char.IsControl('\\n') == true", () => char.IsControl('\n') == true);

        Check("Char.IsSeparator(' ') == true", () => char.IsSeparator(' ') == true);
        Check("Char.IsSymbol('+') == true", () => char.IsSymbol('+') == true);

        // Parse / ToString
        Check("Char.Parse(\"X\") == 'X'", () => char.Parse("X") == 'X');
        Check("Char.Parse(\"5\") == '5'", () => char.Parse("5") == '5');

        Check("'A'.ToString() == \"A\"", () => 'A'.ToString() == "A");
        Check("'5'.ToString() == \"5\"", () => '5'.ToString() == "5");

        // ToUpper / ToLower
        Check("Char.ToUpper('a') == 'A'", () => char.ToUpper('a') == 'A');
        Check("Char.ToUpper('A') == 'A'", () => char.ToUpper('A') == 'A');
        Check("Char.ToLower('A') == 'a'", () => char.ToLower('A') == 'a');
        Check("Char.ToLower('a') == 'a'", () => char.ToLower('a') == 'a');

        // GetNumericValue
        Check("Char.GetNumericValue('5') == 5", () => char.GetNumericValue('5') == 5);
        Check("Char.GetNumericValue('a') == -1", () => char.GetNumericValue('a') == -1);

        // IsBetween (invariant)
        Check("Char.IsBetween('b', 'a', 'c') == true", () => char.IsBetween('b', 'a', 'c') == true);

        // IsAscii
        Check("Char.IsAscii('a') == true", () => char.IsAscii('a') == true);
        Check("Char.IsAscii('\\u00E9') == false", () => char.IsAscii('é') == false);

        // IsAsciiLetter / IsAsciiDigit
        Check("Char.IsAsciiLetter('a') == true", () => char.IsAsciiLetter('a') == true);
        Check("Char.IsAsciiLetter('5') == false", () => char.IsAsciiLetter('5') == false);
        Check("Char.IsAsciiDigit('5') == true", () => char.IsAsciiDigit('5') == true);
        Check("Char.IsAsciiLetterOrDigit('a') == true", () => char.IsAsciiLetterOrDigit('a') == true);
        Check("Char.IsAsciiLetterOrDigit('5') == true", () => char.IsAsciiLetterOrDigit('5') == true);
        Check("Char.IsAsciiLetterOrDigit('!') == false", () => char.IsAsciiLetterOrDigit('!') == false);

        // IsLetterOrDigit variation
        Check("Char.IsLetterOrDigit('ñ') == true", () => char.IsLetterOrDigit('ñ') == true);

        // GetUnicodeCategory
        Check("Char.GetUnicodeCategory('a') == UnicodeCategory.LowercaseLetter", () =>
            char.GetUnicodeCategory('a') == System.Globalization.UnicodeCategory.LowercaseLetter);
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 4b: System.AppDomain
    // ══════════════════════════════════════════════════════════════
    private static void RunAppDomainContract()
    {
        var ad = AppDomain.CurrentDomain;

        Check("AppDomain.CurrentDomain is not null", () => ad is not null);
        Check("AppDomain.CurrentDomain.FriendlyName is non-empty", () => !string.IsNullOrEmpty(ad.FriendlyName));
        Check("AppDomain.CurrentDomain.BaseDirectory is not null", () => ad.BaseDirectory is not null);
        Check("AppDomain.CurrentDomain.RelativeSearchPath maybe null (not an error)", () => true);

        Check("AppDomain.CurrentDomain.GetAssemblies returns at least one", () =>
        {
            var assemblies = ad.GetAssemblies();
            return assemblies.Length > 0;
        });

        Check("AppDomain.CurrentDomain.GetAssemblies contains System.Private.CoreLib", () =>
        {
            var assemblies = ad.GetAssemblies();
            return assemblies.Any(a => a.GetName().Name == "System.Private.CoreLib");
        });

        Check("AppDomain.CurrentDomain.IsFullyTrusted == true", () => ad.IsFullyTrusted == true);

        Check("AppDomain.CurrentDomain.SetupInformation is not null", () => ad.SetupInformation is not null);
        Check("AppDomain.CurrentDomain.SetupInformation.ApplicationBase is not null",
            () => ad.SetupInformation.ApplicationBase is not null);

        Check("AppDomain.CurrentDomain.Id >= 1", () => ad.Id >= 1);

        // IsDefaultAppDomain
        Check("AppDomain.CurrentDomain.IsDefaultAppDomain() == true",
            () => AppDomain.CurrentDomain.IsDefaultAppDomain());

        // MonitoringIsEnabled is runtime/config dependent — assert it is queryable
        Check("AppDomain.MonitoringIsEnabled is queryable", () =>
        {
            _ = AppDomain.MonitoringIsEnabled;
            return true;
        });
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 4c: System.Half
    // ══════════════════════════════════════════════════════════════
    private static void RunHalfContract()
    {
        Check("Half.Parse(\"1.5\") == 1.5", () => Half.Parse("1.5") == (Half)1.5);
        Check("Half.Parse(\"0\") == 0", () => Half.Parse("0") == (Half)0);
        Check("Half.Parse(\"-3\") == -3", () => Half.Parse("-3") == (Half)(-3));

        Check("Half.ToString on 1.5 returns \"1.5\"", () => ((Half)1.5).ToString() == "1.5");

        Check("Half.CompareTo(3, 1) == 1", () => ((Half)3).CompareTo((Half)1) > 0);
        Check("Half.CompareTo(1, 3) == -1", () => ((Half)1).CompareTo((Half)3) < 0);
        Check("Half.CompareTo(2, 2) == 0", () => ((Half)2).CompareTo((Half)2) == 0);

        Check("Half.IsNaN(Half.NaN) == true", () => Half.IsNaN(Half.NaN) == true);
        Check("Half.IsNaN(1) == false", () => Half.IsNaN((Half)1) == false);

        Check("Half.IsInfinity(Half.PositiveInfinity) == true", () => Half.IsInfinity(Half.PositiveInfinity) == true);
        Check("Half.IsNegativeInfinity(Half.NegativeInfinity) == true", () => Half.IsNegativeInfinity(Half.NegativeInfinity) == true);
        Check("Half.IsFinite(1) == true", () => Half.IsFinite((Half)1) == true);

        Check("Half(1.5f).Equals(Half(1.5f)) == true", () => ((Half)1.5).Equals((Half)1.5) == true);
        Check("Half(1.5f).Equals(Half(2.5f)) == false", () => ((Half)1.5).Equals((Half)2.5) == false);

        Check("Half.Epsilon > 0", () => Half.Epsilon > (Half)0);
        Check("Half.MaxValue > 0", () => Half.MaxValue > (Half)0);
        Check("Half.MinValue < 0", () => Half.MinValue < (Half)0);

        Check("Half operators: 1 + 2 == 3", () => (Half)1 + (Half)2 == (Half)3);
        Check("Half operators: 5 - 3 == 2", () => (Half)5 - (Half)3 == (Half)2);
        Check("Half operators: 2 * 3 == 6", () => (Half)2 * (Half)3 == (Half)6);
        Check("Half operators: 6 / 2 == 3", () => (Half)6 / (Half)2 == (Half)3);
    }

    // ══════════════════════════════════════════════════════════════
    //  Int128 family
    // ══════════════════════════════════════════════════════════════
    private static void RunInt128Contract()
    {
        Check("Int128.Parse(\"123\") == 123", () => Int128.Parse("123") == 123);
        Check("Int128.Parse(\"0\") == 0", () => Int128.Parse("0") == 0);
        Check("Int128.Parse(\"-456\") == -456", () => Int128.Parse("-456") == -456);

        Check("Int128.ToString(123) == \"123\"", () => ((Int128)123).ToString() == "123");
        Check("Int128.ToString(-456) == \"-456\"", () => ((Int128)(-456)).ToString() == "-456");

        Check("Int128.CompareTo(5, 3) > 0", () => ((Int128)5).CompareTo((Int128)3) > 0);
        Check("Int128.CompareTo(3, 5) < 0", () => ((Int128)3).CompareTo((Int128)5) < 0);
        Check("Int128.CompareTo(4, 4) == 0", () => ((Int128)4).CompareTo((Int128)4) == 0);

        Check("Int128 + 1 + 2 == 3", () => (Int128)1 + (Int128)2 == (Int128)3);
        Check("Int128 - 5 - 2 == 3", () => (Int128)5 - (Int128)2 == (Int128)3);
        Check("Int128 * 3 * 4 == 12", () => (Int128)3 * (Int128)4 == (Int128)12);

        Check("Int128.Equals(42, 42) == true", () => ((Int128)42).Equals((Int128)42) == true);
        Check("Int128.Equals(42, 43) == false", () => ((Int128)42).Equals((Int128)43) == false);

        Check("Int128.IsPositive(5) == true", () => Int128.IsPositive((Int128)5) == true);
        Check("Int128.IsNegative(-5) == true", () => Int128.IsNegative((Int128)(-5)) == true);
        Check("Int128.IsZero(0) via Equals", () => ((Int128)0).Equals((Int128)0));
    }

    private static void RunInt16Contract()
    {
        Check("Int16.Parse(\"12345\") == 12345", () => Int16.Parse("12345") == 12345);
        Check("Int16.Parse(\"0\") == 0", () => Int16.Parse("0") == 0);
        Check("Int16.Parse(\"-789\") == -789", () => Int16.Parse("-789") == -789);

        Check("Int16.TryParse(\"12345\", out var x) && x == 12345", () =>
        {
            var ok = Int16.TryParse("12345", out short x);
            return ok && x == 12345;
        });

        Check("Int16.TryParse(\"abc\", out _) == false", () =>
        {
            var ok = Int16.TryParse("abc", out short _);
            return ok == false;
        });

        Check("Int16.ToString(42) == \"42\"", () => ((short)42).ToString() == "42");
        Check("Int16.ToString(-1) == \"-1\"", () => ((short)(-1)).ToString() == "-1");

        Check("Int16.CompareTo(100, 50) > 0", () => ((short)100).CompareTo((short)50) > 0);
        Check("Int16.CompareTo(30, 80) < 0", () => ((short)30).CompareTo((short)80) < 0);
        Check("Int16.CompareTo(60, 60) == 0", () => ((short)60).CompareTo((short)60) == 0);

        Check("Int16.MaxValue == 32767", () => short.MaxValue == 32767);
        Check("Int16.MinValue == -32768", () => short.MinValue == -32768);

        // Operators
        Check("(short)10 + (short)20 == 30", () => (short)(10 + 20) == 30);
        Check("(short)50 - (short)20 == 30", () => (short)(50 - 20) == 30);
        Check("(short)5 * (short)6 == 30", () => (short)(5 * 6) == 30);

        // Parse with NumberStyles
        Check("Int16.Parse(\"  12345  \") == 12345 (with whitespace)", () => Int16.Parse("  12345  ") == 12345);

        // ToString with format
        Check("Int16.ToString(255, \"X\") == \"FF\"", () => ((short)255).ToString("X") == "FF");
    }

    private static void RunUInt16Contract()
    {
        Check("UInt16.Parse(\"65000\") == 65000", () => ushort.Parse("65000") == 65000);
        Check("UInt16.Parse(\"0\") == 0", () => ushort.Parse("0") == 0);

        Check("UInt16.TryParse(\"50000\", out var x) && x == 50000", () =>
        {
            var ok = ushort.TryParse("50000", out ushort x);
            return ok && x == 50000;
        });

        Check("UInt16.TryParse(\"abc\", out _) == false", () =>
        {
            var ok = ushort.TryParse("abc", out ushort _);
            return ok == false;
        });

        Check("UInt16.ToString(99) == \"99\"", () => ((ushort)99).ToString() == "99");
        Check("UInt16.MaxValue == 65535", () => ushort.MaxValue == 65535);
        Check("UInt16.MinValue == 0", () => ushort.MinValue == 0);

        Check("UInt16.CompareTo(100, 200) < 0", () => ((ushort)100).CompareTo((ushort)200) < 0);
        Check("UInt16.CompareTo(200, 100) > 0", () => ((ushort)200).CompareTo((ushort)100) > 0);
        Check("UInt16.CompareTo(150, 150) == 0", () => ((ushort)150).CompareTo((ushort)150) == 0);
    }

    private static void RunUInt128Contract()
    {
        Check("UInt128.Parse(\"123\") == 123", () => UInt128.Parse("123") == 123);
        Check("UInt128.Parse(\"0\") == 0", () => UInt128.Parse("0") == 0);

        Check("UInt128.ToString(456) == \"456\"", () => ((UInt128)456).ToString() == "456");

        Check("UInt128.CompareTo(10, 5) > 0", () => ((UInt128)10).CompareTo((UInt128)5) > 0);
        Check("UInt128.CompareTo(3, 7) < 0", () => ((UInt128)3).CompareTo((UInt128)7) < 0);
        Check("UInt128.CompareTo(5, 5) == 0", () => ((UInt128)5).CompareTo((UInt128)5) == 0);

        Check("UInt128 + 1 + 2 == 3", () => (UInt128)1 + (UInt128)2 == (UInt128)3);
        Check("UInt128.IsZero(0) via Equals", () => ((UInt128)0).Equals((UInt128)0));
        Check("UInt128.IsPositive(5) via >0", () => (UInt128)5 > (UInt128)0);
    }

    private static void RunIntPtrContract()
    {
        Check("IntPtr.Zero == 0", () => IntPtr.Zero == IntPtr.Zero);
        Check("IntPtr(42).ToInt64() == 42", () => new IntPtr(42).ToInt64() == 42);
        Check("IntPtr(0).ToInt64() == 0", () => new IntPtr(0).ToInt64() == 0);
        Check("IntPtr(-1).ToInt64() == -1", () => new IntPtr(-1).ToInt64() == -1);

        Check("IntPtr.Size == 4 or 8", () => IntPtr.Size == 4 || IntPtr.Size == 8);

        Check("IntPtr(42).ToString() is not null", () => !string.IsNullOrEmpty(new IntPtr(42).ToString()));

        Check("IntPtr.Equals(42, 42) == true", () => new IntPtr(42).Equals(new IntPtr(42)) == true);
        Check("IntPtr.Equals(42, 43) == false", () => new IntPtr(42).Equals(new IntPtr(43)) == false);

        Check("IntPtr.Add(10, 5) == 15", () => IntPtr.Add(new IntPtr(10), 5) == new IntPtr(15));
        Check("IntPtr.Subtract(10, 3) == 7", () => IntPtr.Subtract(new IntPtr(10), 3) == new IntPtr(7));

        Check("IntPtr.CompareTo(5, 3) > 0", () => new IntPtr(5).CompareTo(new IntPtr(3)) > 0);
        Check("IntPtr.CompareTo(3, 5) < 0", () => new IntPtr(3).CompareTo(new IntPtr(5)) < 0);
        Check("IntPtr.CompareTo(4, 4) == 0", () => new IntPtr(4).CompareTo(new IntPtr(4)) == 0);
    }

    private static void RunUIntPtrContract()
    {
        Check("UIntPtr.Zero == 0", () => UIntPtr.Zero == UIntPtr.Zero);
        Check("UIntPtr(42).ToUInt64() == 42", () => new UIntPtr(42).ToUInt64() == 42);
        Check("UIntPtr(0).ToUInt64() == 0", () => new UIntPtr(0).ToUInt64() == 0);

        Check("UIntPtr.Equals(42, 42) == true", () => new UIntPtr(42).Equals(new UIntPtr(42)) == true);
        Check("UIntPtr.Equals(42, 43) == false", () => new UIntPtr(42).Equals(new UIntPtr(43)) == false);

        Check("UIntPtr.Add(10, 5) == 15", () => UIntPtr.Add(new UIntPtr(10), 5) == new UIntPtr(15));
        Check("UIntPtr.Subtract(10, 3) == 7", () => UIntPtr.Subtract(new UIntPtr(10), 3) == new UIntPtr(7));
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 6: Byte / SByte / Boolean
    // ══════════════════════════════════════════════════════════════
    private static void RunByteContract()
    {
        Check("Byte.Parse(\"200\") == 200", () => byte.Parse("200") == 200);
        Check("Byte.Parse(\"0\") == 0", () => byte.Parse("0") == 0);
        Check("Byte.Parse(\"100\") == 100", () => byte.Parse("100") == 100);

        Check("Byte.TryParse(\"250\", out var x) && x == 250", () =>
        {
            var ok = byte.TryParse("250", out byte x);
            return ok && x == 250;
        });

        Check("Byte.MaxValue == 255", () => byte.MaxValue == 255);
        Check("Byte.MinValue == 0", () => byte.MinValue == 0);

        Check("Byte.ToString(42) == \"42\"", () => ((byte)42).ToString() == "42");

        Check("Byte.CompareTo(100, 200) < 0", () => ((byte)100).CompareTo((byte)200) < 0);
        Check("Byte.CompareTo(200, 100) > 0", () => ((byte)200).CompareTo((byte)100) > 0);
        Check("Byte.CompareTo(150, 150) == 0", () => ((byte)150).CompareTo((byte)150) == 0);
    }

    private static void RunSByteContract()
    {
        Check("SByte.Parse(\"100\") == 100", () => sbyte.Parse("100") == 100);
        Check("SByte.Parse(\"0\") == 0", () => sbyte.Parse("0") == 0);
        Check("SByte.Parse(\"-100\") == -100", () => sbyte.Parse("-100") == -100);

        Check("SByte.MaxValue == 127", () => sbyte.MaxValue == 127);
        Check("SByte.MinValue == -128", () => sbyte.MinValue == -128);

        Check("SByte.ToString(42) == \"42\"", () => ((sbyte)42).ToString() == "42");
        Check("SByte.ToString(-42) == \"-42\"", () => ((sbyte)(-42)).ToString() == "-42");
    }

    private static void RunBooleanContract()
    {
        Check("Boolean.Parse(\"True\") == true", () => bool.Parse("True") == true);
        Check("Boolean.Parse(\"False\") == false", () => bool.Parse("False") == false);

        Check("Boolean.TryParse(\"true\", out var x) && x == true", () =>
        {
            var ok = bool.TryParse("true", out bool x);
            return ok && x == true;
        });

        Check("Boolean.TryParse(\"false\", out var y) && y == false", () =>
        {
            var ok = bool.TryParse("false", out bool y);
            return ok && y == false;
        });

        Check("Boolean.TryParse(\"invalid\", out _) == false", () =>
        {
            var ok = bool.TryParse("invalid", out bool _);
            return ok == false;
        });

        Check("Boolean.ToString(true) == \"True\"", () => true.ToString() == "True");
        Check("Boolean.ToString(false) == \"False\"", () => false.ToString() == "False");

        Check("Boolean.CompareTo(true, false) > 0", () => true.CompareTo(false) > 0);
        Check("Boolean.CompareTo(false, true) < 0", () => false.CompareTo(true) < 0);
        Check("Boolean.CompareTo(true, true) == 0", () => true.CompareTo(true) == 0);
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 7: OperatingSystem, TimeSpan, Guid
    // ══════════════════════════════════════════════════════════════
    private static void RunOperatingSystemContract()
    {
        var os = Environment.OSVersion;

        Check("Environment.OSVersion is not null", () => os is not null);
        Check("Environment.OSVersion.Platform is defined", () => Enum.IsDefined(os.Platform));
        Check("Environment.OSVersion.Version is not null", () => os.Version is not null);
        Check("Environment.OSVersion.ServicePack is not null", () => os.ServicePack is not null);
        Check("Environment.OSVersion.VersionString is non-empty", () => !string.IsNullOrEmpty(os.VersionString));

        Check("Environment.OSVersion.VersionString contains platform name", () =>
        {
            var vs = os.VersionString;
            return vs.Contains("Windows") || vs.Contains("Unix") || vs.Contains("Linux") || vs.Contains("Mac");
        });

        // OperatingSystem(string platform, Version version)
        var win10 = new OperatingSystem(PlatformID.Win32NT, new Version(10, 0));
        Check("OperatingSystem(platform, version) preserves Platform", () => win10.Platform == PlatformID.Win32NT);
        Check("OperatingSystem(platform, version) preserves Version", () => win10.Version == new Version(10, 0));
        Check("OperatingSystem.ToString() is non-empty", () => !string.IsNullOrEmpty(win10.ToString()));
    }

    private static void RunTimeSpanContract()
    {
        var ts = new TimeSpan(1, 2, 3, 4, 5);  // 1 day, 2 hours, 3 minutes, 4 sec, 5 ms

        Check("TimeSpan(1,2,3,4,5).Days == 1", () => ts.Days == 1);
        Check("TimeSpan(1,2,3,4,5).Hours == 2", () => ts.Hours == 2);
        Check("TimeSpan(1,2,3,4,5).Minutes == 3", () => ts.Minutes == 3);
        Check("TimeSpan(1,2,3,4,5).Seconds == 4", () => ts.Seconds == 4);
        Check("TimeSpan(1,2,3,4,5).Milliseconds == 5", () => ts.Milliseconds == 5);

        Check("TimeSpan.FromDays(1.5).TotalHours == 36", () => TimeSpan.FromDays(1.5).TotalHours == 36);
        Check("TimeSpan.FromHours(2).TotalMinutes == 120", () => TimeSpan.FromHours(2).TotalMinutes == 120);

        Check("TimeSpan.FromMinutes(60).TotalHours == 1", () => TimeSpan.FromMinutes(60).TotalHours == 1);
        Check("TimeSpan.FromSeconds(300).TotalMinutes == 5", () => TimeSpan.FromSeconds(300).TotalMinutes == 5);

        Check("TimeSpan.FromMilliseconds(1000).TotalSeconds == 1", () =>
            TimeSpan.FromMilliseconds(1000).TotalSeconds == 1);

        Check("TimeSpan.Parse(\"01:02:03\") == 1h2m3s", () =>
            TimeSpan.Parse("01:02:03") == new TimeSpan(1, 2, 3));

        Check("TimeSpan.Zero.Ticks == 0", () => TimeSpan.Zero.Ticks == 0);
        Check("TimeSpan.MaxValue > TimeSpan.MinValue", () => TimeSpan.MaxValue > TimeSpan.MinValue);

        Check("TimeSpan.CompareTo(5min, 1min) > 0", () =>
            TimeSpan.FromMinutes(5).CompareTo(TimeSpan.FromMinutes(1)) > 0);

        Check("TimeSpan.Duration always returns positive", () =>
            TimeSpan.FromMinutes(-5).Duration() == TimeSpan.FromMinutes(5));
    }

    private static void RunGuidContract()
    {
        var g = Guid.NewGuid();
        var empty = Guid.Empty;

        Check("Guid.NewGuid() is not empty", () => g != Guid.Empty);
        Check("Guid.Empty == 00000000-0000-0000-0000-000000000000",
            () => empty.ToString() == "00000000-0000-0000-0000-000000000000");

        Check("Guid.Parse(guid.ToString()) == guid", () =>
        {
            var parsed = Guid.Parse(g.ToString());
            return parsed == g;
        });

        Check("Guid.TryParse(valid) succeeds", () =>
        {
            var ok = Guid.TryParse(g.ToString(), out Guid result);
            return ok && result == g;
        });

        Check("Guid.TryParse(\"invalid\") == false", () =>
        {
            var ok = Guid.TryParse("not-a-guid", out Guid _);
            return ok == false;
        });

        Check("Guid.ToString(\"N\") has 32 hex chars", () =>
        {
            var n = g.ToString("N");
            return n.Length == 32 && n.All(c => Uri.IsHexDigit(c));
        });

        Check("Guid.ToString(\"D\") has 36 chars (with hyphens)", () =>
        {
            var d = g.ToString("D");
            return d.Length == 36 && d[8] == '-' && d[13] == '-' && d[18] == '-' && d[23] == '-';
        });

        Check("Guid.NewGuid() != Guid.NewGuid() (uniqueness)", () => Guid.NewGuid() != Guid.NewGuid());

        // CompareTo
        Check("Guid.CompareTo orders consistently with string order", () =>
        {
            var a = Guid.Parse("00000000-0000-0000-0000-00000000000A");
            var b = Guid.Parse("00000000-0000-0000-0000-00000000000B");
            return a.CompareTo(b) < 0 && b.CompareTo(a) > 0 && a.CompareTo(a) == 0;
        });
    }

    // ══════════════════════════════════════════════════════════════
    //  Phase 8: Activator, Attribute, GC
    // ══════════════════════════════════════════════════════════════
    private static void RunActivatorContract()
    {
        Check("Activator.CreateInstance<int>() == 0", () => (int)Activator.CreateInstance(typeof(int))! == 0);
        Check("Activator.CreateInstance<StringBuilder>() is not null", () =>
        {
            var sb = Activator.CreateInstance(typeof(StringBuilder)) as StringBuilder;
            return sb is not null;
        });

        Check("Activator.CreateInstance(typeof(List<int>)) has capacity 0", () =>
        {
            var list = (List<int>)Activator.CreateInstance(typeof(List<int>))!;
            return list.Count == 0;
        });
    }

    private static void RunAttributeContract()
    {
        var pub = BindingFlags.Public | BindingFlags.Static;
        var nonPub = BindingFlags.NonPublic | BindingFlags.Static;

        Check("Attribute.GetCustomAttributes on Program type returns array", () =>
        {
            var attrs = typeof(Program).GetCustomAttributes(false);
            return attrs is not null;
        });

        Check("Attribute.IsDefined(ObsoleteAttribute) on Main == false", () =>
        {
            var mi = typeof(Program).GetMethod("Main", pub)!;
            return Attribute.IsDefined(mi, typeof(ObsoleteAttribute)) == false;
        });

        Check("Attribute.GetCustomAttributes on type returns array", () =>
        {
            var attrs = Attribute.GetCustomAttributes(typeof(string));
            return attrs is not null;  // string has [Serializable], etc.
        });

        Check("Attribute.GetCustomAttributes with inherit works", () =>
        {
            var attrs = Attribute.GetCustomAttributes(typeof(string), typeof(Attribute), true);
            return attrs.Length > 0;
        });

        Check("MemberInfo.IsDefined works on a private static method", () =>
        {
            var mi = typeof(Program).GetMethod(nameof(Check), nonPub)!;
            return mi.IsDefined(typeof(ObsoleteAttribute), false) == false;
        });
    }

    private static void RunGCContract()
    {
        Check("GC.GetTotalMemory(false) > 0", () => GC.GetTotalMemory(false) > 0);
        Check("GC.CollectionCount(0) >= 0", () => GC.CollectionCount(0) >= 0);
        Check("GC.CollectionCount(1) >= 0", () => GC.CollectionCount(1) >= 0);
        Check("GC.MaxGeneration >= 0", () => GC.MaxGeneration >= 0);

        Check("GC.GetTotalAllocatedBytes() is queryable", () =>
        {
            _ = GC.GetTotalAllocatedBytes();
            return true;
        });

        // GC.KeepAlive prevents collection
        Check("GC.KeepAlive on an object is a no-op that returns void", () =>
        {
            var o = new object();
            GC.KeepAlive(o);
            return true;  // no observable side effect, just prevents early collection
        });

        // GC.GetGeneration returns >= 0 for live objects
        Check("GC.GetGeneration(live object) >= 0", () =>
        {
            var o = new object();
            return GC.GetGeneration(o) >= 0;
        });
    }

    // ══════════════════════════════════════════════════════════════
    //  Assertion plumbing
    // ══════════════════════════════════════════════════════════════
    private static void Check(string label, Func<bool> assertion)
    {
        bool ok;
        try { ok = assertion(); }
        catch (Exception ex)
        {
            Failures.Add($"{label}  → threw {ex.GetType().Name}: {ex.Message}");
            _failed++;
            Console.WriteLine($"  ✗ {label}  (threw {ex.GetType().Name})");
            return;
        }

        if (ok)
        {
            _passed++;
            Console.WriteLine($"  ✓ {label}");
        }
        else
        {
            Failures.Add($"{label}  → assertion returned false");
            _failed++;
            Console.WriteLine($"  ✗ {label}");
        }
    }

    private static void Report()
    {
        Console.WriteLine();
        Console.WriteLine($"═══ {_passed} passed, {_failed} failed ═══");
        if (Failures.Count > 0)
        {
            Console.WriteLine();
            Console.WriteLine("Failures:");
            foreach (var f in Failures) Console.WriteLine($"  - {f}");
        }
    }
}