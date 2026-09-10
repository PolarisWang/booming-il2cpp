// Minimal console app for il2cpp full-translation e2e verification.
// Purpose: Provide a known-good .NET executable assembly to feed into
// `convert-to-cpp --full-closure` and verify the generated C++ code
// compiles and runs correctly.
//
// Covers: string, math, array, class instantiation, primitive operations.

using Il2CppTranslationTest;

// String
string hello = "Hello, il2cpp!";
int len = hello.Length;
bool hasHello = hello.Contains("il2cpp");

// Math
int sum = Add(10, 20);
int product = Multiply(3, 7);

// Array
int[] numbers = [1, 2, 3, 4, 5];
int first = numbers[0];
int last = numbers[numbers.Length - 1];
int total = SumArray(numbers);

// Object construction
var point = new Point(3, 4);
double distance = point.Distance();

// Return 0 if all checks pass
int result = (len > 0 && hasHello && sum == 30 && product == 21
              && first == 1 && last == 5 && total == 15
              && distance > 0.0) ? 0 : 1;
return result;

static int Add(int a, int b) => a + b;
static int Multiply(int a, int b) => a * b;
static int SumArray(int[] arr)
{
    int total = 0;
    foreach (int v in arr)
        total += v;
    return total;
}
