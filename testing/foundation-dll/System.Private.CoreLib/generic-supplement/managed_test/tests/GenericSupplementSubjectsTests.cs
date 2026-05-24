// Auto-generated SDK test skeleton
// Do not modify — re-generate via foundation-dll pipeline

using Chaos.TestFramework;
using Xunit;

namespace GenericSupplementSubjectsTests;

[CapabilityTest("family/System.Private.CoreLib/generic/supplement")]
public class GenericSupplementSubjectsTests
{
    [Fact]
    [MethodSubjectId("GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestSupplementalMetadataRoundtrip:System.Int32()")]
    public void Subject_0()
    {
        GenericSupplementSubjects._exitCode = 0;
        GenericSupplementSubjects.Subject_0();
        Assert.Equal(0, GenericSupplementSubjects._exitCode);
    }

    [Fact]
    [MethodSubjectId("GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestTypeLookupBySubjectId:System.Int32()")]
    public void Subject_1()
    {
        GenericSupplementSubjects._exitCode = 0;
        GenericSupplementSubjects.Subject_1();
        Assert.Equal(0, GenericSupplementSubjects._exitCode);
    }

    [Fact]
    [MethodSubjectId("GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestTypeLookupByToken:System.Int32()")]
    public void Subject_2()
    {
        GenericSupplementSubjects._exitCode = 0;
        GenericSupplementSubjects.Subject_2();
        Assert.Equal(0, GenericSupplementSubjects._exitCode);
    }

    [Fact]
    [MethodSubjectId("GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestGenericInstantiation:System.Int32()")]
    public void Subject_3()
    {
        GenericSupplementSubjects._exitCode = 0;
        GenericSupplementSubjects.Subject_3();
        Assert.Equal(0, GenericSupplementSubjects._exitCode);
    }

}