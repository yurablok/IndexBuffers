//#include "gtest/gtest.h"
#include "gtest/gtest-all.cc"
#include "schema_generated.h"

TEST(Vec2StaticFields, Basic)
{
    std::cout << "sizeof=" << sizeof(ExtNS::IntNS::_inner_::header) << std::endl;

    ExtNS::IntNS::Vec2f srcPoint2f;
    srcPoint2f.create();
    srcPoint2f.set_x(1.23f);
    srcPoint2f.set_y(45.6f);
    void* ptr = srcPoint2f.to();

    ExtNS::IntNS::Vec2f dstPoint2f;
    ASSERT_EQ(dstPoint2f.from(ptr), true);
    EXPECT_EQ(dstPoint2f.get_x(), 1.23f);
    EXPECT_EQ(dstPoint2f.get_y(), 45.6f);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
