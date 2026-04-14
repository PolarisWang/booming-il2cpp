using GoldenMultiProject.Core;

namespace GoldenMultiProject.Middle;

public static class MessageComposer
{
    public static string Compose()
    {
        return "multi:" + SuffixFactory.Create();
    }
}
