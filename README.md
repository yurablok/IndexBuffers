[ENG](#eng) [RUS](#rus)

# <a name="eng"></a> IndexBuffers

TODO

# <a name="rus"></a> IndexBuffers

Инструмент сериализации-десериализации данных. Позволяет подготавливать данные
для передачи/хранения во внутреннем формате IndexBuffers с минимальными накладными
расходами и удобным программным интерфейсом. Поддерживается генерация инструмента
под C++.

* [Принцип использования](#rus_principle_of_use)
* [Основные возможности](#rus_main_features)
* [Формат описания структуры данных](#rus_schema_format)
* [Описание внутреннего формата данных IndexBuffers](#rus_internal_format)
* [Сравнение производительности и расхода памяти](#rus_benchmarks)

## <a name="rus_principle_of_use"></a> Принцип использования  
1. Написание текстового файла `*.ibs` со [схемой данных](#rus_schema_format).
2. Геренация исходного кода с помощью компилятора IndexBuffers.
3. Включение и использование сгенерированного исходного кода в своём проекте.

## <a name="rus_main_features"></a> Основные возможности

* Собственный C-подобный язык описания схемы.
* Минимум файлов с кодом - по одному на схему.
* Минимум выделений памяти - можно использовать заренее выделенную память или
  использовать единственный для пакета внутренний буфер на `std::vector`.
* Пакеты готовы к использованию в любой момент (TODO: без crc32?, `crc32` или `mmr3`
  расчитываются при вызове `to`).
* TODO
* TODO: Проверить возможность использования в Embedded.
* Опциональные поля, пространства имён, .

## <a name="rus_schema_format"></a> Формат описания структуры данных

Пример описания схемы данных IndexBuffers:  
```rust
//File: "cheat_sheet.ibs"

include "../Tests/additional.ibs" // Имеет namespace AdditionalNS
namespace ExtNS::IntNS

enum Color: uint8 {
    RED       // 0
    GREEN = 3 // 3
    BLUE      // 4
}

struct ScalarTypes: crc32 {
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

struct Arrays {
    optional uint8 f[16] // Массив фиксированного размера
    bytes          b[fixedSize] // Массив байт фиксированного размера
    // Массивы динамического размера всегда являются "optional"
    int16          m[] // Массив целых чисел динамического размера
    Vec3f          v[] // Массив структур динамического размера
}

union Variant {
    Color color // Внутри "union", "optional" и значение по-умолчанию запрещены
    Arrays data
}

struct AnotherOne {
    optional Variant        variant
    Color                   colorChannels[Color.count] // Также есть ".min", ".max"
    Color                   colorMask = Color::BLUE
    AdditionalNS::SomeType  someData      // Из файла "additional.ibs"
}

```

## <a name="rus_internal_format"></a> Описание внутреннего формата данных IndexBuffers

TODO

## <a name="rus_benchmarks"></a> Сравнение производительности и расхода памяти

TODO: сравнение с
[Protocol Buffers](https://github.com/protocolbuffers/protobuf),
[FlatBuffers](https://github.com/google/flatbuffers),
[Cap'n Proto](https://github.com/capnproto/capnproto)

protobuf:     13693 ms
flatbuffers:  5105 ms
capnproto:    8345 ms
IndexBuffers: 7109 ms
