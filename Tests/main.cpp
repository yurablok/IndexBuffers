//#include "gtest/gtest.h"
#include "gtest/gtest-all.cc"
#include "schema_generated.h"

TEST(Vec2StaticFields, Basic)
{
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

TEST(BytesTest, Basic)
{
    ExtNS::IntNS::ArrayTest srcArray;
    srcArray.create();
    uint32_t src[5] = { 31, 42, 53, 64, 75 };
    srcArray.set_var(88);
    srcArray.add_raw(5 * sizeof(uint32_t));
    srcArray.set_raw(src);
    srcArray.add_arr(5);
    srcArray.set_arr(src);
    srcArray.set_arr(1, 21);
    void* ptr = srcArray.to();

    uint32_t dst[5];
    ExtNS::IntNS::ArrayTest dstArray;
    ASSERT_EQ(dstArray.from(ptr), true);
    const uint32_t size = dstArray.size_raw() / sizeof(uint32_t);
    EXPECT_EQ(size, 5);
    const uint32_t* srcPtr = reinterpret_cast<const uint32_t*>(dstArray.get_raw());
    for (uint32_t i = 0; i < size; i++)
        dst[i] = srcPtr[i];
    EXPECT_EQ(dst[0], 31);
    EXPECT_EQ(dst[4], 75);
    EXPECT_EQ(dstArray.get_arr(0), 31);
    EXPECT_EQ(dstArray.get_arr(1), 21);
}

TEST(Advanced, TestCases)
{
    ExtNS::IntNS::ArrayTest srcArray;
    srcArray.create();
    srcArray.add_arr(4);
    void* ptr = srcArray.to();

    ExtNS::IntNS::ArrayTest dstArray;
    ASSERT_EQ(dstArray.from(ptr), true);
    EXPECT_EQ(dstArray.has_arr(), true);
    EXPECT_EQ(dstArray.has_var(), true);
    EXPECT_EQ(dstArray.has_raw(), false);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
