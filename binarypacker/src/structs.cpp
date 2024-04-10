#ifndef BINARYPACKER_STRUCT_CPP
#define BINARYPACKER_STRUCT_CPP

#include <dmsdk/sdk.h>
#include "binarypacker.h"

namespace dmBinaryPacker {
    dmArray<structInfo *> structs;

    static structInfo *GetStructInfo(int structId) {
        if (structId >= structs.Size()) {
            dmLogError("The structure with identifier '%i' is not defined!", structId);
            return nullptr;
        }
        auto structData = structs[structId];
        if (structData == nullptr) dmLogError("The structure with identifier '%i' is not defined!", structId);
        return structData;
    }

    static char *GetFieldKey(lua_State *L, size_t &keyLength) {
        lua_getfield(L, -1, structInfoKeyField);
        if (!lua_isstring(L, -1)) {
            dmLogError("Field '%s' must by string!", structInfoKeyField);
            return nullptr;
        }
        char *key = (char *) luaL_checklstring(L, -1, &keyLength);
        lua_pop(L, 1);
        return key;
    }

    static int GetFieldDataId(lua_State *L) {
        lua_getfield(L, -1, structInfoDataIdField);
        if (!lua_isnumber(L, -1)) dmLogError("Field '%s' must by integer!", structInfoDataIdField);
        int dataId = (int) luaL_checkinteger(L, -1);
        lua_pop(L, 1);
        return dataId;
    }

    static bool GetFieldIsArray(lua_State *L) {
        lua_getfield(L, -1, structInfoIsArrayField);
        bool isArray = false;
        if (!lua_isnil(L, -1)) {
            if (!lua_isboolean(L, -1)) dmLogError("Field '%s' must by boolean!", structInfoIsArrayField);
            isArray = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
        return isArray;
    }

    static void ParseStructInfo(lua_State *L, int structId) {
        if (structId >= structs.Size()) structs.SetSize(structId + 1);
        else if (structs[structId] != nullptr) {
            dmLogError("The structure with identifier %i is already registered!", structId);
            return;
        }
        int countFields = (int) lua_objlen(L, -1);
        auto *structData = new structInfo();
        structData->countFields = countFields;
        structData->fields = new structFieldInfo[countFields];
        structs[structId] = structData;
        auto fields = structData->fields;
        for (int i = 0; i < countFields; i++) {
            lua_rawgeti(L, -1, i + 1);
            if (!lua_istable(L, -1)) dmLogError("The structure field description must be a table!");
            size_t keyLength = 0;
            char *key = GetFieldKey(L, keyLength);
            fields[i].key = new char[keyLength + 1];
            strcpy(fields[i].key, key);
            fields[i].dataId = GetFieldDataId(L);
            fields[i].isArray = GetFieldIsArray(L);
            lua_pop(L, 1);
        }
    }

    static int RegisterStructs(lua_State *L) {
        DM_LUA_STACK_CHECK(L, 0);
        luaL_checktype(L, 1, LUA_TTABLE);
        if (structs.Capacity() < maxStructs) structs.SetCapacity(maxStructs);
        int countStructs = (int) lua_objlen(L, 1);
        for (int i = 1; i <= countStructs; i++) {
            lua_rawgeti(L, 1, i);
            if (!lua_istable(L, -1)) dmLogError("The structure description must be a table!");

            lua_getfield(L, -1, structIdField);
            if (!lua_isnumber(L, -1)) dmLogError("Field '%s' must by integer!", structIdField);
            int structId = (int) luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, structInfoTableField);
            if (!lua_istable(L, -1)) dmLogError("The description of the list of structure fields must be a table!");
            ParseStructInfo(L, structId);
            lua_pop(L, 1);

            lua_pop(L, 1);
        }
        return 0;
    }
}

#endif //BINARYPACKER_STRUCT_CPP
