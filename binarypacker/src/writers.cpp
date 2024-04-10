#ifndef BINARYPACKER_WRITERS_CPP
#define BINARYPACKER_WRITERS_CPP

#include <dmsdk/sdk.h>
#include "binarypacker.h"
#include "structs.cpp"

namespace dmBinaryPacker {
    static bool CanWriteIntoBuffer(unsigned int buffer_size, unsigned int pos, unsigned int need_size) {
        bool is_ok = pos + need_size <= buffer_size;
        if (!is_ok) dmLogError("Write error, buffer full!");
        return is_ok;
    }

    static void PutInt8(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_INT8)) {
            buffer[pos] = (int8_t) luaL_checkinteger(L, numArg);
            pos += SIZE_INT8;
        }
    }

    static void PutUInt8(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_INT8)) {
            buffer[pos] = (uint8_t) luaL_checkinteger(L, numArg);
            pos += SIZE_INT8;
        }
    }

    static void PutInt16(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_INT16)) {
            auto value = (int16_t) luaL_checkinteger(L, numArg);
            memcpy(&buffer[pos], &value, SIZE_INT16);
            pos += SIZE_INT16;
        }
    }

    static void PutUInt16(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_INT16)) {
            auto value = (uint16_t) luaL_checkinteger(L, numArg);
            memcpy(&buffer[pos], &value, SIZE_INT16);
            pos += SIZE_INT16;
        }
    }

    static void PutInt32(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_INT32)) {
            auto value = (int32_t) luaL_checkinteger(L, numArg);
            memcpy(&buffer[pos], &value, SIZE_INT32);
            pos += SIZE_INT32;
        }
    }

    static void PutUInt32(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_INT32)) {
            auto value = (uint32_t) luaL_checkinteger(L, numArg);
            memcpy(&buffer[pos], &value, SIZE_INT32);
            pos += SIZE_INT32;
        }
    }

    static void PutInt64(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_INT64)) {
            auto value = (int64_t) luaL_checknumber(L, numArg);
            memcpy(&buffer[pos], &value, SIZE_INT64);
            pos += SIZE_INT64;
        }
    }

    static void PutFloat32(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_FLOAT32)) {
            auto value = (float_t) luaL_checknumber(L, numArg);
            memcpy(&buffer[pos], &value, SIZE_FLOAT32);
            pos += SIZE_FLOAT32;
        }
    }

    static void PutFloat64(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if (CanWriteIntoBuffer(buffer_size, pos, SIZE_FLOAT64)) {
            auto value = (double_t) luaL_checknumber(L, numArg);
            memcpy(&buffer[pos], &value, SIZE_FLOAT64);
            pos += SIZE_FLOAT64;
        }
    }

    static void PutInt(unsigned int value, bool twoByte, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        int length = twoByte ? 2 : 1;
        if (CanWriteIntoBuffer(buffer_size, pos, length)) {
            memcpy(&buffer[pos], &value, length);
            pos += length;
        }
    }

    static void PutLength(unsigned int length, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        if ((length & ~0x7FFF) != 0) dmLogError("The length of the string or array is out of range!");
        if ((length & ~0x007F) == 0) PutInt(length, false, buffer, pos, buffer_size);
        else {
            length = ((length << 1) & ~0xFF) | (length & 0x007F) | 0x80;
            PutInt(length, true, buffer, pos, buffer_size);
        }
    }

    static void PutString(lua_State *L, int numArg, char *buffer, unsigned int &pos, unsigned int buffer_size) {
        size_t length = 0;
        char *str = (char *) luaL_checklstring(L, numArg, &length);
        PutLength(length, buffer, pos, buffer_size);
        if (length > 0 && CanWriteIntoBuffer(buffer_size, pos, length)) {
            memcpy(&buffer[pos], str, length);
            pos += length;
        }
    }

    static void PutStruct(lua_State *L, int numArg, int structId, char *buffer, unsigned int &pos,
                          unsigned int buffer_size) {
        luaL_checktype(L, numArg, LUA_TTABLE);
        auto structData = GetStructInfo(structId);
        if (structData == nullptr) return;
        if (!lua_istable(L, numArg)) {
            dmLogError("The structure must be a table!");
            return;
        }
        for (int i = 0; i < structData->countFields; i++) {
            auto fieldData = structData->fields[i];
            lua_getfield(L, numArg, fieldData.key);
            if (lua_isnil(L, -1)) dmLogError("Field '%s' not found in the data table!", fieldData.key);
            auto dataId = fieldData.dataId;
            if (fieldData.isArray) {
                if (!lua_istable(L, -1)) dmLogError("The '%s' field must be a table (array)!", fieldData.key);
                int lengthArray = (int) lua_objlen(L, -1);
                PutLength(lengthArray, buffer, pos, buffer_size);
                for (int index = 1; index <= lengthArray; index++) {
                    lua_rawgeti(L, -1, index);
                    PutDataFromStackIntoBuffer(L, -1, dataId, buffer, pos, buffer_size);
                    lua_pop(L, 1);
                }
            } else {
                PutDataFromStackIntoBuffer(L, -1, dataId, buffer, pos, buffer_size);
            }
            lua_pop(L, 1);
        }
    }

    static void PutDataFromStackIntoBuffer(lua_State *L, int numArg, int dataId, char *buf, unsigned int &pos,
                                           unsigned int buffer_size) {
        switch (dataId) {
            case DATA_INT8:
                PutInt8(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_UINT8:
                PutUInt8(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_INT16:
                PutInt16(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_UINT16:
                PutUInt16(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_INT32:
                PutInt32(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_UINT32:
                PutUInt32(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_INT64:
                PutInt64(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_FLOAT32:
                PutFloat32(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_FLOAT64:
                PutFloat64(L, numArg, buf, pos, buffer_size);
                break;
            case DATA_STRING:
                PutString(L, numArg, buf, pos, buffer_size);
                break;
            default:
                if (dataId >= 0) PutStruct(L, numArg, dataId, buf, pos, buffer_size);
                else
                    dmLogError("Error writing to buffer, unknown data type!");
        }
    }
}

#endif //BINARYPACKER_WRITERS_CPP
