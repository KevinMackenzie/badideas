//
// Bad idea credit: me
//
// Purpose: Create a control flow system for bounds checking results based on
//           pattern matching "catch" statements
//
// Issues: It creates an exponential explosion of new types even for fairly
//          small numbers and small ranges
//
#include <iostream>
#include <cstdint>

template<typename parent, bool predicate> class inherit_if;
template<typename parent> class inherit_if<parent, false> {};
template<typename parent> class inherit_if<parent, true> : public parent {};

class num_base { 
public:
    virtual size_t value() const = 0;
};

class range_base {
public:
    size_t value() const {
        return 0;
    }
};

template<size_t mn, size_t mx>
class range : public range_base {};

// Ideally we could reuse some of this machinery so we don't create n classes
//  for each instance of the number
template<size_t num, size_t mn, size_t mx, size_t mnn, size_t mxx>
class inherit_range : 
    public range<mn,mx>,
    public inherit_range<num, mn-1, mx, mnn, mxx> {};
template<size_t num, size_t mx, size_t mnn, size_t mxx>
class inherit_range<num, mnn, mx, mnn, mxx> : 
    public range<mnn,mx>,
    public inherit_range<num, num, mx+1, mnn, mxx>{};
template<size_t num, size_t mnn, size_t mxx>
class inherit_range<num, mnn, mxx, mnn, mxx> {};

template<size_t num, size_t mnn, size_t mxx>
class inherit_num : public inherit_if<inherit_range<num, num, num, mnn, mxx>,
                                     (num > mnn && num < mxx)>,
                    public num_base {
public:
    virtual size_t value() const { return num; }
};

constexpr size_t sm_range = 255;
template<size_t num>
using small_num = inherit_num<num, 0, sm_range>;

int main() {
    try {
        throw small_num<40>();
    } catch(const range<2, 14>& r) {
        std::cout << "In Range A: " << r.value() << std::endl;
    } catch(const range<15, 20>& r) {
        std::cout << "In Range B: " << r.value() << std::endl;
    } catch(const range_base& r) {
        std::cout << "Not in Range: " << r.value() << std::endl;
    }
}
