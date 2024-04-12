# BinaryPacker extension for Defold

## Типы данных:

- DATA_INT8
- DATA_UINT8
- DATA_INT16
- DATA_UINT16
- DATA_INT32
- DATA_UINT32
- DATA_INT64
- DATA_FLOAT32
- DATA_FLOAT64
- DATA_STRING

### function register_structs(structs)

Регистрация структур данных

- structs: массив

## Функции для создания буфера:

### function create_write_buffer(size)

Создать буфер для записи

- size: размер буфера

### function clear_write_buffer()

Очистка буфера записи

### function put_data(data_id, value)

Поместить данные в буфер записи

- data_id: тип данных, константа DATA_XXX
- value: данные

### function get_write_buffer()

Получить буфер записи в виде строки

## Функции парсинга буфера:

### function set_read_buffer(buffer)

Установить буфер для парсинга

- buffer: строка

### function get_data(data_id)

Получить данные из буфера, если data_id == nil, то будет попытка распаковать структуру

- data_id: тип данных, константа DATA_XXX

### unpack_buffer(buffer)

Распаковать буфер данных, функция возвращает массив с данными:

```lua
local data = {
    {
        message_id = 1,
        message = {}
    },
    {
        message_id = 2,
        message = {}
    },
} 
```
