// Handwritten custom entries for the custom-generics family.
// Provides CustomGenerics type definitions (Box, Pair, Util, Wrapper)
// and Subject_N method implementations for cross-module generic AOT
// entry verification (G6-1).
//
// All 6 contract methods are marked as custom entries because the
// auto-generator cannot produce valid stubs for generic types.
// See capability-family-contract.json customEntryIndices.

using System;

namespace CustomGenerics {

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
}

// Generic class with class constraint.
class Wrapper<T> where T : class {
    public T Ref;
    public Wrapper(T ref_) { Ref = ref_; }
    public Type GetRefType() => Ref.GetType();
}

}  // namespace CustomGenerics

public static partial class CustomGenericsSubjects {

    // [0] CustomGenerics.Box`1::Get:T()  — value type
    public static void Subject_0() {
        try {
            var box = new CustomGenerics.Box<int>(42);
            if (box.Get() != 42) { _exitCode = 1; }
        } catch { _exitCode = 1; }
    }

    // [1] CustomGenerics.Box`1::Set:(T)  — value type
    public static void Subject_1() {
        try {
            var box = new CustomGenerics.Box<int>(42);
            box.Set(100);
            if (box.Get() != 100) { _exitCode = 1; }
        } catch { _exitCode = 1; }
    }

    // [2] CustomGenerics.Pair`2::Deconstruct:(T1&,T2&)
    public static void Subject_2() {
        try {
            var pair = new CustomGenerics.Pair<int, string>(1, "one");
            pair.Deconstruct(out int f, out string s);
            if (f != 1 || s != "one") { _exitCode = 1; }
        } catch { _exitCode = 1; }
    }

    // [3] CustomGenerics.Util::Echo:T(T)
    public static void Subject_3() {
        try {
            int result = CustomGenerics.Util.Echo<int>(42);
            if (result != 42) { _exitCode = 1; }
        } catch { _exitCode = 1; }
    }

    // [4] CustomGenerics.Util::Equal:bool(T,T)
    public static void Subject_4() {
        try {
            bool eq = CustomGenerics.Util.Equal<int>(1, 1);
            if (!eq) { _exitCode = 1; }
            bool neq = CustomGenerics.Util.Equal<int>(1, 2);
            if (neq) { _exitCode = 1; }
        } catch { _exitCode = 1; }
    }

    // [5] CustomGenerics.Wrapper`1::GetRefType:System.Type()
    public static void Subject_5() {
        try {
            var w = new CustomGenerics.Wrapper<string>("data");
            Type t = w.GetRefType();
            if (t != typeof(string)) { _exitCode = 1; }
        } catch { _exitCode = 1; }
    }
}
