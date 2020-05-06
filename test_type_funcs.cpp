//
// Bad idea credit: me
//
// Purpose: Create a syntax for defining functions between types and values
//
#include <iostream>
#include <vector>

#define TypeToValueDeclI(N, R) template<typename t> R N(); R N();
#define TypeToValueDecl(N, R, T, V) template<> constexpr R N<T>() { return V; }
#define TypeToValue(N, T) N<T>()

#define ValueToTypeDeclI(N, I) template<I v> class N;
#define ValueToTypeDecl(N, V, T) template<> class N<V> { public: T mem; };
#define ValueToType(N, X) decltype(N<X>::mem)

#define TypeToTypeDeclI(N) ValueToTypeDeclI(N, typename)
#define TypeToTypeDecl(N, Tf, Tt) ValueToTypeDecl(N, Tf, Tt)
#define TypeToType(N, T) ValueToType(N, T)

TypeToValueDeclI(TestConv, int)
TypeToValueDecl(TestConv, int, double, 5)
TypeToValueDecl(TestConv, int, float, 6)
TypeToValueDecl(TestConv, int, int, 7)

ValueToTypeDeclI(TestConv0, size_t)
ValueToTypeDecl(TestConv0, 7, double)
ValueToTypeDecl(TestConv0, 6, float)
ValueToTypeDecl(TestConv0, 5, int)

int main() {
    std::vector<ValueToType(TestConv0, TypeToValue(TestConv, int))> v;
    v.push_back(5.1);
    std::cout << v[0] << std::endl;
    // std::cout << TypeToValue(TestConv, ValueToType(TestConv0, 12)) << std::endl;

    return 0;
}
