using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
using Xunit;

[CapabilityTest("family/System.Private.CoreLib/reflection/binding/proof")]
public partial class ReflectionBindingFlagsTests
{
    // === R4.1 BindingFlags proof: verify BindingFlags filtering does not crash ===

    [Fact]
    public void _ReflectionBindingFlags_GetFieldsPublic()
    {
        // Purpose: BindingFlags Public | Instance on GetFields does not throw.
        var flags = BindingFlags.Public | BindingFlags.Instance;
        var fields = typeof(string).GetFields(flags);
        Assert.NotNull((object)fields);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetFieldsStatic()
    {
        // Purpose: BindingFlags Static | Public on GetFields does not throw.
        var flags = BindingFlags.Static | BindingFlags.Public;
        var fields = typeof(Environment).GetFields(flags);
        Assert.NotNull((object)fields);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetFieldsNonPublic()
    {
        // Purpose: BindingFlags NonPublic | Instance on GetFields does not throw.
        var flags = BindingFlags.NonPublic | BindingFlags.Instance;
        var fields = typeof(string).GetFields(flags);
        Assert.NotNull((object)fields);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetFieldsDeclaredOnly()
    {
        // Purpose: BindingFlags DeclaredOnly on GetFields does not throw.
        var flags = BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly;
        var fields = typeof(string).GetFields(flags);
        Assert.NotNull((object)fields);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetMethodsPublic()
    {
        // Purpose: BindingFlags Public | Instance on GetMethods does not throw.
        var flags = BindingFlags.Public | BindingFlags.Instance;
        var methods = typeof(Environment).GetMethods(flags);
        Assert.NotNull((object)methods);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetMethodsStatic()
    {
        // Purpose: BindingFlags Static | Public on GetMethods does not throw.
        var flags = BindingFlags.Static | BindingFlags.Public;
        var methods = typeof(Environment).GetMethods(flags);
        Assert.NotNull((object)methods);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetPropertiesInstance()
    {
        // Purpose: BindingFlags Instance | Public on GetProperties does not throw.
        var flags = BindingFlags.Instance | BindingFlags.Public;
        var props = typeof(string).GetProperties(flags);
        Assert.NotNull((object)props);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetPropertiesStatic()
    {
        // Purpose: BindingFlags Static | Public on GetProperties does not throw.
        var flags = BindingFlags.Static | BindingFlags.Public;
        var props = typeof(Environment).GetProperties(flags);
        Assert.NotNull((object)props);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetConstructorsPublic()
    {
        // Purpose: BindingFlags Public | Instance on GetConstructors does not throw.
        var flags = BindingFlags.Public | BindingFlags.Instance;
        var ctors = typeof(string).GetConstructors(flags);
        Assert.NotNull((object)ctors);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetConstructorsStatic()
    {
        // Purpose: BindingFlags Static | Public | Instance on GetConstructors does not throw.
        var flags = BindingFlags.Static | BindingFlags.Public | BindingFlags.Instance;
        var ctors = typeof(Math).GetConstructors(flags);
        Assert.NotNull((object)ctors);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetNestedTypesPublic()
    {
        // Purpose: BindingFlags Public on GetNestedTypes does not throw.
        var flags = BindingFlags.Public;
        var nested = typeof(Environment).GetNestedTypes(flags);
        Assert.NotNull((object)nested);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetMembersAll()
    {
        // Purpose: BindingFlags Public | NonPublic | Instance | Static on GetMembers does not throw.
        var flags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static;
        var members = typeof(string).GetMembers(flags);
        Assert.NotNull((object)members);
    }

    [Fact]
    public void _ReflectionBindingFlags_FlattenHierarchy()
    {
        // Purpose: BindingFlags FlattenHierarchy on GetFields does not throw.
        var flags = BindingFlags.Public | BindingFlags.Static | BindingFlags.FlattenHierarchy;
        var fields = typeof(Environment).GetFields(flags);
        Assert.NotNull((object)fields);
    }

    [Fact]
    public void _ReflectionBindingFlags_GetDefaultMembers()
    {
        // Purpose: Type.GetDefaultMembers does not throw.
        var members = typeof(string).GetDefaultMembers();
        Assert.NotNull((object)members);
    }
}
