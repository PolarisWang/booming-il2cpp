namespace StubAssembly;

public static class StubMethods
{
    public static int ReturnZero() => 0;
    public static int Add(int a, int b) => a + b;
    public static void NoOp() { }
    public static string GetHello() => "Hello";

    // Boxing — triggers box opcode in lowering
    public static object BoxInt32(int x) => x;

    // Unboxing — triggers unbox opcode
    public static int UnboxToInt32(object x) => (int)x;

    // String concatenation — triggers string concat optimization
    public static string Concat(string a, string b) => a + b;

    // Division — triggers div opcode
    public static int Divide(int a, int b) => a / b;

    // Comparison — triggers cgt/clt opcodes
    public static bool IsGreaterThan(int a, int b) => a > b;
    public static bool IsLessThan(int a, int b) => a < b;

    // Array operations — triggers newarr, ldelem, stelem, ldlen
    public static int[] MakeArray(int a, int b) => new[] { a, b };
    public static int ArrayLength(int[] arr) => arr.Length;
    public static int FirstElement(int[] arr) => arr[0];

    // Loop with locals — exercises stloc/ldloc, br, branch opcodes
    public static int Sum(int[] arr)
    {
        var sum = 0;
        for (var i = 0; i < arr.Length; i++)
            sum += arr[i];
        return sum;
    }

    // Conditional (ternary) — exercises brtrue/brfalse
    public static int Max(int a, int b) => a > b ? a : b;

    // Exception handling — exercises throw/leave opcodes
    public static int SafeDivide(int a, int b)
    {
        if (b == 0)
            throw new System.DivideByZeroException();
        return a / b;
    }

    // Nested call — exercises call opcode with non-void return
    public static int DoubleAdd(int a, int b) => Add(a, b) + Add(b, a);

    // Null check — exercises brfalse on object
    public static bool IsNull(object? x) => x is null;

    // ──────────────────────────────────────────────
    // Switch statement — exercises switch opcode
    // ──────────────────────────────────────────────
    public static string DayName(int day) => day switch
    {
        1 => "Monday",
        2 => "Tuesday",
        3 => "Wednesday",
        4 => "Thursday",
        5 => "Friday",
        _ => "Unknown",
    };

    // ──────────────────────────────────────────────
    // Enum operations — exercises enum metadata
    // ──────────────────────────────────────────────
    public static bool IsWeekend(DayOfWeek day) =>
        day == DayOfWeek.Saturday || day == DayOfWeek.Sunday;

    // ──────────────────────────────────────────────
    // Interface dispatch via helper — exercises callvirt
    // ──────────────────────────────────────────────
    public static int UseCalculator(ICalculator calc, int x, int y) =>
        calc.Compute(x, y);

    // ──────────────────────────────────────────────
    // Nested try/catch — exercises exception regions
    // ──────────────────────────────────────────────
    public static int TryCatchDivide(int a, int b)
    {
        try
        {
            return a / b;
        }
        catch (System.DivideByZeroException)
        {
            return -1;
        }
    }

    // ──────────────────────────────────────────────
    // Generic method — exercises generic instantiation
    // ──────────────────────────────────────────────
    public static T Identity<T>(T value) => value;

    // ──────────────────────────────────────────────
    // Null coalescing — exercises related opcodes
    // ──────────────────────────────────────────────
    public static string Coalesce(string? a, string b) => a ?? b;

    // ──────────────────────────────────────────────
    // Enum.ToString — exercises box on enum + call
    // ──────────────────────────────────────────────
    public static string DayToString(DayOfWeek day) => day.ToString();

    // ──────────────────────────────────────────────
    // While loop — exercises brtrue loop pattern
    // ──────────────────────────────────────────────
    public static int CountTo(int n)
    {
        var count = 0;
        while (count < n)
            count++;
        return count;
    }

    // ──────────────────────────────────────────────
    // Type cast + check — exercises castclass, isinst
    // ──────────────────────────────────────────────
    public static string CastAndCheck(object obj) => obj is string s ? s : null;

