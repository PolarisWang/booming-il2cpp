using Xunit;

namespace Chaos.IL2CPP.CodeGen.SnapshotTests;

public sealed class SnapshotTests : SnapshotTestBase
{
    public static IEnumerable<object[]> GetFixtures() =>
        FixtureRepository.DiscoverFixtures();

    [Theory]
    [MemberData(nameof(GetFixtures))]
    public void EmittedCodeMatchesBaseline(string fixtureName)
    {
        AssertSnapshotMatches(fixtureName);
    }
}
