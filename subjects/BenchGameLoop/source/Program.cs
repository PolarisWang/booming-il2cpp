using System;
using System.Diagnostics;

namespace BenchGameLoop;

internal sealed class GameObject
{
    public float X, Y, VX, VY;
    public bool Active;
}

internal static class Program
{
    public static int Main(string[] args)
    {
        int iterations = args.Length > 0 && int.TryParse(args[0], out var n) ? n : 1000;
        const int ObjectCount = 100;
        const int FrameCount = 10;

        var objects = new GameObject[ObjectCount];
        for (int i = 0; i < ObjectCount; i++)
            objects[i] = new GameObject { X = i, Y = i * 0.5f, VX = 0.1f, VY = -0.05f, Active = true };

        var sw = Stopwatch.StartNew();
        double checksum = 0;

        for (int iter = 0; iter < iterations; iter++)
        {
            for (int frame = 0; frame < FrameCount; frame++)
            {
                // Update phase
                foreach (var obj in objects)
                {
                    if (!obj.Active) continue;
                    obj.X += obj.VX;
                    obj.Y += obj.VY;
                    if (obj.X > 100 || obj.X < 0) obj.VX = -obj.VX;
                    if (obj.Y > 100 || obj.Y < 0) obj.VY = -obj.VY;
                }

                // Simple collision check (O(n^2) subset)
                for (int i = 0; i < 10; i++)
                    for (int j = i + 1; j < 10; j++)
                    {
                        float dx = objects[i].X - objects[j].X;
                        float dy = objects[i].Y - objects[j].Y;
                        checksum += Math.Sqrt(dx * dx + dy * dy);
                    }
            }
        }
        sw.Stop();

        double framesPerSec = (iterations * FrameCount) / (sw.Elapsed.TotalMilliseconds / 1000.0);
        Console.WriteLine($"{{\"elapsedMilliseconds\":{sw.Elapsed.TotalMilliseconds:F3},\"iterations\":{iterations},\"framesPerSecond\":{framesPerSec:F0},\"checksum\":{(long)(checksum % 10000)}}}");
        return 0;
    }
}
