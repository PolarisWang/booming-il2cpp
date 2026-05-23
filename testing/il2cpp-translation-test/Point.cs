namespace Il2CppTranslationTest;

public class Point
{
    private readonly int _x;
    private readonly int _y;

    public Point(int x, int y)
    {
        _x = x;
        _y = y;
    }

    public double Distance()
    {
        return Math.Sqrt(_x * _x + _y * _y);
    }
}
