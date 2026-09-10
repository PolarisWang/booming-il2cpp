// numerics_vectors_test.cpp — regression test for Vector2/3/4 _All lane-reducers.
//
// A2-1 (cross-platform-unify): the Vector2/3/4::GreaterThanAll/LessThanAll/
// GreaterThanOrEqualAll/LessThanOrEqualAll reducers were previously unresolved
// (no native call emitted) → routed to ChaosExternalRuntimeFallback → interpreter
// / managed exception (A1 gap; fact numerics 164→176 with the fix).  This test
// locks the native lane-reduce semantics directly so a regression back to the
// fallback path (returning 0/constant) is caught by the unit layer.
//
// Each _All reducer returns int32 (1 if every lane satisfies the relation, else 0)
// — matching the fact bool→Int64 subject contract.

#include "runtime_core.h"

#include <gtest/gtest.h>

namespace chaos::il2cpp::runtime_core {

TEST(NumericsVectorAll, Vector2GreaterThanAll) {
    RuntimeNumericsVector2Carrier a{3.0f, 5.0f};
    RuntimeNumericsVector2Carrier b{2.0f, 3.0f};
    EXPECT_EQ(Vector2GreaterThanAll(a, b), 1);        // 3>2 && 5>3
    EXPECT_EQ(Vector2GreaterThanAll(b, a), 0);        // 2>3 false
}

TEST(NumericsVectorAll, Vector2LessThanAll) {
    RuntimeNumericsVector2Carrier a{3.0f, 5.0f};
    RuntimeNumericsVector2Carrier b{2.0f, 3.0f};
    EXPECT_EQ(Vector2LessThanAll(b, a), 1);           // 2<3 && 3<5
    EXPECT_EQ(Vector2LessThanAll(a, b), 0);           // 3<2 false
}

TEST(NumericsVectorAll, Vector2OrEqual) {
    RuntimeNumericsVector2Carrier a{3.0f, 5.0f};
    RuntimeNumericsVector2Carrier b{2.0f, 3.0f};
    EXPECT_EQ(Vector2GreaterThanOrEqualAll(a, b), 1); // 3>=2 && 5>=3
    EXPECT_EQ(Vector2LessThanOrEqualAll(b, a), 1);    // 2<=3 && 3<=5
    // Equal lanes: not strictly greater, but or-equal true.
    RuntimeNumericsVector2Carrier e{4.0f, 4.0f};
    EXPECT_EQ(Vector2GreaterThanAll(e, e), 0);
    EXPECT_EQ(Vector2LessThanAll(e, e), 0);
    EXPECT_EQ(Vector2GreaterThanOrEqualAll(e, e), 1);
    EXPECT_EQ(Vector2LessThanOrEqualAll(e, e), 1);
}

TEST(NumericsVectorAll, Vector3All) {
    RuntimeNumericsVector3Carrier a{3.0f, 5.0f, 1.0f};
    RuntimeNumericsVector3Carrier b{2.0f, 3.0f, 0.0f};
    EXPECT_EQ(Vector3GreaterThanAll(a, b), 1);   // 3>2 && 5>3 && 1>0
    EXPECT_EQ(Vector3LessThanAll(b, a), 1);      // 2<3 && 3<5 && 0<1
    RuntimeNumericsVector3Carrier c{3.0f, 2.0f, 1.0f};
    EXPECT_EQ(Vector3GreaterThanOrEqualAll(c, a), 0); // 3>=3 but 2>=5 false
    EXPECT_EQ(Vector3LessThanOrEqualAll(c, a), 1);    // 3<=3 && 2<=5 && 1<=1
}

TEST(NumericsVectorAll, Vector4All) {
    RuntimeNumericsVector4Carrier a{9.0f, 8.0f, 7.0f, 6.0f};
    RuntimeNumericsVector4Carrier b{8.0f, 7.0f, 6.0f, 5.0f};
    EXPECT_EQ(Vector4GreaterThanAll(a, b), 1);
    EXPECT_EQ(Vector4LessThanAll(b, a), 1);
    RuntimeNumericsVector4Carrier c{9.0f, 8.0f, 7.0f, 10.0f};
    EXPECT_EQ(Vector4GreaterThanAll(a, c), 0);   // 9>9 false
    EXPECT_EQ(Vector4GreaterThanOrEqualAll(a, c), 0); // 6>=10 false
    EXPECT_EQ(Vector4LessThanOrEqualAll(c, a), 0);    // 10<=6 false
}

}  // namespace chaos::il2cpp::runtime_core
