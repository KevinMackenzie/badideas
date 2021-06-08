//
// Bad idea credit: Me
//
// Purpose: A system for enforcing incompatable type aliases with identical
//          arithmetic operations; Essentially strong type aliases
//

#define newtype(X, Y) typedef struct {Y _val;} X;

// This is less than ideal; we cannot concatenate "operator" and "+" as
//  preprocessor tokens to form "operator+" automatically
#define nt_binop(X, OP_NAME, OP) \
inline X OP_NAME(const X& x0, const X& x1) { return {x0._val OP x1._val}; }

#define nt_add(X) nt_binop(X, operator+, +)
#define nt_sub(X) nt_binop(X, operator-, -)
#define nt_linear(X) nt_add(X) nt_sub(X)


struct Vec3 {
    float x, y, z;
};

#define v3cw_binop(OP_NAME, OP) \
inline Vec3 OP_NAME(const Vec3& v0, const Vec3& v1) { \
    return {v0.x OP v1.x, v0.y OP v1.y, v0.z OP v1.z}; \
}

v3cw_binop(operator+, +)
v3cw_binop(operator-, -)

newtype(ColorRGBf, Vec3)
nt_linear(ColorRGBf);

newtype(Position, Vec3)
nt_linear(Position);

newtype(Flux, Vec3)
nt_linear(Flux);


int main() {
    ColorRGBf col0 = {1, 0.5, 1};
    Position pos0 = {20, 30, 40};
    Flux pow0 = {0.01, 0.02, 0.04};

    auto col1 = col0 + col0;
    auto pos1 = pos0 + pos0;
    auto pow1 = pow0 - pow0;

    // Doesn't compile because po1 and col0 are distinct types
    // auto col1 = pos1 + col0;
}