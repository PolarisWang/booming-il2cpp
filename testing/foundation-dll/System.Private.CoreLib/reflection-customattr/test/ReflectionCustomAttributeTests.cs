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

[CapabilityTest("family/System.Private.CoreLib/reflection/customattr/proof")]
public partial class ReflectionCustomAttributeTests
{
    // === R4.3 CustomAttribute proof: verify IsDefined and GetCustomAttribute do not crash ===

    [Fact]
    public void _ReflectionCustomAttribute_IsDefinedOnType()
    {
        // Purpose: Type.IsDefined does not throw even if AOT attribute resolution is limited.
        bool defined = typeof(string).IsDefined(typeof(SerializableAttribute), false);
    }

    [Fact]
    public void _ReflectionCustomAttribute_IsDefinedOnMethod()
    {
        // Purpose: MethodInfo.IsDefined does not throw.
        var method = typeof(string).GetMethod("ToString", Array.Empty<Type>());
        if (method != null)
        {
            bool defined = method.IsDefined(typeof(SerializableAttribute), false);
        }
    }

    [Fact]
    public void _ReflectionCustomAttribute_GetCustomAttributeOnType()
    {
        // Purpose: Type.GetCustomAttribute does not throw.
        var attr = typeof(string).GetCustomAttribute(typeof(SerializableAttribute), false);
    }

    [Fact]
    public void _ReflectionCustomAttribute_GetCustomAttributesOnType()
    {
        // Purpose: Type.GetCustomAttributes does not throw.
        var attrs = typeof(string).GetCustomAttributes(false);
        Assert.NotNull((object)attrs);
    }

    [Fact]
    public void _ReflectionCustomAttribute_GetCustomAttributesOnMethod()
    {
        // Purpose: MethodInfo.GetCustomAttributes does not throw.
        var method = typeof(string).GetMethod("ToString", Array.Empty<Type>());
        if (method != null)
        {
            var attrs = method.GetCustomAttributes(false);
            Assert.NotNull((object)attrs);
        }
    }

    [Fact]
    public void _ReflectionCustomAttribute_IsDefinedOnField()
    {
        // Purpose: FieldInfo.IsDefined does not throw.
        var field = typeof(string).GetField("Empty", BindingFlags.Public | BindingFlags.Static);
        if (field != null)
        {
            bool defined = field.IsDefined(typeof(SerializableAttribute), false);
        }
    }

    [Fact]
    public void _ReflectionCustomAttribute_GetCustomAttributesOnField()
    {
        // Purpose: FieldInfo.GetCustomAttributes does not throw.
        var field = typeof(string).GetField("Empty", BindingFlags.Public | BindingFlags.Static);
        if (field != null)
        {
            var attrs = field.GetCustomAttributes(false);
            Assert.NotNull((object)attrs);
        }
    }

    [Fact]
    public void _ReflectionCustomAttribute_GetCustomAttributesOnProperty()
    {
        // Purpose: PropertyInfo.GetCustomAttributes does not throw.
        var prop = typeof(string).GetProperty("Length", BindingFlags.Public | BindingFlags.Instance);
        if (prop != null)
        {
            var attrs = prop.GetCustomAttributes(false);
            Assert.NotNull((object)attrs);
        }
    }

    [Fact]
    public void _ReflectionCustomAttribute_GetCustomAttributesOnParameter()
    {
        // Purpose: ParameterInfo.GetCustomAttributes does not throw.
        var method = typeof(string).GetMethod("IndexOf", new Type[] { typeof(string) });
        if (method != null)
        {
            var pars = method.GetParameters();
            if (pars.Length > 0)
            {
                var attrs = pars[0].GetCustomAttributes(false);
                Assert.NotNull((object)attrs);
            }
        }
    }

    [Fact]
    public void _ReflectionCustomAttribute_IsDefinedOnMethodNoInherit()
    {
        // Purpose: IsDefined with inherit=false on method does not throw.
        var method = typeof(object).GetMethod("ToString", Array.Empty<Type>());
        if (method != null)
        {
            bool defined = method.IsDefined(typeof(SerializableAttribute), false);
        }
    }

    [Fact]
    public void _ReflectionCustomAttribute_GetCustomAttributeOnEnum()
    {
        // Purpose: GetCustomAttribute on enum type does not throw.
        var attrs = typeof(StringComparison).GetCustomAttributes(typeof(SerializableAttribute), false);
    }
}
