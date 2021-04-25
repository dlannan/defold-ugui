// UGUI.cpp
// Extension lib defines
#define LIB_NAME "UGUI"
#define MODULE_NAME "ugui"


// include the Defold SDK
#include <dmsdk/sdk.h>
#include "ugui.h"

#include <stdio.h>
#include <stdlib.h>

#define     CONSOLE_HEIGHT         212
#define     MAX_WINDOW_OBJECTS     50

const UG_FONT    *fonts[] = {
    &FONT_8X8,
    &FONT_8X12,
    &FONT_8X14,
    &FONT_12X20,
    &FONT_22X36,
    &FONT_24X40
};


static UG_GUI gui;
// Internal renderbuffer - this will probably change!
static UG_COLOR *renderbuffer   = (UG_COLOR *)NULL;
static int renderWidth          = 640;
static int renderHeight         = 480;

void UserSetPixel (UG_S16 x, UG_S16 y, UG_COLOR c) {
    assert(renderbuffer);
    renderbuffer[ y * renderWidth + x ] = c;
}

void all_windows_callback( UG_MESSAGE *msg )
{
}

static int WindowCreate(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    UG_WINDOW     *window = (UG_WINDOW *)malloc(sizeof(UG_WINDOW));
    UG_OBJECT     *win_objs = (UG_OBJECT *)malloc(sizeof(UG_OBJECT) * MAX_WINDOW_OBJECTS);
    UG_WindowCreate( window, win_objs, MAX_WINDOW_OBJECTS, all_windows_callback );
    lua_pushlightuserdata(L, window);
    return 1;
}

static int ButtonCreate(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 1);
    UG_WINDOW * window = (UG_WINDOW *)lua_touserdata(L, 1);
    int id          = luaL_checknumber(L, 2);
    int xpos        = luaL_checknumber(L, 3);
    int ypos        = luaL_checknumber(L, 4);
    int width       = luaL_checknumber(L, 5);
    int height      = luaL_checknumber(L, 6);
    
    UG_BUTTON *btn = (UG_BUTTON *)malloc(sizeof(UG_BUTTON));
    UG_ButtonCreate( window, btn, id, xpos, ypos, xpos + width, ypos + height);
    lua_pushlightuserdata(L, btn);
    return 1;
}

static int WindowShow(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    UG_WINDOW * window = (UG_WINDOW *)lua_touserdata(L, 1);
    UG_WindowShow( window );
    return 0;
}

static int SetText(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    UG_WINDOW * window  = (UG_WINDOW *)lua_touserdata(L, 1);
    int id              = luaL_checknumber(L, 2);
    int objtype         = luaL_checknumber(L, 3);
    char *text          = (char *)luaL_checkstring(L, 4);

    switch(objtype) {
        case 0:     // Window
            UG_WindowSetTitleText(window, text);
        break;
        case 1:     // button
            UG_ButtonSetText(window, id, text);
        break;
        case 2:     // textbox
            UG_TextboxSetText(window, id, text);
        break;
        default:
        break;
    }
    return 0;
}

static int SetFont(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    int id              = luaL_checknumber(L, 1);
    UG_FontSelect(fonts[id]);
    return 0;
}


static int Init(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    int width       = luaL_checknumber(L, 1);
    int height      = luaL_checknumber(L, 2);

    renderWidth     = width;
    renderHeight    = height;
    if(renderbuffer != NULL) free(renderbuffer);
    renderbuffer = (UG_COLOR*)malloc(width * height * sizeof(UG_COLOR));
        
    /* Configure uGUI */
    UG_Init(&gui, UserSetPixel , width, height);

    /* Draw text with uGUI */
    /* TODO: Add a few good defaults. */
    UG_FontSelect(&FONT_8X14);
    
    UG_ConsoleSetArea(0, 60, width, CONSOLE_HEIGHT);
    UG_ConsoleSetBackcolor(C_BLACK);
    UG_ConsoleSetForecolor(C_RED);
    UG_ConsolePutString("Beginning System Initialization...\n");
    UG_ConsoleSetForecolor(C_GREEN);
    UG_ConsolePutString("System Initialization Complete\n");
    return 0;
}

static int UpdateBufferBytes(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    dmScript::LuaHBuffer *buffer = dmScript::CheckBuffer(L, 1);
    const char *streamname = luaL_checkstring(L, 2);

    uint8_t* bytes = 0x0;
    uint32_t size = 0;
    uint32_t count = 0;
    uint32_t components = 0;
    uint32_t stride = 0;
    dmBuffer::Result r = dmBuffer::GetStream(buffer->m_Buffer, dmHashString64(streamname), (void**)&bytes, &count, &components, &stride);

    size_t idx = 0;
    if (r == dmBuffer::RESULT_OK) {
        for (int i = 0; i < count; ++i)
        {
            uint32_t     data = renderbuffer[idx];
            bytes[0] = (data >> 16) & 0xff;
            bytes[1] = (data >> 8) & 0xff;
            bytes[2] = (data >> 0) & 0xff;
            bytes[3] = (data >> 24) & 0xff;
            idx++;
            bytes += stride;
        }
    } else {
        // handle error
    }

    r = dmBuffer::ValidateBuffer(buffer->m_Buffer);
    return 0;
}

static int Update(lua_State* L) 
{
    DM_LUA_STACK_CHECK(L, 0);
    UG_Update();
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
    {"windowcreate", WindowCreate },
    {"buttoncreate", ButtonCreate },
    {"windowshow", WindowShow },

    {"settext", SetText },
    {"setfont", SetFont },
    
    {"update", Update },
    {"updatebufferbytes", UpdateBufferBytes },
    {"init", Init},
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
    //dmLogInfo("OnUpdateUGUI\n");
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
