// Auto-generated SDK test skeleton
// Do not modify — re-generate via foundation-dll pipeline

using Chaos.TestFramework;
using Xunit;

namespace DispatchBasicSubjectsTests;

[CapabilityTest("family/System.Private.CoreLib/dispatch/basic")]
public class DispatchBasicSubjectsTests
{
    [Fact]
    [MethodSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::Run:System.Int32(System.Int32)")]
    public void Subject_0()
    {
        DispatchBasicSubjects._exitCode = 0;
        DispatchBasicSubjects.Subject_0();
        Assert.Equal(0, DispatchBasicSubjects._exitCode);
    }

    [Fact]
    [MethodSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()")]
    public void Subject_1()
    {
        DispatchBasicSubjects._exitCode = 0;
        DispatchBasicSubjects.Subject_1();
        Assert.Equal(0, DispatchBasicSubjects._exitCode);
    }

    [Fact]
    [MethodSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()")]
    public void Subject_2()
    {
        DispatchBasicSubjects._exitCode = 0;
        DispatchBasicSubjects.Subject_2();
        Assert.Equal(0, DispatchBasicSubjects._exitCode);
    }

    [Fact]
    [MethodSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()")]
    public void Subject_3()
    {
        DispatchBasicSubjects._exitCode = 0;
        DispatchBasicSubjects.Subject_3();
        Assert.Equal(0, DispatchBasicSubjects._exitCode);
    }

    [Fact]
    [MethodSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()")]
    public void Subject_4()
    {
        DispatchBasicSubjects._exitCode = 0;
        DispatchBasicSubjects.Subject_4();
        Assert.Equal(0, DispatchBasicSubjects._exitCode);
    }

}