    // ──────────────────────────────────────────────
    // Long/int64 arithmetic — exercises ldc.i8, add/mul on int64
    // ──────────────────────────────────────────────
    public static long UseLong(long a, long b) => a * b + a;

    // ──────────────────────────────────────────────
    // Floating point — exercises ldc.r4, ldc.r8, add/mul/div on float/double
    // ──────────────────────────────────────────────
    public static float UseFloat(float a, float b) => a * b + a;
    public static double UseDouble(double a, double b) => a / b;
    public static double IntToDouble(int a) => a; // conv.r8

    // ──────────────────────────────────────────────
    // Bitwise operations — exercises and, or, xor, not, shl, shr, neg
    // ──────────────────────────────────────────────
    public static int BitwiseAndOr(int a, int b) => (a & b) | (a ^ b);
    public static int ShiftBits(int x, int n) => (x << n) | (x >> n);
    public static int NegateValue(int a) => -a;

    // ──────────────────────────────────────────────
    // Modulo — exercises rem
    // ──────────────────────────────────────────────
    public static int Modulo(int a, int b) => a % b;
    public static uint ModUnsigned(uint a, uint b) => a % b; // rem.un

    // ──────────────────────────────────────────────
    // Subtraction/multiplication — exercises sub, mul
    // ──────────────────────────────────────────────
    public static int Subtract(int a, int b) => a - b;
    public static int Multiply(int a, int b) => a * b;

    // ──────────────────────────────────────────────
    // Unsigned division — exercises div.un
    // ──────────────────────────────────────────────
    public static uint DivideUnsigned(uint a, uint b) => a / b;

    // ──────────────────────────────────────────────
    // Overflow-checked arithmetic — exercises add.ovf, sub.ovf
    // ──────────────────────────────────────────────
    public static int OverflowAdd(int a, int b) => checked(a + b);
    public static int OverflowSub(int a, int b) => checked(a - b);

    // ──────────────────────────────────────────────
    // Ref/out parameters — exercises ldarga/starg/ldarg patterns
    // ──────────────────────────────────────────────
    public static void AddOne(ref int x) => x++;
    public static bool TryParseInt(string s, out int result) => int.TryParse(s, out result);

    // ──────────────────────────────────────────────
    // Static field load/store — exercises ldsfld, stsfld
    // ──────────────────────────────────────────────
    private static int _counter;
    public static int IncrementCounter() => ++_counter; // ldsfld + stsfld + add
    public static int ReadCounter() => _counter; // ldsfld

    // ──────────────────────────────────────────────
    // Type conversion — exercises conv.i1, conv.i2, conv.i8, conv.u4, conv.r4
    // ──────────────────────────────────────────────
    public static byte IntToByte(int a) => (byte)a; // conv.u1
    public static short IntToShort(int a) => (short)a; // conv.i2
    public static long IntToLong(int a) => a; // conv.i8
    public static float IntToFloat(int a) => a; // conv.r4

    // ──────────────────────────────────────────────
    // String.Join with IEnumerable — triggers EnumerableJoin support resolution
    // ──────────────────────────────────────────────
    public static string JoinStrings(string[] items, string separator) =>
        string.Join(separator, items);

    // ──────────────────────────────────────────────
    // Foreach on IEnumerable<T> — triggers generic interface dispatch + iterator
    // ──────────────────────────────────────────────
    public static int SumEnumerable(IEnumerable<int> items)
    {
        var sum = 0;
        foreach (var item in items)
            sum += item;
        return sum;
    }

    // ──────────────────────────────────────────────
    // List<T> operations — triggers generic instantiation for List<T>
    // ──────────────────────────────────────────────
    public static int UseList(int a, int b)
    {
        var list = new List<int> { a, b };
        return list[0] + list[1];
    }

