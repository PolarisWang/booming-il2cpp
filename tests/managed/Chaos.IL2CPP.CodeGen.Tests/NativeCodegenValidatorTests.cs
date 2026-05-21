using Chaos.IL2CPP.CodeGen.Validation;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

public sealed class NativeCodegenValidatorTests
{
    private readonly NativeCodegenValidator _validator = new();

    // ──────────────────────────────────────────────
    // Helper
    // ──────────────────────────────────────────────

    private NativeCodegenValidator.ValidationResult Validate(string content, string fileName)
    {
        return _validator.ValidateContent(content, fileName);
    }

    // ──────────────────────────────────────────────
    // Rule 1: No `using namespace std;` or `using namespace chaos;`
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule1_UsingNamespaceStd_ReturnsError()
    {
        var result = Validate("using namespace std;", "test.cpp");
        Assert.Contains(result.Errors, e => e.Contains("using namespace std/chaos"));
    }

    [Fact]
    public void Rule1_UsingNamespaceChaos_ReturnsError()
    {
        var result = Validate("using namespace chaos;", "test.cpp");
        Assert.Contains(result.Errors, e => e.Contains("using namespace std/chaos"));
    }

    [Fact]
    public void Rule1_UsingNamespaceWithExtraWhitespace_ReturnsError()
    {
        var result = Validate("using  namespace  std  ;", "test.cpp");
        Assert.Contains(result.Errors, e => e.Contains("using namespace std/chaos"));
    }

