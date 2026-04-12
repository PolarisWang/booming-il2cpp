using System;

namespace BenchGameLoop;

internal sealed class GameObject
{
    public float X, Y, VX, VY;
    public bool Active;
}

internal static class Program
{
    private const int IterationCount = 1000;
    private const int ObjectCount = 100;
    private const int FrameCount = 10;
    private static long s_lastChecksum;

    public static int Main()
    {
        s_lastChecksum = RunWorkload();
        return 0;
    }

    public static long RunWorkload()
    {
        var objects = new GameObject[ObjectCount];
        for (int i = 0; i < ObjectCount; i++)
        {
            objects[i] = new GameObject { X = i, Y = i * 0.5f, VX = 0.1f, VY = -0.05f, Active = true };
        }

        double checksum = 0;

        for (int iter = 0; iter < IterationCount; iter++)
        {
            for (int frame = 0; frame < FrameCount; frame++)
            {
                foreach (var obj in objects)
                {
                    if (!obj.Active) continue;
                    obj.X += obj.VX;
                    obj.Y += obj.VY;
                    if (obj.X > 100 || obj.X < 0) obj.VX = -obj.VX;
                    if (obj.Y > 100 || obj.Y < 0) obj.VY = -obj.VY;
                }

                for (int i = 0; i < 10; i++)
                {
                    for (int j = i + 1; j < 10; j++)
                    {
                        float dx = objects[i].X - objects[j].X;
                        float dy = objects[i].Y - objects[j].Y;
                        checksum += Math.Sqrt(dx * dx + dy * dy);
                    }
                }
            }
        }

        return (long)(checksum % 10000);
    }
}