    // ──────────────────────────────────────────────
    // Nullable<T> — triggers Nullable<T> operations
    // ──────────────────────────────────────────────
    public static int UnwrapNullable(int? value) => value ?? -1;
    public static bool HasNullableValue(int? value) => value.HasValue;

    // ──────────────────────────────────────────────
    // Using statement — triggers try-finally + Dispose pattern
    // ──────────────────────────────────────────────
    public static string ReadFileFirstLine(string path)
    {
        using var reader = new System.IO.StreamReader(path);
        return reader.ReadLine() ?? "";
    }

    // ──────────────────────────────────────────────
    // String.Split — exercises string BCL method calls
    // ──────────────────────────────────────────────
    public static string[] SplitComma(string input) => input.Split(',');

    // ──────────────────────────────────────────────
    // Nested try-catch-finally — exercises complete EH patterns
    // ──────────────────────────────────────────────
    public static int TryCatchFinally(int a, int b)
    {
        try
        {
            return a / b;
        }
        catch (System.DivideByZeroException)
        {
            return -1;
        }
        finally
        {
            // no-op
        }
    }

    // ──────────────────────────────────────────────
    // Object.GetHashCode + Equals + ToString — triggers Object shape entries
    // ──────────────────────────────────────────────
    public static int GetObjHash(object obj) => obj.GetHashCode();

    public static bool ObjEquals(object a, object b) => a.Equals(b);

    public static string ObjToString(object obj) => obj.ToString() ?? "";

    // ──────────────────────────────────────────────
    // Typeof — triggers Type.GetTypeFromHandle shape
    // ──────────────────────────────────────────────
    public static Type GetStringType() => typeof(string);

    // ──────────────────────────────────────────────
    // GC.KeepAlive + SuppressFinalize — triggers GC shapes
    // ──────────────────────────────────────────────
    public static void UseGcKeepAlive(object obj)
    {
        GC.KeepAlive(obj);
    }

    public static void UseGcSuppressFinalize(object obj)
    {
        GC.SuppressFinalize(obj);
    }

    // ──────────────────────────────────────────────
    // Environment.CurrentManagedThreadId — triggers Environment shape
    // ──────────────────────────────────────────────
    public static int GetCurrentThreadId() => Environment.CurrentManagedThreadId;

    // ──────────────────────────────────────────────
    // Int32.ToString + string.IsNullOrEmpty — triggers value type ToString + string shape
    // ──────────────────────────────────────────────
    public static string IntToString(int x) => x.ToString();

    public static bool IsNullOrEmpty(string s) => string.IsNullOrEmpty(s);

    // ──────────────────────────────────────────────
    // Array.Empty<T>() — triggers Array.Empty<T> generic static method
    // ──────────────────────────────────────────────
    public static bool UseArrayEmpty() => Array.Empty<int>().Length == 0;

    // ──────────────────────────────────────────────
    // lock statement — triggers Monitor.Enter/Exit shapes
    // ──────────────────────────────────────────────
    private static readonly object _lockObj = new();

    public static int LockAndIncrement()
    {
        lock (_lockObj)
        {
            return _counter++;
        }
    }

    // ──────────────────────────────────────────────
    // Large array literal — triggers RuntimeHelpers.InitializeArray
    // ──────────────────────────────────────────────
    public static int[] GetFiveElements() => new[] { 1, 2, 3, 4, 5 };

    // ──────────────────────────────────────────────
    // Math.Max + Min — triggers Math shape entries
    // ──────────────────────────────────────────────
    public static int UseMathMax(int a, int b) => Math.Max(a, b);
    public static int UseMathMin(int a, int b) => Math.Min(a, b);
    public static double UseMathAbs(double x) => Math.Abs(x);

    // ──────────────────────────────────────────────
    // Delegate creation + invocation — triggers delegate shapes
    // ──────────────────────────────────────────────
    public static int UseDelegate(Func<int, int> fn, int x) => fn(x);

    // ──────────────────────────────────────────────
    // DateTime.UtcNow + TimeSpan — triggers date/time shapes
    // ──────────────────────────────────────────────
    public static double GetAgeInHours(DateTime birth) =>
        (DateTime.UtcNow - birth).TotalHours;

