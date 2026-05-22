namespace StubAssembly;

public struct ValuePoint
{
    public int X;
    public int Y;
}

public static class DataOperations
{
    // Exercises stfld, ldfld, initobj
    public static int SumStructFields()
    {
        var p = new ValuePoint();
        p.X = 10;
        p.Y = 20;
        return p.X + p.Y;
    }

    // Exercises ldflda (field address)
    public static void DoubleField(ref ValuePoint p)
    {
        p.X *= 2;
        p.Y *= 2;
    }

    // Exercises ldsfld, stsfld via static field on a struct
    public static ValuePoint Origin => new ValuePoint { X = 0, Y = 0 };
}
