//#include "gtest/gtest.h"
#include "gtest/gtest-all.cc"
#include "schema_generated.h"

TEST(OnlyStatic, Basic)
{
    ExtNS::IntNS::OnlyStatic src;
    ExtNS::IntNS::OnlyStatic dst;

    ASSERT_EQ(dst.from(src.to()), true);
    EXPECT_EQ(dst.has_a(), true);
    EXPECT_EQ(dst.has_b(), true);
    EXPECT_EQ(dst.has_c(), true);
    EXPECT_EQ(dst.has_d(), true);
    EXPECT_EQ(dst.has_e(), true);
    EXPECT_EQ(dst.has_f(), true);

    src.set_a(INT8_MAX - 0xa);
    src.set_b(UINT16_MAX - 0xb);
    src.set_c(INT32_MAX - 0xc);
    src.set_d(UINT64_MAX - 0xd);
    src.set_e(INT16_MAX - 0xe);
    src.set_f(INT32_MAX - 0xf);

    ASSERT_EQ(dst.from(src.to()), true);
    EXPECT_EQ(dst.get_a(), INT8_MAX - 0xa);
    EXPECT_EQ(dst.get_b(), UINT16_MAX - 0xb);
    EXPECT_EQ(dst.get_c(), INT32_MAX - 0xc);
    EXPECT_EQ(dst.get_d(), UINT64_MAX - 0xd);
    EXPECT_EQ(dst.get_e(), INT16_MAX - 0xe);
    EXPECT_EQ(dst.get_f(), INT32_MAX - 0xf);
}

TEST(OnlyOptional, Basic)
{
    ExtNS::IntNS::OnlyOptional src;
    ExtNS::IntNS::OnlyOptional dst;

    ASSERT_EQ(dst.from(src.to()), true);
    EXPECT_EQ(dst.has_a(), false);
    EXPECT_EQ(dst.has_b(), false);
    EXPECT_EQ(dst.has_c(), false);
    EXPECT_EQ(dst.has_d(), false);
    EXPECT_EQ(dst.has_e(), false);
    EXPECT_EQ(dst.has_f(), false);

    src.set_a(INT8_MAX - 0xa);
    src.set_c(INT32_MAX - 0xc);
    src.set_d(UINT64_MAX - 0xd);
    src.set_f(INT32_MAX - 0xf);

    ASSERT_EQ(dst.from(src.to()), true);
    EXPECT_EQ(dst.has_a(), true);
    EXPECT_EQ(dst.has_b(), false);
    EXPECT_EQ(dst.has_c(), true);
    EXPECT_EQ(dst.has_d(), true);
    EXPECT_EQ(dst.has_e(), false);
    EXPECT_EQ(dst.has_f(), true);

    src.set_b(UINT16_MAX - 0xb);
    src.set_e(INT16_MAX - 0xe);

    ASSERT_EQ(dst.from(src.to()), true);
    EXPECT_EQ(dst.has_a(), true);
    EXPECT_EQ(dst.has_b(), true);
    EXPECT_EQ(dst.has_c(), true);
    EXPECT_EQ(dst.has_d(), true);
    EXPECT_EQ(dst.has_e(), true);
    EXPECT_EQ(dst.has_f(), true);

    EXPECT_EQ(dst.get_a(), INT8_MAX - 0xa);
    EXPECT_EQ(dst.get_b(), UINT16_MAX - 0xb);
    EXPECT_EQ(dst.get_c(), INT32_MAX - 0xc);
    EXPECT_EQ(dst.get_d(), UINT64_MAX - 0xd);
    EXPECT_EQ(dst.get_e(), INT16_MAX - 0xe);
    EXPECT_EQ(dst.get_f(), INT32_MAX - 0xf);
}

