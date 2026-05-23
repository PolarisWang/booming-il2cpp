// Family: generic/custom
// Assembly: CustomGenericsSubjects
// Description: Defines and exercises custom generic types to validate
// cross-module AOT entry generation (G6-1) and generic sharing.

using System;

namespace CustomGenerics {

// ── Custom generic types defined in this assembly ──────────────────────

// Simple generic class with one type parameter.
class Box<T> where T : notnull {
    private T _value;
    public Box(T value) { _value = value; }
    public T Get() => _value;
    public void Set(T value) { _value = value; }
    public bool IsDefault() => _value.Equals(default(T));
}

// Generic struct with two type parameters.
struct Pair<T1, T2> {
    public T1 First;
    public T2 Second;
    public Pair(T1 first, T2 second) { First = first; Second = second; }
    public void Deconstruct(out T1 first, out T2 second) {
        first = First; second = Second;
    }
}

// Non-generic class with generic methods.
class Util {
    public static T Echo<T>(T value) => value;
    public static bool Equal<T>(T a, T b) where T : IEquatable<T> => a.Equals(b);
    public static T CreateDefault<T>() where T : new() => new T();
}

// Generic class with constraint and base class call.
class Wrapper<T> where T : class {
    public T Ref;
    public Wrapper(T ref_) { Ref = ref_; }
    public Type GetRefType() => Ref.GetType();
}

}  // namespace CustomGenerics

public static partial class CustomGenericsSubjects {
    public static int _exitCode = 0;

    // ── Subject 1: Custom generic class with value type ────────────────
    public static void Subject_1() { try {
        var box = new CustomGenerics.Box<int>(42);
        int val = box.Get();
        if (val != 42) { _exitCode = 1; return; }
        box.Set(100);
        if (box.Get() != 100) { _exitCode = 1; return; }
        if (box.IsDefault()) { _exitCode = 1; return; }
    } catch { _exitCode = 1; }}

    // ── Subject 2: Custom generic class with reference type ────────────
    public static void Subject_2() { try {
        var box = new CustomGenerics.Box<string>("hello");
        if (box.Get() != "hello") { _exitCode = 1; return; }
        box.Set("world");
        if (box.Get() != "world") { _exitCode = 1; return; }
    } catch { _exitCode = 1; }}

    // ── Subject 3: Generic struct with two type parameters ─────────────
    public static void Subject_3() { try {
        var pair = new CustomGenerics.Pair<int, string>(1, "one");
        if (pair.First != 1) { _exitCode = 1; return; }
        if (pair.Second != "one") { _exitCode = 1; return; }
        pair.Deconstruct(out int f, out string s);
        if (f != 1 || s != "one") { _exitCode = 1; return; }
    } catch { _exitCode = 1; }}

    // ── Subject 4: Generic method on non-generic class ────────────────
    public static void Subject_4() { try {
        int result = CustomGenerics.Util.Echo<int>(42);
        if (result != 42) { _exitCode = 1; return; }
        string echoed = CustomGenerics.Util.Echo<string>("test");
        if (echoed != "test") { _exitCode = 1; return; }
    } catch { _exitCode = 1; }}

    // ── Subject 5: Generic method with constraint (IEquatable<T>) ──────
    public static void Subject_5() { try {
        bool eq = CustomGenerics.Util.Equal<int>(1, 1);
        if (!eq) { _exitCode = 1; return; }
        bool neq = CustomGenerics.Util.Equal<int>(1, 2);
        if (neq) { _exitCode = 1; return; }
    } catch { _exitCode = 1; }}

    // ── Subject 6: Generic class with class constraint ────────────────
    public static void Subject_6() { try {
        var w = new CustomGenerics.Wrapper<string>("data");
        if (w.Ref != "data") { _exitCode = 1; return; }
        Type t = w.GetRefType();
        if (t != typeof(string)) { _exitCode = 1; return; }
    } catch { _exitCode = 1; }}

    // ── Subject 7: Mixed generic usage ─────────────────────────────────
    public static void Subject_7() { try {
        var b1 = new CustomGenerics.Box<CustomGenerics.Pair<int, string>>(
            new CustomGenerics.Pair<int, string>(1, "a"));
        var b2 = new CustomGenerics.Box<CustomGenerics.Pair<int, string>>(
            new CustomGenerics.Pair<int, string>(2, "b"));
        var p1 = b1.Get();
        var p2 = b2.Get();
        if (p1.First != 1 || p2.Second != "b") { _exitCode = 1; return; }
    } catch { _exitCode = 1; }}
}
