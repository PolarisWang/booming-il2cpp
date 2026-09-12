using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;

namespace Chaos.IL2CPP.Tests.ReflectionSemanticContract;

/// <summary>
/// Reflection semantic contract — real assertions over the reflection APIs that
/// the coverage matrix (Phase 0) marked tier=<c>real</c>.
///
/// Unlike the ATG-generated <c>[UNVERIFIED]</c> smoke tests (which feed null /
/// uninitialized inputs and therefore only prove "does not crash"), this suite
/// feeds <b>valid</b> inputs and asserts the <b>semantic</b> result.
///
/// Scope: docs/dev/in-progress/reflection-production-readiness/api-coverage-matrix.json
///        entries where tier == "real" (26 APIs across 8 types).
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
        Console.WriteLine("═══ Reflection Semantic Contract ═══");
        Console.WriteLine($"runtime: {Environment.Version} / {RuntimeInformation.FrameworkDescription}");
        Console.WriteLine();

        // ── Fixture types used as reflection subjects ────────────────────
        var sample = new SampleSubject { Name = "chaos", Count = 7 };
        var sampleType = typeof(SampleSubject);

        // ── Assembly.* (9 real APIs) ─────────────────────────────────────
        Check("Assembly.GetExecutingAssembly non-null",
            () => Assembly.GetExecutingAssembly() is not null);

        Check("Assembly.GetExecutingAssembly == this assembly",
            () => Assembly.GetExecutingAssembly().GetName().Name == typeof(Program).Assembly.GetName().Name);

        Check("Assembly.GetTypes returns non-empty and contains SampleSubject",
            () => { var t = typeof(Program).Assembly.GetTypes(); return t.Length > 0 && t.Contains(sampleType); });

        Check("Assembly.GetType(name) resolves SampleSubject",
            () => typeof(Program).Assembly.GetType(sampleType.FullName!) == sampleType);

        Check("Assembly.GetType(unknown) returns null",
            () => typeof(Program).Assembly.GetType("No.Such.Type") is null);

        Check("Assembly.GetName().Name is non-empty",
            () => !string.IsNullOrEmpty(typeof(Program).Assembly.GetName().Name));

        Check("Assembly.GetManifestResourceNames returns array (may be empty)",
            () => typeof(Program).Assembly.GetManifestResourceNames() is not null);

        Check("Assembly.GetModules returns non-empty",
            () => typeof(Program).Assembly.GetModules().Length > 0);

        // ── Type / TypeInfo (via Assembly-resolved type) ─────────────────
        Check("Type.Namespace is the declared namespace",
            () => sampleType.Namespace == "Chaos.IL2CPP.Tests.ReflectionSemanticContract");

        Check("Type.Name is SampleSubject",
            () => sampleType.Name == nameof(SampleSubject));

        Check("Type.FullName == Namespace + '.' + Name",
            () => sampleType.FullName == $"{sampleType.Namespace}.{sampleType.Name}");

        Check("Type.BaseType is System.Object",
            () => sampleType.BaseType == typeof(object));

        Check("Type.IsClass is true",
            () => sampleType.IsClass);

        Check("Type.IsValueType is false for a class",
            () => !sampleType.IsValueType);

        Check("Type.IsEnum is true for an enum",
            () => typeof(SampleEnum).IsEnum);

        Check("Type.IsInterface is true for an interface",
            () => typeof(ISampleContract).IsInterface);

        Check("Type.IsAbstract is true for a static class",
            () => typeof(StaticHolder).IsAbstract && typeof(StaticHolder).IsSealed);

        Check("Type.GetField('Count') resolves to the declared field",
            () => sampleType.GetField(nameof(SampleSubject.Count)) is not null);

        Check("Type.GetMethod('Greet') resolves with parameter types",
            () => sampleType.GetMethod(nameof(SampleSubject.Greet), new[] { typeof(string) }) is not null);

        Check("Type.GetProperty('Name') resolves",
            () => sampleType.GetProperty(nameof(SampleSubject.Name)) is not null);

        Check("Type.GetConstructor(empty) resolves the default ctor",
            () => sampleType.GetConstructor(Type.EmptyTypes) is not null);

        Check("Type.GetInterfaces contains ISampleContract",
            () => sampleType.GetInterfaces().Contains(typeof(ISampleContract)));

        Check("Type.IsSubclassOf(object) is true",
            () => sampleType.IsSubclassOf(typeof(object)));

        Check("Type.IsAssignableFrom(base) is true",
            () => typeof(object).IsAssignableFrom(sampleType));

        Check("Type.IsInstanceOfType(instance) is true",
            () => sampleType.IsInstanceOfType(sample));

        // ── MemberInfo.* ─────────────────────────────────────────────────
        var nameProp = sampleType.GetProperty(nameof(SampleSubject.Name))!;
        var countField = sampleType.GetField(nameof(SampleSubject.Count))!;

        Check("MemberInfo.Name matches the declared member",
            () => nameProp.Name == nameof(SampleSubject.Name) && countField.Name == nameof(SampleSubject.Count));

        Check("MemberInfo.DeclaringType is SampleSubject",
            () => nameProp.DeclaringType == sampleType);

        Check("MemberInfo.ReflectedType is SampleSubject",
            () => nameProp.ReflectedType == sampleType);

        Check("MemberInfo.MetadataToken is non-zero",
            () => nameProp.MetadataToken != 0);

        // ── FieldInfo.* (GetValue / SetValue — 4 real APIs) ──────────────
        Check("FieldInfo.GetValue reads the instance field",
            () => (int)countField.GetValue(sample)! == 7);

        Check("FieldInfo.SetValue writes the instance field",
            () =>
            {
                countField.SetValue(sample, 99);
                return sample.Count == 99;
            });

        Check("FieldInfo.get_FieldType is int",
            () => countField.FieldType == typeof(int));

        Check("FieldInfo.IsStatic is true for a static field",
            () => typeof(StaticHolder).GetField(nameof(StaticHolder.Counter))!.IsStatic);

        Check("FieldInfo.IsInitOnly is true for a readonly field",
            () => sampleType.GetField(nameof(SampleSubject.ReadonlyTag))!.IsInitOnly);

        Check("FieldInfo.IsLiteral is true for a const field",
            () => sampleType.GetField(nameof(SampleSubject.ConstTag))!.IsLiteral);

        // ── Field modifier surface (Phase 3 additions) ───────────────────
        var publicField = sampleType.GetField(nameof(SampleSubject.Count))!;
        var privateField = sampleType.GetField("HiddenCount", BindingFlags.NonPublic | BindingFlags.Instance)!;

        Check("FieldInfo.IsPublic is true for a public field",
            () => publicField.IsPublic);

        Check("FieldInfo.IsPrivate is true for a private field",
            () => privateField.IsPrivate && !privateField.IsPublic);

        Check("FieldInfo.IsAssembly/IsFamily/IsFamilyOrAssembly are false on a public field",
            () => !publicField.IsAssembly && !publicField.IsFamily && !publicField.IsFamilyOrAssembly);

        Check("FieldInfo.IsNotSerialized is false by default",
            () => !publicField.IsNotSerialized);

        Check("FieldInfo.get_Attributes reports Public for a public field",
            () => (publicField.Attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Public);

        Check("FieldInfo.get_Attributes reports Private for a private field",
            () => (privateField.Attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Private);

        Check("FieldInfo.get_Attributes reports Static for a static field",
            () => (typeof(StaticHolder).GetField(nameof(StaticHolder.Counter))!.Attributes
                   & FieldAttributes.Static) != 0);

        Check("FieldInfo.get_Attributes reports InitOnly for a readonly field",
            () => (sampleType.GetField(nameof(SampleSubject.ReadonlyTag))!.Attributes
                   & FieldAttributes.InitOnly) != 0);

        Check("FieldInfo.get_Attributes reports Literal for a const field",
            () => (sampleType.GetField(nameof(SampleSubject.ConstTag))!.Attributes
                   & FieldAttributes.Literal) != 0);

        Check("FieldInfo.GetRawConstantValue returns the const value",
            () => (string)sampleType.GetField(nameof(SampleSubject.ConstTag))!.GetRawConstantValue()! == "const");

        // ── Method modifier surface (Phase 3 additions) ──────────────────
        var publicMethod = sampleType.GetMethod(nameof(SampleSubject.Greet))!;
        var privateMethod = sampleType.GetMethod("Hidden", BindingFlags.NonPublic | BindingFlags.Instance)!;
        var staticMethod = typeof(StaticHolder).GetMethod(nameof(StaticHolder.Bump))!;

        Check("MethodBase.IsPublic is true for a public method",
            () => publicMethod.IsPublic);

        Check("MethodBase.IsPrivate is true for a private method",
            () => privateMethod.IsPrivate && !privateMethod.IsPublic);

        Check("MethodBase.IsStatic is true for a static method",
            () => staticMethod.IsStatic && !publicMethod.IsStatic);

        Check("MethodBase.get_Attributes reports Public for a public method",
            () => (publicMethod.Attributes & MethodAttributes.MemberAccessMask) == MethodAttributes.Public);

        Check("MethodBase.get_Attributes reports Private for a private method",
            () => (privateMethod.Attributes & MethodAttributes.MemberAccessMask) == MethodAttributes.Private);

        Check("MethodBase.get_Attributes reports Static for a static method",
            () => (staticMethod.Attributes & MethodAttributes.Static) != 0);

        Check("MethodBase.IsConstructor is true for a constructor",
            () => sampleType.GetConstructor(Type.EmptyTypes)!.IsConstructor);

        Check("MethodBase.get_MethodImplementationFlags is readable",
            () => publicMethod.GetMethodImplementationFlags() is MethodImplAttributes);

        Check("MethodInfo.GetBaseDefinition returns the method itself for a non-override",
            () => publicMethod.GetBaseDefinition() == publicMethod);

        // ── PropertyInfo.* ───────────────────────────────────────────────
        Check("PropertyInfo.get_PropertyType is string",
            () => nameProp.PropertyType == typeof(string));

        Check("PropertyInfo.CanRead is true",
            () => nameProp.CanRead);

        Check("PropertyInfo.CanWrite is true",
            () => nameProp.CanWrite);

        Check("PropertyInfo.GetValue reads through the getter",
            () => (string)nameProp.GetValue(sample)! == "chaos");

        Check("PropertyInfo.SetValue writes through the setter",
            () =>
            {
                nameProp.SetValue(sample, "updated");
                return sample.Name == "updated";
            });

        // ── MethodBase / MethodInfo ──────────────────────────────────────
        var greet = sampleType.GetMethod(nameof(SampleSubject.Greet), new[] { typeof(string) })!;

        Check("MethodBase.GetParameters returns the declared parameters",
            () =>
            {
                var ps = greet.GetParameters();
                return ps.Length == 1 && ps[0].ParameterType == typeof(string);
            });

        Check("MethodBase.Invoke calls the method and returns the result",
            () => (string)greet.Invoke(sample, new object[] { "world" })! == "hello world");

        Check("MethodInfo.MakeGenericMethod constructs a closed generic method",
            () =>
            {
                var open = typeof(SampleSubject).GetMethod(nameof(SampleSubject.Identity))!;
                var closed = open.MakeGenericMethod(typeof(int));
                return closed.GetGenericArguments()[0] == typeof(int);
            });

        // ── ParameterInfo.* ──────────────────────────────────────────────
        var p0 = greet.GetParameters()[0];

        Check("ParameterInfo.Name matches the declared parameter",
            () => p0.Name == "who");

        Check("ParameterInfo.Position is 0",
            () => p0.Position == 0);

        Check("ParameterInfo.ParameterType is string",
            () => p0.ParameterType == typeof(string));

        // ── Activator.CreateInstance ─────────────────────────────────────
        Check("Activator.CreateInstance constructs SampleSubject",
            () => Activator.CreateInstance(sampleType) is SampleSubject);

        Check("Activator.CreateInstance with ctor args sets fields",
            () =>
            {
                var created = (SampleSubject?)Activator.CreateInstance(
                    sampleType, new object[] { "from-ctor" });
                return created is not null && created.Name == "from-ctor";
            });

        // ── Module.* ─────────────────────────────────────────────────────
        var mod = typeof(Program).Assembly.GetModules()[0];

        Check("Module.GetType resolves SampleSubject",
            () => mod.GetType(sampleType.FullName!) == sampleType);

        Check("Module.GetTypes contains SampleSubject",
            () => mod.GetTypes().Contains(sampleType));

        Check("Module.Name is non-empty",
            () => !string.IsNullOrEmpty(mod.Name));

        // ── CustomAttributeData ──────────────────────────────────────────
        Check("MemberInfo.GetCustomAttributesData returns attribute data",
            () => sampleType.GetCustomAttributesData() is not null);

        Report();
        return _failed == 0 ? 0 : 1;
    }

    // ── Assertion plumbing ───────────────────────────────────────────────

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

// ── Fixture types ────────────────────────────────────────────────────────

public interface ISampleContract
{
    string Name { get; }
}

public sealed class SampleSubject : ISampleContract
{
    public const string ConstTag = "const";
    public static readonly string ReadonlyTag = "readonly";

    public SampleSubject() { }
    public SampleSubject(string name) { Name = name; }

    public string Name { get; set; } = "";
    public int Count;
    private int HiddenCount;
    private void Hidden() { }

    public string Greet(string who) => $"hello {who}";

    public T Identity<T>(T value) => value;
}

public enum SampleEnum { Alpha, Beta, Gamma }

public static class StaticHolder
{
    public static int Counter;
    public static void Bump() => Counter++;
}

file static class RuntimeInformation
{
    public static string FrameworkDescription => System.Runtime.InteropServices.RuntimeInformation.FrameworkDescription;
}
