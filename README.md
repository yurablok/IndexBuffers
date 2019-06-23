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

* Опциональные поля
* Минимум файлов с кодом (по одному на схему)
* Минимум выделений памяти (TODO)
* Пакеты готовы к использованию в любой момент (TODO: без crc32?)
* TODO
* TODO: getMinimumSize(), getMaximumSize()
* TODO: Проверить возможность использования в Embedded.

## <a name="rus_schema_format"></a> Формат описания структуры данных

Пример описания схемы данных IndexBuffers:  
```rust
//File: "cheat_sheet.ibs"

include "additional.ibs" // Имеет namespace AdditionalNS
namespace ExtNS::IntNS

enum Color: uint8 {
    RED        // 0
    GREEN = 3  // 3
    BLUE       // 4
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

const uint32 fixedSize = 16 // Только атомарные типы

struct Arrays {
    optional uint8[16] f  // Массив фиксированного размера
    bytes[fixedSize]   b  // Массив байт фиксированного размера
    // Массивы динамического размера всегда являются "optional"
    int16[] m  // Массив целых чисел динамического размера
    Vec3f[] v  // Массив структур динамического размера
}

union Variant {
    Color color // Внутри "union", "optional" и значение по-умолчанию запрещёны
    Arrays data
}

struct AnotherOne {
    optional Variant        variant
    Color[Color.count]      colorChannels // Также есть ".min", ".max"
    Color                   color = Color::BLUE
    AdditionalNS::SomeType  someData      // Из файла "additional.ibs"
}
```

## <a name="rus_internal_format"></a> Описание внутреннего формата данных IndexBuffers

TODO

## <a name="rus_benchmarks"></a> Сравнение производительности и расхода памяти

TODO: сравнение с [Protocol Buffers](https://github.com/protocolbuffers/protobuf),
[FlatBuffers](https://github.com/google/flatbuffers)
