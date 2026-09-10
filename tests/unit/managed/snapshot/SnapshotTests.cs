using Xunit;

namespace Chaos.IL2CPP.Generator.SnapshotTests;

public sealed class SnapshotTests : SnapshotTestBase
{
    public static IEnumerable<object[]> GetFixtures() =>
        FixtureRepository.DiscoverFixtures()
            .Where(f => !NegationTests.NegativeFixtureNames.Contains((string)f[0]));

    [Theory]
    [MemberData(nameof(GetFixtures))]
    public void EmittedCodeMatchesBaseline(string fixtureName)
    {
        AssertSnapshotMatches(fixtureName);
    }
}
