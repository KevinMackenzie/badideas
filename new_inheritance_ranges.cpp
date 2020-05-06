//
// Bad idea credit: me
//
// Purpose: Create a control flow system for bounds checking results based on
//           pattern matching "catch" statements, but less type explosion
//
// Issues: It doesn't work
//
#include <iostream>
#include <vector>

class foo {
    public:
        void Hello() const { std::cout << "Hello" << std::endl; }
};

class bar : protected foo {
    public:
        void NoThanks() const { *(size_t*)(0x1337);}
};

template<typename...all>
class inherit_all;
template<typename head, typename...tail>
class inherit_all : public inherit_once<head, tail>, public inherit_all<tail> {};
template<>
class inherit_all<> {};

// Inherits from 't' if 't' is not in 'ls'
template<typename t, typename...ls>
class inherit_once;
template<typename t, typename n, typename...ls>
class inherit_once<t, n, ls> : public inherit_once<t, ls> {};
template<typename t, typename...ls>
class inherit_once<t, t, ls> {};
template<typename t>
class inherit_once<t> : public t {};

// Inherits from all parents of a range with min/max with quadratic complexity wrt. range size
template<size_t l, size_t h, size_t min, size_t max, typename...collected>
class range_parents : public range_parents<l, h-1, min, max, range<l,h>, ...collected> {};
template<size_t l, size_t min, size_t max, typename...collected>
class range_parents<l, l, min, max, ...collected> : public range_parents<l-1, max, min, max, range<l,l>, ...collected> {};
template<size_t min, size_t max, typename...collected>
class range_parents<min, min, min, max, ...collected> : public inherit_all<...collected> {};

// Inheritence pattern represents the join of the lattice
//      of the supersets of ordered pairs (x,y) st. x <= y
//                                                  x,y in [min,max]
//  Subclassing = set inclusion
//  Superclassing = set intersection
template<size_t min, size_t max>
class range : public range<min-1, max>,
              public range<min, max+1> {};
template<size_t v>
class range<v> {
public:
    class num {
    public:
        constexpr size_t value() { return v; }
    }
};

template<size_t v>
using num = range<v>::num;

// we want numbers to only derive from ranges that are referenced.
//  We need some sort of global storage of instantiated ranges


int main() {

    try {
        throw bar();
    } catch(const foo&) {
        std::cout << "Got a foo" << std::endl;
    } catch(...) {
        std::cout << "Got not a foo" << std::endl;
    }
    return 0;
}
