// Copyright 2020-2022 David Colson. All rights reserved.

#include "Bind_Scene.h"

#include <string>

#include "Scene.h"

namespace Bind
{
    // ***********************************************************************

    void CheckAndInitPropertyTable(lua_State* pLua, Node* pNode)
    {
        lua_getfield(pLua, LUA_REGISTRYINDEX, "_nodePropTables");
        lua_pushinteger(pLua, pNode->m_id);
        lua_gettable(pLua, -2);

        if (lua_type(pLua, -1) != LUA_TTABLE)
        {
            lua_pop(pLua, 1);
            lua_newtable(pLua);

            // Two properties provided by c++
            if (pNode->m_meshId != UINT32_MAX)
            {
                lua_pushinteger(pLua, pNode->m_meshId + 1);
                lua_setfield(pLua, -2, "meshId");
            }

            lua_pushlstring(pLua, pNode->m_name.data(), pNode->m_name.size());
            lua_setfield(pLua, -2, "name");

            lua_pushinteger(pLua, pNode->m_id);
            lua_pushvalue(pLua, -2);
            lua_settable(pLua, -4); // Sets _nodePropTables[nodeId] = newTable (and leaves new table on the stack)
        }
        lua_pop(pLua, 2);
    }

    // ***********************************************************************

    int Node_GetLocalPosition(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");

        Vec3f pos = pNode->GetLocalPosition();
        lua_pushnumber(pLua, pos.x);
        lua_pushnumber(pLua, pos.y);
        lua_pushnumber(pLua, pos.z);
        return 3;
    }

    // ***********************************************************************

    int Node_GetWorldPosition(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");

        Vec3f pos = pNode->GetWorldPosition();
        lua_pushnumber(pLua, pos.x);
        lua_pushnumber(pLua, pos.y);
        lua_pushnumber(pLua, pos.z);
        return 3;
    }

    // ***********************************************************************

    int Node_SetLocalPosition(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");
        Vec3f pos;
        pos.x = (float)luaL_checknumber(pLua, 2);
        pos.y = (float)luaL_checknumber(pLua, 3);
        pos.z = (float)luaL_checknumber(pLua, 4);
        pNode->SetLocalPosition(pos);
        return 0;
    }

    // ***********************************************************************

    int Node_GetLocalRotation(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");

        Vec3f rot = pNode->GetLocalRotation();
        lua_pushnumber(pLua, rot.x);
        lua_pushnumber(pLua, rot.y);
        lua_pushnumber(pLua, rot.z);
        return 3;
    }

    // ***********************************************************************

    int Node_GetWorldRotation(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");

        Vec3f rot = pNode->GetWorldRotation();
        lua_pushnumber(pLua, rot.x);
        lua_pushnumber(pLua, rot.y);
        lua_pushnumber(pLua, rot.z);
        return 3;
    }
    
    // ***********************************************************************

    int Node_SetLocalRotation(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");
        Vec3f rot;
        rot.x = (float)luaL_checknumber(pLua, 2);
        rot.y = (float)luaL_checknumber(pLua, 3);
        rot.z = (float)luaL_checknumber(pLua, 4);
        pNode->SetLocalRotation(rot);
        return 0;
    }

    // ***********************************************************************

    int Node_GetLocalScale(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");

        Vec3f sca = pNode->GetLocalScale();
        lua_pushnumber(pLua, sca.x);
        lua_pushnumber(pLua, sca.y);
        lua_pushnumber(pLua, sca.z);
        return 3;
    }

    // ***********************************************************************

    int Node_GetWorldScale(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");

        Vec3f sca = pNode->GetWorldScale();
        lua_pushnumber(pLua, sca.x);
        lua_pushnumber(pLua, sca.y);
        lua_pushnumber(pLua, sca.z);
        return 3;
    }

    // ***********************************************************************

    int Node_SetLocalScale(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");
        Vec3f sca;
        sca.x = (float)luaL_checknumber(pLua, 2);
        sca.y = (float)luaL_checknumber(pLua, 3);
        sca.z = (float)luaL_checknumber(pLua, 4);
        pNode->SetLocalScale(sca);
        return 0;
    }

    // ***********************************************************************

    int Node_GetNumChildren(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");
        lua_pushinteger(pLua, pNode->GetNumChildren());
        return 1;
    }

    // ***********************************************************************