    // ──────────────────────────────────────────────
    // Thread.Sleep + Thread.Yield — triggers Thread shapes
    // ──────────────────────────────────────────────
    public static void ShortSleep() => System.Threading.Thread.Sleep(1);
    public static bool YieldOnce() => System.Threading.Thread.Yield();

    // ──────────────────────────────────────────────
    // Console.Error — triggers Console shape
    // ──────────────────────────────────────────────
    public static void WriteError(string msg) =>
        System.Console.Error.WriteLine(msg);

    // ──────────────────────────────────────────────
    // String.Format — triggers string.Format shape resolution
    // ──────────────────────────────────────────────
    public static string UseStringFormat(string name, int age) =>
        string.Format("Name={0}, Age={1}", name, age);

    // ──────────────────────────────────────────────
    // Guid.NewGuid — triggers Guid shape + ToString
    // ──────────────────────────────────────────────
    public static string NewGuidToString() =>
        Guid.NewGuid().ToString();

    // ──────────────────────────────────────────────
    // TimeSpan.FromMinutes — triggers TimeSpan static method
    // ──────────────────────────────────────────────
    public static double UseTimeSpan(double minutes) =>
        TimeSpan.FromMinutes(minutes).TotalSeconds;

    // ──────────────────────────────────────────────
    // Process.Start — triggers Process shape
    // ──────────────────────────────────────────────
    public static bool FileExists(string path) =>
        System.IO.File.Exists(path);

    // ──────────────────────────────────────────────
    // StringBuilder usage — triggers StringBuilder shapes
    // ──────────────────────────────────────────────
    public static string BuildString(string a, string b, string c)
    {
        var sb = new System.Text.StringBuilder();
        sb.Append(a);
        sb.Append(b);
        sb.Append(c);
        return sb.ToString();
    }

    // ──────────────────────────────────────────────
    // String.Contains — triggers string.Contains
    // ──────────────────────────────────────────────
    public static bool StringContains(string input, string value) =>
        input.Contains(value);

    // ──────────────────────────────────────────────
    // Enum.TryParse — triggers Enum.TryParse shape
    // ──────────────────────────────────────────────
    public static bool TryParseDayName(string name) =>
        Enum.TryParse<DayOfWeek>(name, out _);

    // ──────────────────────────────────────────────
    // Activator.CreateInstance<T> — triggers activator shape
    // ──────────────────────────────────────────────
    public static T CreateInstance<T>() where T : new() =>
        new T();

    // ──────────────────────────────────────────────
    // HttpUtility.UrlEncode — triggers System.Web shape
    // ──────────────────────────────────────────────
    public static string EscapeDataString(string value) =>
        Uri.EscapeDataString(value);

    // ──────────────────────────────────────────────
    // Convert.ToInt32 — triggers Convert shape
    // ──────────────────────────────────────────────
    public static int StringToInt32(string s) =>
        Convert.ToInt32(s);

    // ──────────────────────────────────────────────
    // Nullable<T> explicit GetValueOrDefault
    // ──────────────────────────────────────────────
    public static int NullableGetValueOrDefault(int? value) =>
        value.GetValueOrDefault();

    // ──────────────────────────────────────────────
    // Path.Combine — triggers Path shape
    // ──────────────────────────────────────────────
    public static string CombinePaths(string a, string b) =>
        System.IO.Path.Combine(a, b);

    // ──────────────────────────────────────────────
    // String.StartsWith — triggers string.StartsWith
    // ──────────────────────────────────────────────
    public static bool StartsWithHello(string input) =>
        input.StartsWith("Hello");

    // ──────────────────────────────────────────────
    // Ldobj / Stobj via DateTime assignment
    // ──────────────────────────────────────────────
    public static long DateTimeToBinary(DateTime dt) =>
        dt.ToBinary();

