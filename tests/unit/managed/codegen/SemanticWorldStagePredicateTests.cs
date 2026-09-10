using System.Reflection;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.SemanticWorld.Tests;

public sealed class SemanticWorldStagePredicateTests
{
    private static readonly Type s_stageType = typeof(SemanticWorldStage);
    private const BindingFlags s_flags = BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Public;

    private static T? InvokeStatic<T>(string methodName, params object?[] args)
    {
        var method = s_stageType.GetMethod(methodName, s_flags)
            ?? throw new InvalidOperationException($"Method '{methodName}' not found on SemanticWorldStage");
        return (T?)method.Invoke(null, args);
    }

    // ──────────────────────────────────────────────
    // IsDelegateInvokeCallee
    // Checks normalized.Contains("::Invoke(")
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.MulticastDelegate::Invoke()", true)]
    [InlineData("System.Private.CoreLib/System.Delegate::Invoke()", true)]
    [InlineData("System.Private.CoreLib/System.Action::Invoke()", true)]
    [InlineData("System.Private.CoreLib/System.Object::ToString", false)]
    [InlineData("", false)]
    public void IsDelegateInvokeCallee_VariousInputs(string input, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("IsDelegateInvokeCallee", input));
    }

    // ──────────────────────────────────────────────
    // ContainsAsyncSurfaceMarker
    // Checks subjectId.Contains("IAsyncStateMachine") || Contains("AsyncTaskMethodBuilder")
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Runtime.CompilerServices.IAsyncStateMachine", true)]
    [InlineData("SomeType.IAsyncStateMachine.SomeMethod", true)]
    [InlineData("System.Runtime.CompilerServices.AsyncTaskMethodBuilder", true)]
    [InlineData("System.Runtime.CompilerServices.AsyncTaskMethodBuilder<Task<bool>>", true)]
    [InlineData("System.Threading.Tasks.TaskAwaiter", false)]
    [InlineData("System.String", false)]
    [InlineData("", false)]
    public void ContainsAsyncSurfaceMarker_VariousInputs(string input, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("ContainsAsyncSurfaceMarker", input));
    }

    // ──────────────────────────────────────────────
    // ContainsTaskAwaiterMarker
    // Checks normalized.Contains("TaskAwaiter") || "::GetAwaiter(" || "::GetResult("
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Runtime.CompilerServices.TaskAwaiter", true)]
    [InlineData("System.Runtime.CompilerServices.ValueTaskAwaiter", true)]
    [InlineData("SomeType::GetAwaiter()", true)]
    [InlineData("SomeType::GetResult()", true)]
    [InlineData("System.Runtime.CompilerServices.AsyncTaskMethodBuilder", false)]
    [InlineData("System.String", false)]
    [InlineData("", false)]
    public void ContainsTaskAwaiterMarker_VariousInputs(string input, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("ContainsTaskAwaiterMarker", input));
    }

    // ──────────────────────────────────────────────
    // IsStringJoinEnumerableSurface
    // Matches constant with IEnumerable<System.String> (no backtick)
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Private.CoreLib/System.String::Join(System.String,System.Collections.Generic.IEnumerable<System.String>)", true)]
    [InlineData("System.Private.CoreLib/System.String::Join(System.String,System.Object[])", false)]
    [InlineData("System.Private.CoreLib/System.String::Concat(System.String,System.String)", false)]
    [InlineData("", false)]
    public void IsStringJoinEnumerableSurface_VariousInputs(string input, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("IsStringJoinEnumerableSurface", input));
    }

    // ──────────────────────────────────────────────
    // MethodSubjectIdEquals
    // Both sides normalized, then ordinal comparison
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("A.B::M", "A.B::M", true)]
    [InlineData("A.B::M()", "A.B::M()", true)]
    [InlineData("A.B::M:System.Void()", "A.B::M()", true)]
    [InlineData("A.B::M", "X.Y::Z", false)]
    public void MethodSubjectIdEquals_VariousInputs(string a, string b, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("MethodSubjectIdEquals", a, b));
    }

    // ──────────────────────────────────────────────
    // NormalizeMethodSubjectIdForMatching
    // Strips return type before '(' but keeps parens
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("A.B::M:System.Void()", "A.B::M()")]
    [InlineData("A.B::M:System.Int32(System.Int32)", "A.B::M(System.Int32)")]
    [InlineData("A.B::M", "A.B::M")]
    [InlineData("", "")]
    public void NormalizeMethodSubjectIdForMatching_VariousInputs(string input, string expected)
    {
        Assert.Equal(expected, InvokeStatic<string?>("NormalizeMethodSubjectIdForMatching", input));
    }

    // ──────────────────────────────────────────────
    // ContainsMonitorSurfaceMarker
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Threading.Monitor::Enter(", true)]
    [InlineData("System.Threading.Monitor::Exit(", true)]
    [InlineData("System.Threading.Monitor::TryEnter(", false)]
    [InlineData("System.Threading.Mutex", false)]
    [InlineData("", false)]
    public void ContainsMonitorSurfaceMarker_VariousInputs(string input, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("ContainsMonitorSurfaceMarker", input));
    }

    // ──────────────────────────────────────────────
    // ContainsUtf8MarshalSurfaceMarker
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8(", true)]
    [InlineData("System.Runtime.InteropServices.Marshal::PtrToStringUTF8(", true)]
    [InlineData("System.Runtime.InteropServices.Marshal::FreeCoTaskMem(", true)]
    [InlineData("System.Runtime.InteropServices.Marshal::PtrToStringAnsi(", false)]
    [InlineData("", false)]
    public void ContainsUtf8MarshalSurfaceMarker_VariousInputs(string input, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("ContainsUtf8MarshalSurfaceMarker", input));
    }

    // ──────────────────────────────────────────────
    // HasNestedExceptionHandlerShape
    // ──────────────────────────────────────────────

    [Fact]
    public void HasNestedExceptionHandlerShape_NullBlocks_ThrowsNullReferenceException()
    {
        // The implementation does not guard against null, so this throws via reflection
        var ex = Assert.Throws<TargetInvocationException>(() =>
            InvokeStatic<bool>("HasNestedExceptionHandlerShape", new object?[] { null }));
        Assert.IsType<NullReferenceException>(ex.InnerException);
    }

    [Fact]
    public void HasNestedExceptionHandlerShape_EmptyBlocks_ReturnsFalse()
    {
        // Must cast to object to avoid C# array covariance treating ManagedExceptionRegionModel[] as object?[]
        var empty = (object)Array.Empty<ManagedExceptionRegionModel>();
        Assert.False(InvokeStatic<bool>("HasNestedExceptionHandlerShape", empty));
    }

    [Fact]
    public void HasNestedExceptionHandlerShape_NonNested_ReturnsFalse()
    {
        var regions = new List<ManagedExceptionRegionModel>
        {
            new() { HandlingKind = "catch", TryOffset = 0, TryLength = 10, HandlerOffset = 20, HandlerLength = 5 },
            new() { HandlingKind = "catch", TryOffset = 30, TryLength = 10, HandlerOffset = 50, HandlerLength = 5 },
        };
        Assert.False(InvokeStatic<bool>("HasNestedExceptionHandlerShape", regions));
    }

    [Fact]
    public void HasNestedExceptionHandlerShape_Nested_ReturnsTrue()
    {
        var regions = new List<ManagedExceptionRegionModel>
        {
            new() { HandlingKind = "catch", TryOffset = 0, TryLength = 100, HandlerOffset = 100, HandlerLength = 10 },
            new() { HandlingKind = "finally", TryOffset = 10, TryLength = 20, HandlerOffset = 120, HandlerLength = 5 },
        };
        Assert.True(InvokeStatic<bool>("HasNestedExceptionHandlerShape", regions));
    }

    [Fact]
    public void HasNestedExceptionHandlerShape_CoincidentBounds_ReturnsFalse()
    {
        // identical try bounds should not count as nesting
        var regions = new List<ManagedExceptionRegionModel>
        {
            new() { HandlingKind = "catch", TryOffset = 0, TryLength = 50, HandlerOffset = 60, HandlerLength = 5 },
            new() { HandlingKind = "finally", TryOffset = 0, TryLength = 50, HandlerOffset = 70, HandlerLength = 5 },
        };
        Assert.False(InvokeStatic<bool>("HasNestedExceptionHandlerShape", regions));
    }
}
