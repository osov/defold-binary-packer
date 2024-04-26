---@class binarypacker_struct_info
---@field key string Ключ в таблице с данными
---@field data_id number Идентификатор данных или структуры
---@field is_array boolean|nil Данные являются массивом

---@class binarypacker_struct
---@field struct_id number Идентификатор структуры
---@field struct binarypacker_struct_info[] Описание структуры

---@class binarypacker_unpacked_buffer
---@field message_id number
---@field message table

---@class binarypacker
---@field DATA_INT8 number Тип данных 8 битное число со знаком
---@field DATA_UINT8 number Тип данных 8 битное число без знака
---@field DATA_INT16 number Тип данных 16 битное число со знаком
---@field DATA_UINT16 number Тип данных 16 битное число без знака
---@field DATA_INT32 number Тип данных 32 битное число со знаком
---@field DATA_UINT32 number Тип данных 32 битное число без знака
---@field DATA_INT64 number Тип данных 64 битное число со знаком
---@field DATA_FLOAT32 number Тип данных 32 битное число с плавающей запятой
---@field DATA_FLOAT64 number Тип данных 64 битное число с плавающей запятой
---@field DATA_STRING number Тип данных строка
---@field DATA_BYTES number Тип данных строка(любой набор данных)
---@field register_structs fun(struct: binarypacker_struct[]) Массив с описанием структур
---@field create_write_buffer fun(size: number) Создать буфер для записи
---@field put_data fun(data_id: number, data: any) Поместить данные в буфер записи
---@field clear_write_buffer fun() Очистка буфера записи
---@field get_write_buffer fun():string Получить буфер записи в виде строки
---@field set_read_buffer fun(buffer:string) Установить буфер для парсинга
---@field get_data fun(data_id:number|nil):any Получить данные из буфера, если data_type == nil, то будет попытка распаковать структуру
---@field unpack_buffer fun(buffer:string):binarypacker_unpacked_buffer[] Распаковать буфер данных, функция возвращает массив с данными

---@type binarypacker
binarypacker = {}