    // ──────────────────────────────────────────────
    // Delegate multicast — triggers Combine/Remove
    // ──────────────────────────────────────────────
    public static int InvokeTwoDelegates(Func<int, int> a, Func<int, int> b, int x)
    {
        var combined = a + b;
        return combined(x);
    }

    // ──────────────────────────────────────────────
    // Constrained callvirt on value type — triggers constrained. IL prefix
    // ──────────────────────────────────────────────
    public static string CallToStringOnInt32(int x) => x.ToString();

    // ──────────────────────────────────────────────
    // Initobj — triggers initobj opcode
    // ──────────────────────────────────────────────
    public static System.DateTime GetDefaultDateTime() => default;

    // ──────────────────────────────────────────────
    // Sizeof — triggers sizeof opcode
    // ──────────────────────────────────────────────
    public static int SizeOfInt32() => sizeof(int);

    // ──────────────────────────────────────────────
    // Ldtoken (RuntimeFieldHandle) — triggers ldtoken opcode
    // ──────────────────────────────────────────────
    public static RuntimeFieldHandle GetStringEmptyFieldHandle() =>
        typeof(string).GetField(nameof(string.Empty))?.FieldHandle ?? default;

    // ──────────────────────────────────────────────
    // Ldtoken (RuntimeTypeHandle) — triggers ldtoken opcode
    // ──────────────────────────────────────────────
    public static RuntimeTypeHandle GetInt32TypeHandle() => typeof(int).TypeHandle;

    // ──────────────────────────────────────────────
    // Generic with interface constraint — triggers constrained callvirt on generic param
    // ──────────────────────────────────────────────
    public static string GenericToString<T>(T value) where T : ISpanFormattable =>
        value.ToString("G", null);

    // ──────────────────────────────────────────────
    // Volatile field access — triggers volatile. prefix
    // ──────────────────────────────────────────────
    private static volatile int _volatileCounter;
    public static int ReadVolatileCounter() => _volatileCounter;
    public static void IncrementVolatileCounter() => _volatileCounter++;

    // ──────────────────────────────────────────────
    // Complex method: exercises nested EH, generics, boxing, and interface dispatch
    // ──────────────────────────────────────────────
    public static string ComplexProcessing<T>(T? value, int mode) where T : struct, ISpanFormattable
    {
        try
        {
            if (value is null)
                return "null";

            try
            {
                return mode switch
                {
                    0 => value.Value.ToString("G", null),
                    1 => value.Value.ToString("X", null),
                    _ => value.Value.ToString("N", null),
                };
            }
            catch (FormatException)
            {
                return "format-error";
            }
        }
        finally
        {
            // no-op cleanup
        }
    }

    // ──────────────────────────────────────────────
    // Delegate creation and invocation — triggers delegate invoke path
    // ──────────────────────────────────────────────
    private static void DoNothing() { }
    public static void InvokeActionDelegate()
    {
        Action a = DoNothing;
        a();
    }

    // ──────────────────────────────────────────────
    // Func delegate — triggers generic delegate path
    // ──────────────────────────────────────────────
    public static int InvokeFuncDelegate()
    {
        Func<int, int> f = x => x + 1;
        return f(41);
    }

    // ──────────────────────────────────────────────
    // Simple lock statement — triggers Monitor.Enter/Exit path
    // ──────────────────────────────────────────────
    private static readonly object _counterLock = new();
    private static int _sharedCounter;
    public static int LockAndIncrementCounter2()
    {
        lock (_counterLock) { return _sharedCounter++; }
    }

    // ──────────────────────────────────────────────
    // Struct with string field — triggers struct marshalling descriptor
    // ──────────────────────────────────────────────
    public struct Person
    {
        public string Name;
        public int Age;
    }
    public static string DescribePerson()
    {
        var p = new Person { Name = "Alice", Age = 30 };
        return p.Name;
    }

