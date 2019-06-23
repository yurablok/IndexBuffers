/*** **************************************** ***
 **                 schema.ibs                 **
 *** **************************************** ***/
namespace ExtNS.IntNS

enum Color : uint8
{
    RED
    GREEN = 3
    BLUE
}

struct Vec2f
{
    x : float32
    y : float32
}

struct Packet :
    crc32
{
    id      : uint64 // comment
    vBytes  : bytes // always optional
    vUint8  : uint8 optional
    points  : Vec2[] // always optional
    vArray  : int64[] // arrays are optional by default
}
/*** **************************************** ***
 **           how using the compiler           **
 *** **************************************** ***/
inbc -i schema.inb -o schema_generated.h
inbc --input schema.inb --output schema_generated.h --detailed 1
inbc -i schema.inb // output to schema_generated.h by default

/*** **************************************** ***
 **                  main.cpp                  **
 *** **************************************** ***/
MyNS::MyStruct src; // calls src.create(0);
//MyNS::MyStruct src(12345); // calls src.create(12345);
src.create(12345); // reserve size in bytes

src.has_vUint8(); // returns false
src.set_vUint8(123);
src.get_vUint8(); // returns `const uint8_t &`
src.has_vUint8(); // returns true

src.has_vUint32(); // always returns true
src.set_vUint32(1234567);

MyNS::MyStruct dst;
dst.from(src.to()); // from `uint8_t *` and ZERO COST!
dst.get_vUint8(); // throw std::logic_error
if (dst.has_vUint8())
    dst.get_vUint8();

//TODO: `src.remove_variable();` and `src.shrink();`. Is IndexBuffers a container or just serializer/deserializer?
