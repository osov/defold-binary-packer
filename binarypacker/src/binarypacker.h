#ifndef BINARYPACKER_H
#define BINARYPACKER_H

namespace dmBinaryPacker {
    const int maxStructs = 256;
    const char structIdField[] = "struct_id";
    const char structInfoTableField[] = "struct";
    const char structInfoKeyField[] = "key";
    const char structInfoDataIdField[] = "data_id";
    const char structInfoIsArrayField[] = "is_array";
    const char messageIdField[] = "message_id";
    const char messageField[] = "message";

    struct structFieldInfo {
        char *key;
        int dataId;
        bool isArray;
    };

    struct structInfo {
        int countFields;
        structFieldInfo *fields;
    };

    int SIZE_INT8 = 1;
    int SIZE_INT16 = 2;
    int SIZE_INT32 = 4;
    int SIZE_INT64 = 8;
    int SIZE_FLOAT32 = 4;
    int SIZE_FLOAT64 = 8;

    enum TypeData {
        DATA_INT8 = -1,
        DATA_UINT8 = -2,
        DATA_INT16 = -3,
        DATA_UINT16 = -4,
        DATA_INT32 = -5,
        DATA_UINT32 = -6,
        DATA_INT64 = -7,
        DATA_FLOAT32 = -8,
        DATA_FLOAT64 = -9,
        DATA_STRING = -10
    };

    static void PutDataFromStackIntoBuffer(lua_State *L, int numArg, int dataId, char *buf, unsigned int &pos,
                                           unsigned int buffer_size);

    static void ParseDataIntoStack(lua_State *L, int dataId, const char *buf, unsigned int &pos, unsigned int size);
}

#endif //BINARYPACKER_H
