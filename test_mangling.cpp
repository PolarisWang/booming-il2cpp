namespace A {
    void foo() noexcept;
}
namespace A {
namespace B {
extern "C" {
    void bar() noexcept { foo(); }
}
}
}
int main() { return 0; }
