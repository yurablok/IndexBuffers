/*** **************************************** ***
 **                 schema.ibs                 **
 *** **************************************** ***/
namespace MyNS;
//namespace ExternalNS.MyNS;

enum MyEnum
{
    field1,
    field2
};

struct Vec2
{
    x : float;
    y : float;
};

struct MyStruct
{
    vUint32 : uint32; // comment
    vBytes : bytes optional;
    vUint8 : uint8 optional;
    point : Vec2 optional;
    vArray : int64[]; // all arrays are optional by default
};
/*** **************************************** ***
 **           schema_generated.hpp             **
 *** **************************************** ***/

//TODO

/*** **************************************** ***
 **                  main.cpp                  **
 *** **************************************** ***/
// inb::NO_COMPRESSION
//TODO: inb::TABLE_COMPRESSION
//TODO: inb::FULL_COMPRESSION
MyNS::MyStruct src(inb::NO_COMPRESSION);
//MyNS::MyStruct src; // inb::NO_COMPRESSION by default
src.create(12345); // reserve size in bytes
//src.create(); // 0 by default

src.has_vUint8(); // returns false
src.set_vUint8(123); // throw std::logic_error
src.add_vUint8();
src.add_vUint8(); // no effects
src.get_vUint8(); // returns `const uint8_t &`
src.has_vUint8(); // returns true

src.set_vUint32(1234567);

src.ptr_vBytes(); // throw std::logic_error
src.add_vBytes(64); // reserve 64 bytes
src.ptr_vBytes(); // returns `uint8_t *`

src.add_point(inb::NO_COMPRESSION);
src.get_point().set_x(12.34f);
MyNS::Vec2 &point = src.get_point();
point.set_y(56.78f);
//TODO: check point.add_something_optional() -> where to store? in `src.buffer` because `src` is owner

src.add_vArray(8); // reserve 8 elements
src.get_vArray(); // returns `int64_t *`

// THERE IS NO NEED TO FINALIZE!

MyNS::MyStruct dst;
dst.from(src.ptr()); // from `uint8_t *` and ZERO COST!
dst.get_vUint8(); // throw std::logic_error
dst.add_vUint8(); // no effects
if (dst.has_vUint8())
    dst.get_vUint8();

if (dst.has_vBytes())
{
    dst.size_vBytes(); // returns `uint32_t` as number of bytes 
    dst.ptr_vBytes(); // returns `uint8_t *`
}
dst.ptr_vArray(); // throw std::logic_error
if (dst.has_vArray()
{
    dst.size_vArray(); // returns `uint32_t` as number of elements
    dst.ptr_vArray(); // returns `int64 *`
}

//TODO: src.erase_variable();
//TODO: src.shrink();
