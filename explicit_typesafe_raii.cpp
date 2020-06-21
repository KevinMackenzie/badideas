//
// Bad idea credit: Me
//
// Purpose: RAII can be very confusing when reading large codebases.  This aims
//           to reduce confusion by requiring explicit notation of RAII objects
//
//          This kind of wrapper class could be useful for other stuff.
//          This could be easily extended to force an explicit 'release' call
//
#include <iostream>

#define RAII_CLASS(X) \
    private: \
        friend class RAII<X>; \
        virtual ~X();

template<class T>
class RAII {
private:
    char _data[sizeof(T)];
    T* data() { return (T*)_data; }
    const T* data() const { return (T*)_data; }

    // static const uint64_t POISON = 0x4141414141414141;
    // bool is_valid() const { return *((uint64_t*)_data) != &_data ^ POISON; }
public:
    template<typename... args>
    RAII(args... a) {
        new (_data) T(a...);
    }
    // void release() {
    //     ((T*)_data)->~T();
    //     *((uint64_t*)_data) = &_data ^ POISON;
    // }
    ~RAII() {
        ((T*)_data)->~T();
        // assert(!is_valid());
    }
    const T* operator *() const { return data(); }
    const T* operator ->() const { return data(); }

    T* operator *() { return data(); }
    T* operator ->() { return data(); }

    operator T&() const { return *data(); }
};

class ScopedResource {
    RAII_CLASS(ScopedResource);
private:
    int resource = 0;
public:
    ScopedResource(int res) : resource(res) {
        std::cout << "Acquired resource " << resource << std::endl;
    }
    int getResource() const { return resource; }
};

ScopedResource::~ScopedResource() {
    std::cout << "Released resource " << resource << std::endl;
}


int main() {
    auto res = RAII<ScopedResource>(100);
    std::cout << "Accessed resource " << res->getResource() << std::endl;

    // res.release();
    return 0;
}