    [Fact]
    public void Rule1_ExplicitStdPrefix_Passes()
    {
        const string code = @"
#include <chaos/common.h>
std::string_view name;
std::int32_t counter;
";
        var result = Validate(code, "test.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("using namespace std/chaos"));
    }

    [Fact]
    public void Rule1_NamespaceBlock_Passes()
    {
        const string code = @"
namespace chaos::il2cpp::runtime_core {
    void Foo();
}
";
        var result = Validate(code, "test.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("using namespace std/chaos"));
    }

    // ──────────────────────────────────────────────
    // Rule 2: No `using std::` or `using chaos::` import directives
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule2_UsingNamespaceDirectiveOnOwnLine_ReturnsError()
    {
        var result = Validate("using namespace std;\n", "test.cpp");
        Assert.Contains(result.Errors, e => e.Contains("using std::"));
    }

    [Fact]
    public void Rule2_UsingNamespaceChaosDirective_ReturnsError()
    {
        var result = Validate("using namespace chaos;\n", "test.cpp");
        Assert.Contains(result.Errors, e => e.Contains("using chaos::"));
    }

    [Fact]
    public void Rule2_InlineUsingInMiddleOfLine_MayPassDependingOnAnchoring()
    {
        // UsingDirectiveRegex is anchored with ^, so inline mid-line should not trigger.
        var result = Validate("int x = 1; using namespace std; // bad", "test.cpp");
        // Rule 1 (non-anchored) will catch it, but rule 2 (anchored) will not
        var rule2Errors = result.Errors.Where(e => e.Contains("using std::") || e.Contains("using chaos::"));
        Assert.Empty(rule2Errors);
    }

    [Fact]
    public void Rule2_UsingDeclarationsInCode_Passes()
    {
        const string code = "std::string name;\nstd::int32_t value;\n";
        var result = Validate(code, "test.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("using std::") || e.Contains("using chaos::"));
    }

    // ──────────────────────────────────────────────
    // Rule 3: Must use nullptr, not NULL
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule3_NullLiteral_ReturnsError()
    {
        var result = Validate("void* p = NULL;", "test.cpp");
        Assert.Contains(result.Errors, e => e.Contains("NULL"));
    }

    [Fact]
    public void Rule3_NullInFunctionCall_ReturnsError()
    {
        var result = Validate("DoSomething(NULL);", "test.cpp");
        Assert.Contains(result.Errors, e => e.Contains("NULL"));
    }

    [Fact]
    public void Rule3_Nullptr_Passes()
    {
        var result = Validate("void* p = nullptr;", "test.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("NULL"));
    }

    [Fact]
    public void Rule3_NullptrInFunctionCall_Passes()
    {
        var result = Validate("DoSomething(nullptr);", "test.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("NULL"));
    }

    [Fact]
    public void Rule3_MacroNullptr_Passes()
    {
        // NULLPTR macro should not trigger \bNULL\b
        var result = Validate("#define NULLPTR nullptr\nvoid* p = NULLPTR;", "test.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("NULL"));
    }

    // ──────────────────────────────────────────────
    // Rule 4: No bare int/long types (warning in generated code)
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule4_BareInt_FiresWarning()
    {
        var result = Validate("int counter = 0;", "generated.cpp");
        Assert.Contains(result.Warnings, w => w.Contains("CHAOS_IL2CPP_INT32"));
    }

    [Fact]
    public void Rule4_BareLong_FiresWarning()
    {
        var result = Validate("long offset = 0L;", "generated.cpp");
        Assert.Contains(result.Warnings, w => w.Contains("CHAOS_IL2CPP"));
    }

    [Fact]
    public void Rule4_UnsignedInt_FiresWarning()
    {
        var result = Validate("unsigned int flags = 0u;", "generated.cpp");
        Assert.Contains(result.Warnings, w => w.Contains("CHAOS_IL2CPP"));
    }

    [Fact]
    public void Rule4_UnsignedLong_FiresWarning()
    {
        var result = Validate("unsigned long mask = 0UL;", "generated.cpp");
        Assert.Contains(result.Warnings, w => w.Contains("CHAOS_IL2CPP"));
    }

    [Fact]
    public void Rule4_IntInStructMember_FiresWarning()
    {
        var result = Validate("struct Foo { int x; int y; };", "generated.cpp");
        Assert.Contains(result.Warnings, w => w.Contains("CHAOS_IL2CPP_INT32"));
    }

    [Fact]
    public void Rule4_StdIntTypes_NoWarning()
    {
        var result = Validate("std::int32_t counter = 0;\nstd::uint64_t mask = 0UL;", "generated.cpp");
        Assert.Empty(result.Warnings);
    }

    [Fact]
    public void Rule4_IntInChaosHeader_NoWarning()
    {
        // Rule 4 is suppressed for files starting with "chaos/"
        var result = Validate("int internal_counter;", "chaos/runtime_core.h");
        Assert.Empty(result.Warnings);
    }

    [Fact]
    public void Rule4_IntInVariableNameContainingUnderscore_NoWarning()
    {
        // The regex has (?!\w)(?!\s*_) to avoid matching `int_` prefix patterns
        var result = Validate("int_parse_error;", "generated.cpp");
        Assert.Empty(result.Warnings);
    }

    // ──────────────────────────────────────────────
    // Rule 5: Include guard format for .h files
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule5_HeaderMissingIncludeGuard_FiresWarning()
    {
        const string header = "#pragma once\nvoid Foo();\n";
        var result = Validate(header, "my_types.h");
        Assert.Contains(result.Warnings, w => w.Contains("include guard"));
    }

    [Fact]
    public void Rule5_HeaderWithCorrectIncludeGuard_Passes()
    {
        const string header = "#ifndef CHAOS_IL2CPP_MY_TYPES_H_\n#define CHAOS_IL2CPP_MY_TYPES_H_\nvoid Foo();\n#endif\n";
        var result = Validate(header, "my_types.h");
        Assert.DoesNotContain(result.Warnings, w => w.Contains("include guard"));
    }

    [Fact]
    public void Rule5_HeaderWithExtraSpacesInGuard_Fails()
    {
        // The regex expects exactly one \s+ between #ifndef and CHAOS_IL2CPP_...
        const string header = "#ifndef  CHAOS_IL2CPP_MY_TYPES_H_\n#define CHAOS_IL2CPP_MY_TYPES_H_\n#endif\n";
        var result = Validate(header, "my_types.h");
        // \s+ matches one or more spaces, so double space should still match
        Assert.DoesNotContain(result.Warnings, w => w.Contains("include guard"));
    }

    [Fact]
    public void Rule5_ChaosHeaderWithoutGuard_NoWarning()
    {
        // Headers under chaos/ are exempt
        var result = Validate("#pragma once\nvoid Foo();\n", "chaos/common.h");
        Assert.DoesNotContain(result.Warnings, w => w.Contains("include guard"));
    }

    [Fact]
    public void Rule5_CppFileWithoutGuard_NoWarning()
    {
        // Include guard rule only applies to .h files
        var result = Validate("int main() {}", "test.cpp");
        Assert.DoesNotContain(result.Warnings, w => w.Contains("include guard"));
    }

    [Fact]
    public void Rule5_GuardOnLaterLine_Fails()
    {
        // Guard must be on the very first line
        const string header = "\n#ifndef CHAOS_IL2CPP_FOO_H_\n#define CHAOS_IL2CPP_FOO_H_\n#endif\n";
        var result = Validate(header, "foo.h");
        Assert.Contains(result.Warnings, w => w.Contains("include guard"));
    }

    // ──────────────────────────────────────────────
    // Rule 6: ChaosIl2cpp::Common symbols must not use chaos_ prefix
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule6_ChaosPrefixInChaosIl2cppCommonNamespace_ReturnsError()
    {
        const string code = @"
namespace ChaosIl2cpp::Common {
    class chaos_string_utils { };
}
";
        var result = Validate(code, "test.cpp");
        Assert.Contains(result.Errors, e => e.Contains("chaos_"));
    }

    [Fact]
    public void Rule6_NoChaosPrefixInCommonNamespace_Passes()
    {
        const string code = @"
namespace ChaosIl2cpp::Common {
    class string_utils { };
}
";
        var result = Validate(code, "test.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("chaos_"));
    }

    [Fact]
    public void Rule6_ChaosPrefixOutsideCommonNamespace_NoCheck()
    {
        // Rule 6 only activates when content contains "namespace ChaosIl2cpp::Common"
        const string code = "void chaos_helper() { }";
        var result = Validate(code, "test.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("chaos_"));
    }

    // ──────────────────────────────────────────────
    // Rule 7: Generated .cpp must include <chaos/common.h>, not inline preludes
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule7_InlinePreludeChaosFinallyScopeGuard_ReturnsError()
    {
        const string code = @"
#include <cstdint>
chaos_finally_scope_guard guard;
";
        var result = Validate(code, "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("inline prelude"));
    }

    [Fact]
    public void Rule7_InlinePreludeChaosWrapAdd_ReturnsError()
    {
        const string code = @"
#include <cstdint>
auto wrapper = chaos_wrap_add(x, y);
";
        var result = Validate(code, "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("inline prelude"));
    }

    [Fact]
    public void Rule7_IncludesChaosCommon_Passes()
    {
        const string code = @"
#include <chaos/common.h>
void Foo() { }
";
        var result = Validate(code, "generated.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("inline prelude"));
    }

    [Fact]
    public void Rule7_ChaosFileExempt_NoCheck()
    {
        // Files with "chaos/" in the name are exempt from rule 7
        const string code = "chaos_finally_scope_guard guard;";
        var result = Validate(code, "chaos/generated.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("inline prelude"));
    }

    [Fact]
    public void Rule7_HeaderFileExempt_NoCheck()
    {
        // Rule 7 only applies to .cpp files
        const string code = "chaos_finally_scope_guard guard;";
        var result = Validate(code, "generated.h");
        Assert.DoesNotContain(result.Errors, e => e.Contains("inline prelude"));
    }

    // ──────────────────────────────────────────────
    // Rule 8: No raw std:: types — use CHAOS_IL2CPP_* macros
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule8_RawStdInt32_ReturnsError()
    {
        var result = Validate("std::int32_t x = 0;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_RawStdString_ReturnsError()
    {
        var result = Validate("std::string name;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_RawStdVector_ReturnsError()
    {
        var result = Validate("std::vector<int> items;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_RawStdMalloc_ReturnsError()
    {
        var result = Validate("auto p = std::malloc(64);", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_RawStdMemset_ReturnsError()
    {
        var result = Validate("std::memset(buf, 0, size);", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_RawStdUniquePtr_ReturnsError()
    {
        var result = Validate("auto p = std::make_unique<int>(42);", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_RawStdUnorderedMap_ReturnsError()
    {
        var result = Validate("std::unordered_map<int,int> m;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_ChaosMacro_Passes()
    {
        const string code = @"
CHAOS_IL2CPP_INT32 x = 0;
CHAOS_IL2CPP_STRING name;
CHAOS_IL2CPP_VECTOR items;
";
        var result = Validate(code, "generated.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_ChaosHeaderExempt_NoCheck()
    {
        var result = Validate("std::string name;", "chaos/profile.h");
        Assert.DoesNotContain(result.Errors, e => e.Contains("raw std:: type"));
    }

    [Fact]
    public void Rule8_NativeTypesExempt_NoCheck()
    {
        var result = Validate("std::int32_t x;", "native_types_generated.h");
        Assert.DoesNotContain(result.Errors, e => e.Contains("raw std:: type"));
    }

    // ──────────────────────────────────────────────
    // Rule 9: No bare <cstdint> types — use CHAOS_IL2CPP_* macros
    // ──────────────────────────────────────────────

    [Fact]
    public void Rule9_BareInt32T_ReturnsError()
    {
        var result = Validate("int32_t x = 0;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_BareUint64T_ReturnsError()
    {
        var result = Validate("uint64_t mask = 0UL;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_BareSizeT_ReturnsError()
    {
        var result = Validate("size_t len = 0;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_BareIntPtrT_ReturnsError()
    {
        var result = Validate("intptr_t addr;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_BarePtrdiffT_ReturnsError()
    {
        var result = Validate("ptrdiff_t diff;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_ChaosMacro_Passes()
    {
        const string code = "CHAOS_IL2CPP_INT32 x = 0;\nCHAOS_IL2CPP_UINT64 mask = 0UL;\n";
        var result = Validate(code, "generated.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_BareTypeInStdPrefixedContext_IsStillCaught()
    {
        // The regex matches the bare type name even when preceded by std::
        // because (?<!\w) is satisfied by the ':' character. Both Rule 8 and
        // Rule 9 fire for "std::int32_t" -- that is expected overlap.
        var result = Validate("std::int32_t x = 0;", "generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_StdString_NoError()
    {
        // std::string is caught by Rule 8 (raw std:: type), but the bare type
        // regex in Rule 9 only checks for cstdint types, not string.
        var result = Validate("std::string name;", "generated.cpp");
        Assert.DoesNotContain(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_ChaosHeaderExempt_NoCheck()
    {
        var result = Validate("int32_t x;", "chaos/types.h");
        Assert.DoesNotContain(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void Rule9_NativeTypesExempt_NoCheck()
    {
        var result = Validate("int32_t x;", "native_types_map.h");
        Assert.DoesNotContain(result.Errors, e => e.Contains("bare type"));
    }

    // ──────────────────────────────────────────────
    // Combined / clean-pass tests
    // ──────────────────────────────────────────────

    [Fact]
    public void ValidCppCode_PassesAllChecks()
    {
        const string code = @"
#include <chaos/common.h>

namespace chaos::il2cpp::runtime_core {
    CHAOS_IL2CPP_INT32 g_counter = 0;

    void Foo(CHAOS_IL2CPP_STRING name) {
        void* ptr = nullptr;
        CHAOS_IL2CPP_INT32 len = 0;
    }
}
";
        var result = Validate(code, "generated.cpp");
        Assert.True(result.IsValid, string.Join("; ", result.Errors));
    }

    [Fact]
    public void ValidHeaderFile_PassesAllChecks()
    {
        const string code = @"
#ifndef CHAOS_IL2CPP_MY_HEADER_H_
#define CHAOS_IL2CPP_MY_HEADER_H_

#include <chaos/common.h>
#include <cstdint>

namespace chaos::il2cpp::support {
    CHAOS_IL2CPP_INT32 GetValue();
    void SetValue(CHAOS_IL2CPP_INT32 v);
}

#endif
";
        var result = Validate(code, "my_header.h");
        Assert.True(result.IsValid, string.Join("; ", result.Errors));
    }

    // ──────────────────────────────────────────────
    // EstimateLineNumber implicit tests
    // ──────────────────────────────────────────────

    [Fact]
    public void ErrorMessage_IncludesLineNumber()
    {
        const string code = "int x = 1;\nint y = 2;\nint z = NULL;\n";
        var result = Validate(code, "test.cpp");
        var error = Assert.Single(result.Errors);
        // The NULL on line 3 should report line 3
        Assert.Contains("line 3", error);
    }

    [Fact]
    public void ErrorMessage_IncludesFileName()
    {
        var result = Validate("void* p = NULL;", "my_generated.cpp");
        Assert.Contains(result.Errors, e => e.Contains("my_generated.cpp"));
    }

    [Fact]
    public void WarningMessage_IncludesFileName()
    {
        var result = Validate("int bare_int;", "my_generated.cpp");
        Assert.Contains(result.Warnings, w => w.Contains("my_generated.cpp"));
    }

    [Fact]
    public void MultipleErrors_AllCollected()
    {
        const string code = @"
using namespace std;
void* p = NULL;
std::vector<int> v;
int32_t x;
";
        var result = Validate(code, "bad_output.cpp");
        Assert.False(result.IsValid);
        Assert.Contains(result.Errors, e => e.Contains("using namespace std/chaos"));
        Assert.Contains(result.Errors, e => e.Contains("NULL"));
        Assert.Contains(result.Errors, e => e.Contains("raw std:: type"));
        Assert.Contains(result.Errors, e => e.Contains("bare type"));
    }

    [Fact]
    public void ValidationResult_IsValid_WhenNoErrors()
    {
        var result = Validate("void f() {}", "clean.cpp");
        Assert.True(result.IsValid);
        Assert.Empty(result.Errors);
    }

    // ──────────────────────────────────────────────
    // ValidateFile (file-path overload)
    // ──────────────────────────────────────────────

    [Fact]
    public void ValidateFile_NonExistent_ReturnsError()
    {
        var stderr = new StringWriter();
        Console.SetError(stderr);
        var result = _validator.ValidateFile("nonexistent_file_for_test.cpp");
        Assert.False(result.IsValid);
        Assert.Contains(result.Errors, e => e.Contains("File not found"));
    }

    [Fact]
    public void ValidateFile_ValidFile_Passes()
    {
        var path = Path.GetTempFileName();
        try
        {
            File.WriteAllText(path, "void f() { }\n");
            var result = _validator.ValidateFile(path);
            Assert.True(result.IsValid, string.Join("; ", result.Errors));
        }
        finally
        {
            File.Delete(path);
        }
    }

    [Fact]
    public void ValidateFile_WithViolations_ReturnsErrors()
    {
        var path = Path.GetTempFileName();
        try
        {
            File.WriteAllText(path, "using namespace std;\nvoid* p = NULL;\n");
            var result = _validator.ValidateFile(path);
            Assert.False(result.IsValid);
            Assert.NotEmpty(result.Errors);
        }
        finally
        {
            File.Delete(path);
        }
    }

    // ──────────────────────────────────────────────
    // ValidateGeneratedFile (static convenience method)
    // ──────────────────────────────────────────────

    [Fact]
    public void ValidateGeneratedFile_NonExistent_ReturnsFalse()
    {
        var errors = new List<string>();
        var result = NativeCodegenValidator.ValidateGeneratedFile(
            "nonexistent_file_for_test.cpp", errors);
        Assert.False(result);
        Assert.NotEmpty(errors);
    }

    [Fact]
    public void ValidateGeneratedFile_ValidFile_ReturnsTrue()
    {
        var path = Path.GetTempFileName();
        try
        {
            File.WriteAllText(path, "void f() { }\n");
            var errors = new List<string>();
            var result = NativeCodegenValidator.ValidateGeneratedFile(path, errors);
            Assert.True(result);
        }
        finally
        {
            File.Delete(path);
        }
    }

    [Fact]
    public void ValidateGeneratedFile_NullErrorsList_DoesNotThrow()
    {
        var path = Path.GetTempFileName();
        try
        {
            File.WriteAllText(path, "void f() { }\n");
            var result = NativeCodegenValidator.ValidateGeneratedFile(path, null);
            Assert.True(result);
        }
        finally
        {
            File.Delete(path);
        }
    }
}
