// UGUI.cpp
// Extension lib defines
#define LIB_NAME "UGUI"
#define MODULE_NAME "ugui"

// include the Defold SDK
#include <dmsdk/sdk.h>
#include "ugui.h"

static UG_GUI gui;

void UserSetPixel (UG_S16 x, UG_S16 y, UG_COLOR c) {
}

static int Test(lua_State *L)
{
    /* Configure uGUI */
    UG_Init(&gui, UserSetPixel , 480, 272);

    /* Draw text with uGUI */
    UG_FontSelect(&FONT_8X14);
    UG_ConsoleSetArea(0, 60, 480, 212);
    UG_ConsoleSetBackcolor(C_BLACK);
    UG_ConsoleSetForecolor(C_RED);
    UG_ConsolePutString("Beginning System Initialization...\n");
    UG_ConsoleSetForecolor(C_GREEN);
    UG_ConsolePutString("System Initialization Complete\n");

//     while (1 == 1) {
//         // Do stuff here I guess?
//     }
// 
    return 0;
}

static int Reverse(lua_State* L)
{
    // The number of expected items to be on the Lua stack
    // once this struct goes out of scope
    DM_LUA_STACK_CHECK(L, 1);

    // Check and get parameter string from stack
    char* str = (char*)luaL_checkstring(L, 1);

    // Reverse the string
    int len = strlen(str);
    for(int i = 0; i < len / 2; i++) {
        const char a = str[i];
        const char b = str[len - i - 1];
        str[i] = b;
        str[len - i - 1] = a;
    }

    // Put the reverse string on the stack
    lua_pushstring(L, str);

    // Return 1 item
    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"test", Test},
    {"reverse", Reverse},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeUGUI(dmExtension::AppParams* params)
{
    dmLogInfo("AppInitializeUGUI\n");
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeUGUI(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeUGUI(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeUGUI\n");
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeUGUI(dmExtension::Params* params)
{
    dmLogInfo("FinalizeUGUI\n");
    return dmExtension::RESULT_OK;
}

dmExtension::Result OnUpdateUGUI(dmExtension::Params* params)
{
    dmLogInfo("OnUpdateUGUI\n");
    return dmExtension::RESULT_OK;
}

void OnEventUGUI(dmExtension::Params* params, const dmExtension::Event* event)
{
    switch(event->m_Event)
    {
        case dmExtension::EVENT_ID_ACTIVATEAPP:
            dmLogInfo("OnEventUGUI - EVENT_ID_ACTIVATEAPP\n");
            break;
        case dmExtension::EVENT_ID_DEACTIVATEAPP:
            dmLogInfo("OnEventUGUI - EVENT_ID_DEACTIVATEAPP\n");
            break;
        case dmExtension::EVENT_ID_ICONIFYAPP:
            dmLogInfo("OnEventUGUI - EVENT_ID_ICONIFYAPP\n");
            break;
        case dmExtension::EVENT_ID_DEICONIFYAPP:
            dmLogInfo("OnEventUGUI - EVENT_ID_DEICONIFYAPP\n");
            break;
        default:
            dmLogWarning("OnEventUGUI - Unknown event id\n");
            break;
    }
}

// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// UGUI is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(UGUI, LIB_NAME, AppInitializeUGUI, AppFinalizeUGUI, InitializeUGUI, OnUpdateUGUI, OnEventUGUI, FinalizeUGUI)
