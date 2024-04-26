// Extension lib defines
#define LIB_NAME "BinaryPacker"
#define MODULE_NAME "binarypacker"

// include the Defold SDK
#include <dmsdk/sdk.h>
#include "parsers.cpp"
#include "writers.cpp"
#include "structs.cpp"

namespace dmBinaryPacker {
    const unsigned int readBufferRoundSize = 1024;

    char *readBuffer = nullptr;
    unsigned int readBufferPos = 0;
    unsigned int readBufferSize = 0;
    unsigned int readBufferMaxSize = 0;

    char *writeBuffer = nullptr;
    unsigned int writeBufferPos = 0;
    unsigned int writeBufferSize = 0;

    static void DestroyReadBuffer() {
        if (readBuffer != nullptr) {
            delete readBuffer;
            readBuffer = nullptr;
        }
    }

    static int SetReadBuffer(lua_State *L) {
        DM_LUA_STACK_CHECK(L, 0);
        size_t size;
        char *buffer = (char *) luaL_checklstring(L, 1, &size);
        if (readBuffer == nullptr || size > readBufferMaxSize) {
            DestroyReadBuffer();
            readBufferMaxSize = ((size - 1) / readBufferRoundSize + 1) * readBufferRoundSize;
            readBuffer = new char[readBufferMaxSize];
        }
        memcpy(readBuffer, buffer, size);
        readBufferPos = 0;
        readBufferSize = size;
        return 0;
    }

    static int GetDataFromReadBuffer(lua_State *L) {
        DM_LUA_STACK_CHECK(L, 1);
        int dataId;
        if (lua_isnoneornil(L, 1)) {
            dataId = ParseDataId(readBuffer, readBufferPos, readBufferSize);
        } else {
            dataId = (int) luaL_checkinteger(L, 1);
            if (readBuffer == nullptr) {
                dmLogError("Buffer not specified! Use a 'set_buffer' before calling the function!");
                return 0;
            }
        }
        ParseDataIntoStack(L, dataId, readBuffer, readBufferPos, readBufferSize);
        return 1;
    }

    static int CreateWriteBuffer(lua_State *L) {
        DM_LUA_STACK_CHECK(L, 0);
        delete writeBuffer;
        writeBufferSize = (unsigned int) luaL_checkinteger(L, 1);
        writeBuffer = new char[writeBufferSize];
        return 0;
    }

    static int ClearWriteBuffer(lua_State *L) {
        DM_LUA_STACK_CHECK(L, 0);
        writeBufferPos = 0;
        return 0;
    }

    static int PutDataIntoBuffer(lua_State *L) {
        DM_LUA_STACK_CHECK(L, 0);
        int dataId = (int) luaL_checkinteger(L, 1);
        if (dataId >= 0 && CanWriteIntoBuffer(writeBufferSize, writeBufferPos, SIZE_INT8)) {
            writeBuffer[writeBufferPos] = (unsigned char) dataId;
            writeBufferPos += SIZE_INT8;
        }
        PutDataFromStackIntoBuffer(L, 2, dataId, writeBuffer, writeBufferPos, writeBufferSize);
        return 0;
    }

    static int GetWriteBuffer(lua_State *L) {
        DM_LUA_STACK_CHECK(L, 1);
        if (writeBufferPos > 0) lua_pushlstring(L, writeBuffer, writeBufferPos);
        else lua_pushnil(L);
        return 1;
    }

    static const luaL_reg Module_methods[] = {
            {"set_read_buffer",     SetReadBuffer},
            {"get_data",            GetDataFromReadBuffer},
            {"create_write_buffer", CreateWriteBuffer},
            {"clear_write_buffer",  ClearWriteBuffer},
            {"put_data",            PutDataIntoBuffer},
            {"get_write_buffer",    GetWriteBuffer},
            {"register_structs",    RegisterStructs},
            {"unpack_buffer",       UnpackBuffer},
            {0,                     0}
    };

    static void LuaInit(lua_State *L) {
        int top = lua_gettop(L);
        luaL_register(L, MODULE_NAME, Module_methods);

        #define SETCONSTANT(name) lua_pushnumber(L, (lua_Number)name); lua_setfield(L, -2, #name);
        SETCONSTANT(DATA_INT8)
        SETCONSTANT(DATA_UINT8)
        SETCONSTANT(DATA_INT16)
        SETCONSTANT(DATA_UINT16)
        SETCONSTANT(DATA_INT32)
        SETCONSTANT(DATA_UINT32)
        SETCONSTANT(DATA_INT64)
        SETCONSTANT(DATA_FLOAT32)
        SETCONSTANT(DATA_FLOAT64)
        SETCONSTANT(DATA_STRING)
        SETCONSTANT(DATA_BYTES)
        #undef SETCONSTANT

        lua_pop(L, 1);
        assert(top == lua_gettop(L));
    }


    dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams *params) {
        dmLogInfo("AppInitializeBinaryPacker\n");
        return dmExtension::RESULT_OK;
    }

    dmExtension::Result InitializeMyExtension(dmExtension::Params *params) {
        // Init Lua
        LuaInit(params->m_L);
        dmLogInfo("Registered %s Extension\n", MODULE_NAME);
        return dmExtension::RESULT_OK;
    }

    dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams *params) {
        dmLogInfo("AppFinalizeBinaryPacker\n");
        return dmExtension::RESULT_OK;
    }

    dmExtension::Result FinalizeMyExtension(dmExtension::Params *params) {
        dmLogInfo("FinalizeBinaryPacker\n");
        return dmExtension::RESULT_OK;
    }
}

// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// MyExtension is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(BinaryPacker, LIB_NAME, dmBinaryPacker::AppInitializeMyExtension,
                     dmBinaryPacker::AppFinalizeMyExtension, dmBinaryPacker::InitializeMyExtension, NULL, NULL,
                     dmBinaryPacker::FinalizeMyExtension)
