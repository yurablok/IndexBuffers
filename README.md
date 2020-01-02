[ENG](#eng) [RUS](#rus)

# <a name="eng"></a> IndexBuffers

Data serialization-deserialization tool. Allows you to prepare data for
transfer/storage in the internal IndexBuffers format with minimal overhead
and a convenient API. Supports tool generation for C++.

* [Principle of use](#eng_principle_of_use)
* [Benefits and features](#eng_benefits_and_features)
* [Schema format](#eng_schema_format)
* [Description of API of the generated source code (TODO)](#eng_cpp_api)
* [Description of IndexBuffers internal data format (TODO)](#eng_internal_format)
* [Comparison with alternatives](#eng_benchmarks)

## [↑](#eng) <a name="eng_principle_of_use"></a> Principle of use

1. Writing a `*.ibs` text file with a [data scheme](#eng_schema_format).
2. Generating of [source code](#eng_cpp_api) by using the IndexBuffers compiler.
3. Inclusion and use of the generated source code in your project.

## [↑](#eng) <a name="eng_benefits_and_features"></a> Benefits and features

* The most simple and intuitive API of the generated code.
* Minimum of source code - one per schema, without external dependencies other than STL.
* Minimum of memory allocations - you can use previously allocated memory or use
  the only internal buffer based on `std::vector` for the packet.
* Zero memory overhead if all fields are fixed.
* Packages are ready for use at any time after the call `create` or `from`.
* Native C-like schema description language.
* Support for optional fields, namespaces, constants, enums, structs, unions.
* Schemes must be UTF-8 encoded.
* Alignment is not currently implemented, but you can manually arrange the
  schema's fields in the desired order.
* Converting Little-Endian <> Big-Endian is not currently implemented.

## [↑](#eng) <a name="eng_schema_format"></a> Schema format

IndexBuffers schema language supports:
- specifying of namespaces
- inclusion of other schema files
- enums with specific values, as well as the use of enums parameters
  as constants (min, max, count)
- constants of different formats (bin, dec, hex, float, scientific float)
- structs with regular fields, optional fields, fixed and variable length
  arrays, default values
- unions for storing one of the fields in one cell

The detailed description of the schema format you can see [here (TODO)](#eng_full_schema_format).  
IndexBuffers schema example:  
```rust
//File: "cheat_sheet.ibs"

include "additional.ibs" // Has namespace AdditionalNS
namespace ExtNS::IntNS

enum Color: uint8 {
    RED       // 0
    GREEN = 3 // 3
    BLUE      // 4
}

struct ScalarTypes: uint8 {
    int8            a
    uint16          b
    optional int32  c
    optional uint64 d
    float32         e
    float64         f
}

struct Vec3f: no_header
{
    float32 x = 0 // When specifying a default value, "optional" is not allowed
    float32 y = 0
    float32 z = 0
}

const uint32 fixedSize = 16 // Atomic types only, including enum

struct Arrays: uint16 {
    optional uint8 f[16] // Fixed size array
    bytes          b[fixedSize] // Fixed size array of bytes
    // Dynamic size arrays are always "optional"
    int16          m[] // Dynamic size array of integers
    Vec3f          v[]: uint8 // Dynamic size array of structs
}

union Variant: uint8 {
    Color  color // Inside "union", "optional" and a default value are not allowed
    Arrays data
    int64  arr[]
}

struct AnotherOne {
    bytes                   string // bytes is always an array
    optional Variant        variant
    Color                   colorChannels[Color.count] // Also exists ".min", ".max"
    Color                   colorMask = Color::BLUE
    AdditionalNS::SomeType  someData // From "additional.ibs"
}
```

## [↑](#eng) <a name="eng_benchmarks"></a> Comparison with alternatives

|                             Tool                                |  Time, ms |  %              |     Size of packet, B | Size of code, KB |
| :-------------------------------------------------------------: | --------: | :-------------: | --------------------: | ---------------: |
| [Protocol Buffers](https://github.com/protocolbuffers/protobuf) |     13693 | 268             |                   106 |  external + 92.9 |
|   [FlatBuffers](https://github.com/google/flatbuffers)          |      5105 | 100             |                   296 |  external + 19.8 |
|   [Cap'n Proto](https://github.com/capnproto/capnproto)         |      8345 | 163             |                   151 |  external + 50.7 |
|    IndexBuffers                                                 |      7109 | 139<sub>1</sub> | 91(uint8) 114(uint16) | 45.7<sub>2</sub> |

1. To make IndexBuffers more faster, need to work out the issue of aligning the fields.
2. Unlike alternative tools, the generated IndexBuffers code has no dependencies  
   on either additional tool header files or additional static tool libraries  
   (like Protocol Buffers and Cap'n Proto).

# <a name="rus"></a> IndexBuffers

Инструмент сериализации-десериализации данных. Позволяет подготавливать данные
для передачи/хранения во внутреннем формате IndexBuffers с минимальными накладными
расходами и удобным программным интерфейсом. Поддерживается генерация инструмента
под C++.

* [Принцип использования](#rus_principle_of_use)
* [Преимущества и характеристики](#rus_benefits_and_features)
* [Формат описания схемы данных](#rus_schema_format)
* [Описание API сгенерированного кода (TODO)](#rus_cpp_api)
* [Описание внутреннего формата данных IndexBuffers (TODO)](#rus_internal_format)
* [Сравнение с альтернативными инструментами](#rus_benchmarks)

## [↑](#rus) <a name="rus_principle_of_use"></a> Принцип использования

1. Написание текстового файла `*.ibs` со [схемой данных](#rus_schema_format).
2. Генерирование [исходного кода](#rus_cpp_api) с помощью компилятора IndexBuffers.
3. Включение и использование сгенерированного исходного кода в своём проекте.

## [↑](#rus) <a name="rus_benefits_and_features"></a> Преимущества и характеристики

* Максимально простой и интуитивно понятный API сгенерированного кода.
* Минимум файлов с кодом - по одному на схему, без внешних зависимостей кроме STL.
* Минимум выделений памяти - можно использовать заренее выделенную память или
  использовать единственный для пакета внутренний буфер на `std::vector`.
* Нулевые накладные расходы по памяти, если все поля фиксированные.
* Пакеты готовы к использованию в любой момент после вызова `create` или `from`.
* Собственный C-подобный язык описания схемы.
* Поддержка опциональных полей, пространств имён, констант, перечислений,
  структур, объединений.
* Схемы данных должны иметь кодировку UTF-8.
* Выравнивание на данный момент не реализовано, но можно вручную располагать
  поля в нужном порядке.
* Конвертация Little-Endian <> Big-Endian на данный момент не реализована.

## [↑](#rus) <a name="rus_schema_format"></a> Формат описания схемы данных

Язык описания схемы данных IndexBuffers поддерживает:
- указание пространств имён
- включение других файлов со схемами данных
- перечисления с указанием специфических значений, а также использование
  параметров перечисления в качестве констант (min, max, count)
- константы разных форматов (bin, dec, hex, float, scientific float)
- структуры с обычными полями, опциональными полями, массивами фиксированной
  и переменной длины, значениями по-умолчанию
- объединения для хранения одного из вариантов полей в одной ячейке

Подробное описания формата схемы данных можно увидеть [здесь (TODO)](#rus_full_schema_format).  
Пример описания схемы данных IndexBuffers:  
```rust
//File: "cheat_sheet.ibs"

include "additional.ibs" // Имеет namespace AdditionalNS
namespace ExtNS::IntNS

enum Color: uint8 {
    RED       // 0
    GREEN = 3 // 3
    BLUE      // 4
}

struct ScalarTypes: uint8 {
    int8            a
    uint16          b
    optional int32  c
    optional uint64 d
    float32         e
    float64         f
}

struct Vec3f: no_header
{
    float32 x = 0 // При указании значения по-умолчанию, "optional" запрещён
    float32 y = 0
    float32 z = 0
}

const uint32 fixedSize = 16 // Только атомарные типы, включая enum

struct Arrays: uint16 {
    optional uint8 f[16] // Массив фиксированного размера
    bytes          b[fixedSize] // Массив байт фиксированного размера
    // Массивы динамического размера всегда являются "optional"
    int16          m[] // Массив целых чисел динамического размера
    Vec3f          v[]: uint8 // Массив структур динамического размера
}

union Variant: uint8 {
    Color  color // Внутри "union", "optional" и значение по-умолчанию запрещены
    Arrays data
    int64  arr[]
}

struct AnotherOne {
    bytes                   string // bytes всегда массив
    optional Variant        variant
    Color                   colorChannels[Color.count] // Также есть ".min", ".max"
    Color                   colorMask = Color::BLUE
    AdditionalNS::SomeType  someData // Из файла "additional.ibs"
}
```

## [↑](#rus) <a name="rus_benchmarks"></a> Сравнение с альтернативными инструментами

|                           Инструмент                            | Время, мс |  %              |      Размер пакета, б |  Размер кода, Кб |
| :-------------------------------------------------------------: | --------: | :-------------: | --------------------: | ---------------: |
| [Protocol Buffers](https://github.com/protocolbuffers/protobuf) |     13693 | 268             |                   106 |   внешний + 92.9 |
|   [FlatBuffers](https://github.com/google/flatbuffers)          |      5105 | 100             |                   296 |   внешний + 19.8 |
|   [Cap'n Proto](https://github.com/capnproto/capnproto)         |      8345 | 163             |                   151 |   внешний + 50.7 |
|    IndexBuffers                                                 |      7109 | 139<sub>1</sub> | 91(uint8) 114(uint16) | 45.7<sub>2</sub> |

1. Чтобы сделать IndexBuffers ещё быстрее, нужно проработать вопрос  
   выравнивания полей.
2. В отличии от альтернативных инструментов, у сгенерированного кода  
   IndexBuffers нет зависимостей ни от дополнительных заголовочных  
   файлов инструмента, ни от дополнительных статических библиотек  
   инстумента (как у Protocol Buffers и Cap'n Proto).
