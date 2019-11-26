#include <cstdlib>
#ifdef _WIN32
#   define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#define ASSERT(x) if (!(x)) { \
    printf("ASSERT FAILED: (%s)\n  function: %s\n  file: %s\n  line: %d\n", \
        #x, __PRETTY_FUNCTION__, __FILE__, __LINE__); std::abort(); }

#include "schema_generated.hpp"

void testColor() {
    using namespace ExtNS::IntNS;
    ASSERT(Color::min() == Color::RED);
    ASSERT(Color::max() == Color::BLUE);
    ASSERT(Color::count() == 3);
    std::string str = Color::to_string(Color::GREEN);
    ASSERT(str == "GREEN");
    ASSERT(Color::from_string(str) == Color::GREEN);
}

void testScalarTypes() {
    using namespace ExtNS::IntNS;
    ScalarTypes src;
    ScalarTypes dst;

    ASSERT(src.to() == nullptr);
    ASSERT(dst.to() == nullptr);
    ASSERT(src.size() == 0);
    ASSERT(dst.size() == 0);
    ASSERT(dst.from(src.to()) == false);
    src.create();
    ASSERT(src.has_a() == true);
    ASSERT(src.has_b() == true);
    ASSERT(src.has_c() == false);
    ASSERT(src.has_d() == false);
    ASSERT(src.has_e() == true);
    ASSERT(src.has_f() == true);
    ASSERT(src.size() == ScalarTypes::size_min());
    src.set_a(-120);
    src.set_b(63000);
    src.set_c(-2100000000);
    src.set_d(18000000000000000000);
    src.set_e(123.456f);
    src.set_f(1234.5678);
    ASSERT(src.has_a() == true);
    ASSERT(src.has_b() == true);
    ASSERT(src.has_c() == true);
    ASSERT(src.has_d() == true);
    ASSERT(src.has_e() == true);
    ASSERT(src.has_f() == true);
    ASSERT(src.get_a() == -120);
    ASSERT(src.get_b() == 63000);
    ASSERT(src.get_c() == -2100000000);
    ASSERT(src.get_d() == 18000000000000000000);
    ASSERT(src.get_e() == 123.456f);
    ASSERT(src.get_f() == 1234.5678);

    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());
    ASSERT(dst.get_a() == -120);
    ASSERT(dst.get_b() == 63000);
    ASSERT(dst.get_c() == -2100000000);
    ASSERT(dst.get_d() == 18000000000000000000);
    ASSERT(dst.get_e() == 123.456f);
    ASSERT(dst.get_f() == 1234.5678);
    ASSERT(dst.size() <= ScalarTypes::size_max());
}

void testArrays() {
    using namespace ExtNS::IntNS;
    Arrays src;
    Arrays dst;

    src.create();
    ASSERT(src.has_f() == false);
    ASSERT(src.has_b() == true);
    ASSERT(src.has_m() == false);
    ASSERT(src.has_v() == false);
    ASSERT(fixedSize == 16);
    ASSERT(src.size_b() == fixedSize);
    ASSERT(src.size() == Arrays::size_min());
    src.set_f();
    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());
    src.set_m(fixedSize);
    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());
    src.set_v(fixedSize);
    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());
    for (uint8_t i = 0; i < fixedSize; ++i) {
        src.get_f(i) = i;
        src.get_b(i) = i;
        src.get_m(i) = i * (-2);
        src.get_v(i).set_x(-i);
        src.get_v(i).set_y(i);
        src.get_v(i).set_z(i * 2);
    }
    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());
    ASSERT(dst.size_f() == fixedSize);
    ASSERT(dst.size_b() == fixedSize);
    ASSERT(dst.size_m() == fixedSize);
    ASSERT(dst.size_v() == fixedSize);
    for (uint8_t i = 0; i < fixedSize; ++i) {
        ASSERT(dst.get_f(i) == i);
        ASSERT(dst.get_b(i) == i);
        ASSERT(dst.get_m(i) == i * (-2));
        ASSERT(dst.get_v(i).get_x() == -i);
        ASSERT(dst.get_v(i).get_y() == i);
        ASSERT(dst.get_v(i).get_z() == i * 2);
    }
    ASSERT(dst.size() <= Arrays::size_max());
}

void testVariant() {
    using namespace ExtNS::IntNS;
    Variant src;
    Variant dst;

    ASSERT(dst.from(src.to()) == false);
    src.create();
    ASSERT(src.size() == Variant::size_min());
    ASSERT(src.has_color() == false);
    ASSERT(src.has_data() == false);
    ASSERT(src.has_arr() == false);
    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());

    src.set_color(Color::GREEN);
    ASSERT(src.has_color() == true);
    ASSERT(src.has_data() == false);
    ASSERT(src.has_arr() == false);
    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());
    ASSERT(dst.get_color() == Color::GREEN);

    src.create();
    src.set_arr(5);
    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() > Variant::size_min());
    ASSERT(dst.size() == src.size());
    ASSERT(dst.size() == Variant::size_min() + 4 + sizeof(int64_t) * 5);

    src.create();
    src.set_data();
    src.get_data().set_v(3);
    src.get_data().get_v(2).set_y(123.456f);
    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());
    ASSERT(src.has_color() == false);
    ASSERT(src.has_data() == true);
    ASSERT(src.has_arr() == false);
    ASSERT(dst.get_data().get_v(2).get_y() == 123.456f);
    ASSERT(dst.size() <= Variant::size_max());
}

void testAnotherOne() {
    using namespace ExtNS::IntNS;
    AnotherOne src;
    AnotherOne dst;

    src.create();
    ASSERT(src.size() == AnotherOne::size_min());
    ASSERT(src.has_string() == false);
    ASSERT(src.has_variant() == false);
    ASSERT(src.has_colorChannels() == true);
    ASSERT(src.has_colorMask() == true);
    ASSERT(src.has_someData() == true);
    ASSERT(src.size_colorChannels() == Color::count());
    ASSERT(src.get_colorMask() == Color::BLUE);
    ASSERT(src.get_someData().has_var() == false);
    ASSERT(src.get_someData().get_def() == -123);
    std::string str_test("test");
    src.set_string(str_test);
    src.set_variant();
    src.get_variant().set_arr(2);

    ASSERT(dst.from(src.to()) == true);
    ASSERT(dst.size() == src.size());
    ASSERT(src.has_string() == true);
    ASSERT(src.has_variant() == true);
    ASSERT(src.has_colorChannels() == true);
    ASSERT(src.has_colorMask() == true);
    ASSERT(src.has_someData() == true);
    ASSERT(dst.size_string() == str_test.size());
    ASSERT(dst.get_string(0) == dst.get_string(3));
    ASSERT(dst.get_string<std::string>() == str_test);
    ASSERT(dst.get_variant().size_arr() == 2);
    ASSERT(dst.get_someData().has_var() == false);
    ASSERT(dst.size() <= AnotherOne::size_max());
}

int main() {
    testColor();
    testScalarTypes();
    testArrays();
    testVariant();
    testAnotherOne();
    return 0;
}