TEST(Arrays, Basic)
{
    ExtNS::IntNS::Arrays src;
    ExtNS::IntNS::Arrays dst;

    ASSERT_EQ(dst.from(src.to()), true);
    EXPECT_EQ(dst.has_b(), false);
    EXPECT_EQ(dst.has_m(), false);
    EXPECT_EQ(dst.has_v(), false);

    std::string strSrc("Hello IndexBuffers");
    src.set_b(strSrc.c_str(), strSrc.size());

    std::vector<int16_t> arrSrc = { 45, -54, 63, -72, 81 };
    src.set_m(nullptr, 5);
    for (uint32_t i = 0; i < arrSrc.size(); ++i)
        src.set_m(i, arrSrc[i]);

    src.set_v(3);
    src.get_v(0).set_x(0.01f);
    src.get_v(0).set_y(0.02f);
    auto& v2 = src.get_v(2);
    v2.set_y(-1.23f);
    v2.set_z(1666.33f);

    ASSERT_EQ(dst.from(src.to()), true);
    EXPECT_EQ(dst.has_b(), true);
    EXPECT_EQ(dst.has_m(), true);
    EXPECT_EQ(dst.has_v(), true);

    std::string strDst;
    strDst.resize(dst.size_b());
    memcpy(&strDst[0], dst.get_b(), strDst.size());
    EXPECT_EQ(strSrc == strDst, true);

    const int16_t* arrDst = dst.get_m();
    EXPECT_EQ(arrDst[0], 45);
    EXPECT_EQ(arrDst[2], 63);
    EXPECT_EQ(arrDst[4], 81);

    ExtNS::IntNS::Vec3f& vec3 = dst.get_v(0);
    EXPECT_EQ(vec3.get_x(), 0.01f);
    EXPECT_EQ(vec3.get_y(), 0.02f);
    EXPECT_EQ(dst.get_v(2).get_y(), -1.23f);
    EXPECT_EQ(dst.get_v(2).get_z(), 1666.33f);
}

//TEST(BytesTest, Basic)
//{
//    ExtNS::IntNS::ArrayTest srcArray;
//    uint32_t src[5] = { 31, 42, 53, 64, 75 };
//    srcArray.set_var(88);
//    //srcArray.add_raw(5 * sizeof(uint32_t));
//    srcArray.set_raw(src, 5 * sizeof(uint32_t));
//    //srcArray.add_arr(5);
//    srcArray.set_arr(src, 5);
//    srcArray.set_arr(1, 21);
//    uint8_t* ptr = srcArray.to();
//
//    uint32_t dst[5];
//    ExtNS::IntNS::ArrayTest dstArray;
//    ASSERT_EQ(dstArray.from(ptr), true);
//    const uint32_t size = dstArray.size_raw() / sizeof(uint32_t);
//    EXPECT_EQ(size, 5);
//    const uint32_t* srcPtr = reinterpret_cast<const uint32_t*>(dstArray.get_raw());
//    for (uint32_t i = 0; i < size; i++)
//        dst[i] = srcPtr[i];
//    EXPECT_EQ(dst[0], 31);
//    EXPECT_EQ(dst[4], 75);
//    EXPECT_EQ(dstArray.get_arr(0), 31);
//    EXPECT_EQ(dstArray.get_arr(1), 21);
//}
//
//TEST(Advanced, TestCases)
//{
//    ExtNS::IntNS::ArrayTest srcArray;
//    srcArray.set_arr(nullptr, 4);
//    uint8_t* ptr = srcArray.to();
//
//    ExtNS::IntNS::ArrayTest dstArray;
//    ASSERT_EQ(dstArray.from(ptr), true);
//    EXPECT_EQ(dstArray.has_arr(), true);
//    EXPECT_EQ(dstArray.has_var(), true);
//    EXPECT_EQ(dstArray.has_raw(), false);
//}
//
//TEST(PacketTest, Test1)
//{
//    ExtNS::IntNS::Packet srcPacket(64);
//
//    ExtNS::IntNS::Packet dstPacket(srcPacket.to());
//    EXPECT_EQ(dstPacket.has_vBytes(),  false);
//    EXPECT_EQ(dstPacket.has_vUint32(),  true);
//
//    srcPacket.set_vUint32(12345);
//    EXPECT_EQ(dstPacket.get_vUint32(),  12345);
//
//    uint32_t src[5] = { 31, 42, 53, 64, 75 };
//    srcPacket.set_vBytes(src, 5 * sizeof(uint32_t));
//
//    uint32_t dst[5];
//    const uint32_t* srcPtr = reinterpret_cast<const uint32_t*>(dstPacket.get_vBytes());
//    const uint32_t srcSize = dstPacket.size_vBytes();
//    std::memcpy(dst, src, srcSize);
//    EXPECT_EQ(dst[0], 31);
//    EXPECT_EQ(dst[4], 75);
//}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
