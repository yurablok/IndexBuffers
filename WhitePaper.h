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
MyNS::MyStruct src;
src.create(12345); // reserve size in bytes
//src.create(); // 0 by default

src.has_vUint8(); // returns false
src.set_vUint8(123); // throw std::logic_error
src.add_vUint8();
src.add_vUint8(); // has no effects
src.get_vUint8(); // returns `const uint8_t &`
src.has_vUint8(); // returns true

src.has_vUint32(); // always returns true
src.set_vUint32(1234567);
src.add_vUint32(); // always has no effects

int64_t vArray[8];

src.get_vBytes(); // throw std::logic_error
src.add_vBytes(64); // reserve 64 bytes
src.set_vBytes(vArray);
src.get_vBytes(); // returns `void *`

src.add_point(inb::NO_COMPRESSION);
src.get_point().set_x(12.34f);
MyNS::Vec2 &point = src.get_point();
point.set_y(56.78f);
//TODO: check point.add_something_optional() -> where to store? in `src.buffer` because `src` is owner

src.add_vArray(8); // reserve 8 elements
src.set_vArray(&vArray);
src.set_vArray(1, 234); // `vArray[1] = 234;`
src.get_vArray(); // returns `int64_t *`
src.get_vArray(1); // returns `const int64_t &`

//TODO: MyNS::MyStruct::internal get/set by enum::id

// THERE IS NO NEED TO FINALIZE!

//      inb::NO_COMPRESSION
//TODO: inb::TABLE_COMPRESSION
//TODO: inb::FULL_COMPRESSION

MyNS::MyStruct dst;
dst.from(src.to()); // from `uint8_t *` and ZERO COST!
//dst.from(src.to(inb::TABLE_COMPRESSION)); // from `std::unique_ptr<uint8_t>` and cost for compression+decompression!
dst.get_vUint8(); // throw std::logic_error
dst.add_vUint8(); // has no effects
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
    dst.get_vArray(1, 234); // returns `int64 *`
}

//TODO: `src.remove_variable();` and `src.shrink();`. Is IndexBuffers a container or just serializer/deserializer?