    // ──────────────────────────────────────────────
    // Using statement (IDisposable) — triggers dispose pattern
    // ──────────────────────────────────────────────
    private sealed class Disposable : IDisposable
    {
        public int Value;
        public void Dispose() { Value = 99; }
    }
    public static int UseUsingStatement()
    {
        using var d = new Disposable();
        d.Value = 42;
        return d.Value;
    }

    // ──────────────────────────────────────────────
    // Dictionary<TKey,TValue> — triggers Dictionary shapes
    // ──────────────────────────────────────────────
    public static string UseDictionary(int key) =>
        new Dictionary<int, string> { { 1, "one" }, { 2, "two" } }.TryGetValue(key, out var v) ? v : "?";

    // ──────────────────────────────────────────────
    // HashSet<T> — triggers HashSet shapes
    // ──────────────────────────────────────────────
    public static bool UseHashSet(int value) =>
        new HashSet<int> { 1, 2, 3 }.Contains(value);

    // ──────────────────────────────────────────────
    // Lazy<T> — triggers Lazy shape
    // ──────────────────────────────────────────────
    public static int UseLazy() =>
        new Lazy<int>(() => 42).Value;

    // ──────────────────────────────────────────────
    // Tuple — triggers Tuple shapes
    // ──────────────────────────────────────────────
    public static int UseTuple(int a, int b) =>
        Tuple.Create(a, b).Item1;

    // ──────────────────────────────────────────────
    // ValueTuple — triggers ValueTuple shapes
    // ──────────────────────────────────────────────
    public static (int, int) UseValueTuple(int a, int b) =>
        (a, b);

    // ──────────────────────────────────────────────
    // IComparable<T> — triggers IComparable<T> shape
    // ──────────────────────────────────────────────
    public static int CompareInts(int a, int b) =>
        a.CompareTo(b);

    // ──────────────────────────────────────────────
    // IEquatable<T> — triggers IEquatable<T> shape
    // ──────────────────────────────────────────────
    public static bool IntsEqual(int a, int b) =>
        a.Equals(b);

    // ──────────────────────────────────────────────
    // MemoryStream — triggers Stream shapes
    // ──────────────────────────────────────────────
    public static byte[] UseMemoryStream(byte[] data)
    {
        using var ms = new MemoryStream(data);
        var result = new byte[data.Length];
        ms.Read(result, 0, data.Length);
        return result;
    }

    // ──────────────────────────────────────────────
    // Regex.IsMatch — triggers Regex shape
    // ──────────────────────────────────────────────
    public static bool UseRegex(string input) =>
        System.Text.RegularExpressions.Regex.IsMatch(input, @"^\d+$");

    // ──────────────────────────────────────────────
    // Enum.HasFlag — triggers Enum.HasFlag shape
    // ──────────────────────────────────────────────
    public static bool HasReadFlag(System.IO.FileAccess access) =>
        access.HasFlag(System.IO.FileAccess.Read);

    // ──────────────────────────────────────────────
    // Uri.TryCreate — triggers Uri shape
    // ──────────────────────────────────────────────
    public static bool UseUri(string url) =>
        Uri.TryCreate(url, UriKind.Absolute, out _);

    // ──────────────────────────────────────────────
    // DateTimeOffset — triggers DateTimeOffset shapes
    // ──────────────────────────────────────────────
    public static DateTimeOffset UseDateTimeOffset(DateTime dt) =>
        new DateTimeOffset(dt);

    // ──────────────────────────────────────────────
    // Stopwatch — triggers Stopwatch/Diagnostics shapes
    // ──────────────────────────────────────────────
    public static long UseStopwatch()
    {
        var sw = System.Diagnostics.Stopwatch.StartNew();
        return sw.ElapsedMilliseconds;
    }

    // ──────────────────────────────────────────────
    // WeakReference<T> — triggers WeakReference shape
    // ──────────────────────────────────────────────
    public static int UseWeakReference()
    {
        var obj = new object();
        var wr = new WeakReference<object>(obj);
        return wr.TryGetTarget(out _) ? 1 : 0;
    }
}