    int Node_GetChild(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");
        int index = (int)luaL_checkinteger(pLua, 2);
        
        Node** ppNode = (Node**)lua_newuserdata(pLua, sizeof(Node*));
        *ppNode = pNode->GetChild(index - 1);
        (*ppNode)->Retain(); // The node data already existed, so this is now a new reference to it
        CheckAndInitPropertyTable(pLua, *ppNode);

        luaL_getmetatable(pLua, "Node");
        lua_setmetatable(pLua, -2);
        return 1;
    }

    // ***********************************************************************

    int Node_GetPropertyTable(lua_State* pLua)
    {
        Node* pNode = *(Node**)luaL_checkudata(pLua, 1, "Node");
        lua_getfield(pLua, LUA_REGISTRYINDEX, "_nodePropTables");

        // node key is an integer (either it's ptr, or eventually a bit merge of scene and node key)
        lua_pushinteger(pLua, pNode->m_id);
        lua_gettable(pLua, -2);

        // We check if that element exists, if not create a new table like _nodePropTables[nodeKey] = t
        if (lua_type(pLua, -1) != LUA_TTABLE)
        {
            lua_pop(pLua, 1);
            lua_newtable(pLua);
            lua_pushinteger(pLua, pNode->m_id);
            lua_pushvalue(pLua, -2);
            lua_settable(pLua, -4); // Sets _nodePropTables[nodeId] = newTable (and leaves new table on the stack)
        }
        return 1;
    }

    // ***********************************************************************

    int Scene_GetNumNodes(lua_State* pLua)
    {
        Scene* pScene = *(Scene**)luaL_checkudata(pLua, 1, "Scene");
        lua_pushinteger(pLua, pScene->GetNumNodes());
        return 1;
    }

    // ***********************************************************************

    int Scene_GetNode(lua_State* pLua)
    {
        Scene* pScene = *(Scene**)luaL_checkudata(pLua, 1, "Scene");
        int index = (int)luaL_checkinteger(pLua, 2);
        
        Node** ppNode = (Node**)lua_newuserdata(pLua, sizeof(Node*));
        *ppNode = pScene->GetNode(index - 1);
        (*ppNode)->Retain(); // The primitive data already existed, so this is now a new reference to it
        CheckAndInitPropertyTable(pLua, *ppNode);

        luaL_getmetatable(pLua, "Node");
        lua_setmetatable(pLua, -2);
        
        return 1;
    }

    // ***********************************************************************

    int LoadScene(lua_State* pLua)
    {
        size_t len;
        const char *str = luaL_checklstring(pLua, 1, &len);
        std::string path(str, len);

        Scene* pScene = Scene::LoadScene(path.c_str());

        // Create a new userdata for our object
        Scene** ppScene = (Scene**)lua_newuserdata(pLua, sizeof(Scene*));
        *ppScene = pScene;

        // Sets the metatable of this new userdata to the type's table
        luaL_getmetatable(pLua, "Scene");
        lua_setmetatable(pLua, -2);

        return 1;
    }

    // ***********************************************************************

    int BindScene(lua_State* pLua)
    {
        lua_newtable(pLua); // _nodePropTables
        lua_setfield(pLua, LUA_REGISTRYINDEX, "_nodePropTables");

         // Bind static scene functions
        const luaL_Reg sceneGlobalFuncs[] = {
            { "LoadScene", LoadScene },
            { NULL, NULL }
        };

        lua_pushglobaltable(pLua);
        luaL_setfuncs(pLua, sceneGlobalFuncs, 0);

        const luaL_Reg sceneMethods[] = {
            { "GetNumNodes", Scene_GetNumNodes },
            { "GetNode", Scene_GetNode },
            { NULL, NULL }
        };
        luax_registertype(pLua, "Scene", sceneMethods);

        const luaL_Reg nodeMethods[] = {
            { "GetNumChildren", Node_GetNumChildren },
            { "GetChild", Node_GetChild },
            { "GetPropertyTable", Node_GetPropertyTable },
            { "GetLocalPosition", Node_GetLocalPosition },
            { "GetWorldPosition", Node_GetWorldPosition },
            { "SetLocalPosition", Node_SetLocalPosition },
            { "GetLocalRotation", Node_GetLocalRotation },
            { "GetWorldRotation", Node_GetWorldRotation },
            { "SetLocalRotation", Node_SetLocalRotation },
            { "GetLocalScale", Node_GetLocalScale },
            { "GetWorldScale", Node_GetWorldScale },
            { "SetLocalScale", Node_SetLocalScale },
            { NULL, NULL }
        };
        luax_registertype(pLua, "Node", nodeMethods);

        return 0;
    }
}