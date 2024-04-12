#ifndef BINARYPACKER_PARSERS_CPP
#define BINARYPACKER_PARSERS_CPP

#include <dmsdk/sdk.h>
#include "binarypacker.h"
#include "structs.cpp"

namespace dmBinaryPacker {
    static bool CanReadBuffer(unsigned int size, unsigned int pos, unsigned int need_size) {
        bool is_ok = pos + need_size <= size;
        if (!is_ok) dmLogError("Parsing error, unexpected end of readBuffer!");
        return is_ok;
    }

    static void ParseInt8(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_INT8)) {
            lua_pushnumber(L, (int8_t) buffer[pos]);
            pos += SIZE_INT8;
        } else lua_pushnil(L);
    }

    static void ParseUInt8(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_INT8)) {
            lua_pushnumber(L, (uint8_t) buffer[pos]);
            pos += SIZE_INT8;
        } else lua_pushnil(L);
    }

    static void ParseInt16(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_INT16)) {
            lua_pushnumber(L, *(int16_t *) &buffer[pos]);
            pos += SIZE_INT16;
        } else lua_pushnil(L);
    }

    static void ParseUInt16(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_INT16)) {
            lua_pushnumber(L, *(uint16_t *) &buffer[pos]);
            pos += SIZE_INT16;
        } else lua_pushnil(L);
    }

    static void ParseInt32(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_INT32)) {
            lua_pushnumber(L, *(int32_t *) &buffer[pos]);
            pos += SIZE_INT32;
        } else lua_pushnil(L);
    }

    static void ParseUInt32(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_INT32)) {
            lua_pushnumber(L, *(uint32_t *) &buffer[pos]);
            pos += SIZE_INT32;
        } else lua_pushnil(L);
    }

    static void ParseInt64(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_INT64)) {
            lua_pushnumber(L, *(int64_t *) &buffer[pos]);
            pos += SIZE_INT64;
        } else lua_pushnil(L);
    }

    static void ParseFloat32(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_FLOAT32)) {
            lua_pushnumber(L, *(float_t *) &buffer[pos]);
            pos += SIZE_FLOAT32;
        } else lua_pushnil(L);
    }

    static void ParseFloat64(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_FLOAT64)) {
            lua_pushnumber(L, *(double_t *) &buffer[pos]);
            pos += SIZE_FLOAT64;
        } else lua_pushnil(L);
    }

    static unsigned int ParseLength(const char *buffer, unsigned int &pos, unsigned int size) {
        if (CanReadBuffer(size, pos, SIZE_INT8)) {
            unsigned int length = (unsigned char) buffer[pos];
            pos += SIZE_INT8;
            if ((length & 0x80) == 0) return length;
            if (CanReadBuffer(size, pos, SIZE_INT8)) {
                auto twoByte = (unsigned char) buffer[pos];
                pos += SIZE_INT8;
                if ((twoByte & 0x80) != 0) dmLogError("Error parsing string or array length!");
                length = ((unsigned int) twoByte << 7) | (length & 0x7F);
                return length;
            }
        }
        return 0;
    }

    static void ParseString(lua_State *L, const char *buffer, unsigned int &pos, unsigned int size) {
        unsigned int length = ParseLength(buffer, pos, size);
        char *str = (char *) &buffer[pos];
        if (CanReadBuffer(size, pos, length)) {
            lua_pushlstring(L, str, length);
            pos += length;
        } else lua_pushnil(L);
    }

    static void ParseStruct(lua_State *L, int structId, const char *buffer, unsigned int &pos, unsigned int size) {
        lua_newtable(L);
        auto structData = GetStructInfo(structId);
        if (structData == nullptr) return;
        for (int i = 0; i < structData->countFields; i++) {
            auto fieldData = structData->fields[i];
            if (fieldData.isArray) {
                auto length = ParseLength(buffer, pos, size);
                lua_newtable(L);
                for (int index = 1; index <= length; index++) {
                    ParseDataIntoStack(L, fieldData.dataId, buffer, pos, size);
                    lua_rawseti(L, -2, index);
                }
                lua_setfield(L, -2, fieldData.key);
            } else {
                ParseDataIntoStack(L, fieldData.dataId, buffer, pos, size);
                lua_setfield(L, -2, fieldData.key);
            }
        }
    }

    static int ParseDataId(const char *buf, unsigned int &pos, unsigned int size) {
        int dataId = -1;
        if (CanReadBuffer(size, pos, SIZE_INT8)) {
            dataId = (unsigned char) buf[pos];
            pos += SIZE_INT8;
        }
        return dataId;
    }

    static void ParseDataIntoStack(lua_State *L, int dataId, const char *buf, unsigned int &pos, unsigned int size) {
        switch (dataId) {
            case DATA_INT8:
                ParseInt8(L, buf, pos, size);
                break;
            case DATA_UINT8:
                ParseUInt8(L, buf, pos, size);
                break;
            case DATA_INT16:
                ParseInt16(L, buf, pos, size);
                break;
            case DATA_UINT16:
                ParseUInt16(L, buf, pos, size);
                break;
            case DATA_INT32:
                ParseInt32(L, buf, pos, size);
                break;
            case DATA_UINT32:
                ParseUInt32(L, buf, pos, size);
                break;
            case DATA_INT64:
                ParseInt64(L, buf, pos, size);
                break;
            case DATA_FLOAT32:
                ParseFloat32(L, buf, pos, size);
                break;
            case DATA_FLOAT64:
                ParseFloat64(L, buf, pos, size);
                break;
            case DATA_STRING:
                ParseString(L, buf, pos, size);
                break;
            default:
                if (dataId >= 0) ParseStruct(L, dataId, buf, pos, size);
                else {
                    dmLogError("Parsing error, unknown data type!");
                    lua_pushnil(L);
                }
        }
    }

    static int UnpackBuffer(lua_State *L) {
        DM_LUA_STACK_CHECK(L, 1);
        size_t size;
        char *buffer = (char *) luaL_checklstring(L, 1, &size);
        unsigned int position = 0;
        lua_newtable(L);
        int index = 1;
        while (position < size) {
            int dataId = ParseDataId(buffer, position, size);
            if (dataId >= 0) {
                lua_newtable(L);
                lua_pushnumber(L, dataId);
                lua_setfield(L, -2, messageIdField);
                ParseDataIntoStack(L, dataId, buffer, position, size);
                lua_setfield(L, -2, messageField);
                lua_rawseti(L, -2, index++);
            } else break;
        }
        return 1;
    }
}

#endif //BINARYPACKER_PARSERS_CPP
