using System;
using System.Collections.Generic;

public static class Program
{
    static int Main()
    {
        var results = new List<string>();
        try { results.Add($"RESULT 0:{Convert.ToInt32(MathNumericsSubjects.Subject_0())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 0:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 1:{Convert.ToInt32(MathNumericsSubjects.Subject_1())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 1:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 2:{Convert.ToInt32(MathNumericsSubjects.Subject_2())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 2:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 3:{Convert.ToInt32(MathNumericsSubjects.Subject_3())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 3:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 4:{Convert.ToInt32(MathNumericsSubjects.Subject_4())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 4:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 5:{Convert.ToInt32(MathNumericsSubjects.Subject_5())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 5:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 6:{Convert.ToInt32(MathNumericsSubjects.Subject_6())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 6:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 7:{Convert.ToInt32(MathNumericsSubjects.Subject_7())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 7:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 8:{Convert.ToInt32(MathNumericsSubjects.Subject_8())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 8:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 9:{Convert.ToInt32(MathNumericsSubjects.Subject_9())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 9:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 10:{Convert.ToInt32(MathNumericsSubjects.Subject_10())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 10:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 11:{Convert.ToInt32(MathNumericsSubjects.Subject_11())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 11:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 12:{Convert.ToInt32(MathNumericsSubjects.Subject_12())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 12:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 13:{Convert.ToInt32(MathNumericsSubjects.Subject_13())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 13:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 14:{Convert.ToInt32(MathNumericsSubjects.Subject_14())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 14:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 15:{Convert.ToInt32(MathNumericsSubjects.Subject_15())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 15:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 16:{Convert.ToInt32(MathNumericsSubjects.Subject_16())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 16:{ex.GetType().Name}"); }
        try { results.Add($"RESULT 17:{Convert.ToInt32(MathNumericsSubjects.Subject_17())}"); }
        catch (System.Exception ex) { results.Add($"EXCEPTION 17:{ex.GetType().Name}"); }
        Console.WriteLine(string.Join("\n", results));
        return 0;
    }
}
