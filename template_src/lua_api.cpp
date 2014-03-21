//
// CS488 - Introduction to Computer Graphics
//
// scene_lua.cpp
//
// Everything that's needed to parse a scene file using Lua.
// You don't necessarily have to understand exactly everything that
// goes on here, although it will be useful to have a reasonable idea
// if you wish to add new commands to the scene format.
//
// Lua interfaces with C/C++ using a special stack. Everytime you want
// to get something from lua, or pass something back to lua (e.g. a
// return value), you need to use this stack. Thus, most of the lua_
// and luaL_ functions actually manipulate the stack. All the
// functions beginning with "lua_" are part of the Lua C API itself,
// whereas the "luaL_" functions belong to a library of useful
// functions on top of that called lauxlib.
//
// This file consists of a bunch of C function declarations which
// implement functions callable from Lua. There are also two tables
// used to set up the interface between Lua and these functions, and
// the main "driver" function, import_lua, which calls the lua
// interpreter and sets up all the state.
//
// Note that each of the function declarations follow the same format:
// they take as their only argument the current state of the lua
// interpreter, and return the number of values returned back to lua.
//
// For more information see the book "Programming In Lua," available
// online at http://www.lua.org/pil/, and of course the Lua reference
// manual at http://www.lua.org/manual/5.0/.
//
// http://lua-users.org/wiki/LauxLibDocumentation provides a useful
// documentation of the "lauxlib" functions (beginning with luaL_).
//
// -- University of Waterloo Computer Graphics Lab 2005

#include "lua_api.hpp"
#include <iostream>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <vector>
#include "lua488.hpp"
#include "scene_tree.hpp"

// Uncomment the following line to enable debugging messages
#define GRLUA_ENABLE_DEBUG

#ifdef GRLUA_ENABLE_DEBUG
#	define GRLUA_DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#	define GRLUA_DEBUG_CALL do { std::cerr << __FUNCTION__ << std::endl; } while (0)
#else
#	define GRLUA_DEBUG(x) do { } while (0)
#	define GRLUA_DEBUG_CALL do { } while (0)
#endif

// You may wonder, for the following types, why we use special "_ud"
// types instead of, for example, just allocating SceneNodes directly
// from lua. Part of the answer is that Lua is a C api. It doesn't
// call any constructors or destructors for you, so it's easier if we
// let it just allocate a pointer for the node, and handle
// allocation/deallocation of the node itself. Another (perhaps more
// important) reason is that we will want SceneNodes to stick around
// even after lua is done with them, after all, we want to pass them
// back to the program. If we let Lua allocate SceneNodes directly,
// we'd lose them all when we are done parsing the script. This way,
// we can easily keep around the data, all we lose is the extra
// pointers to it.

// The "userdata" type for a node. Objects of this type will be
// allocated by Lua to represent nodes.
struct scene_node_ud {
	SceneNode* node;
};


#define SCENE_LIB	"scene"
#define SCENE_META	"scene.scene"

// Useful function to retrieve and check an n-tuple of numbers.
template<typename T>
void get_tuple(lua_State* L, int arg, T* data, int n)
{
	luaL_checktype(L, arg, LUA_TTABLE);
	luaL_argcheck(L, luaL_getn(L, arg) == n, arg, "N-tuple expected");
	for (int i = 1; i <= n; i++) {
	  lua_rawgeti(L, arg, i);
	  data[i - 1] = luaL_checknumber(L, -1);
	  lua_pop(L, 1);
	}
}

void get_color(lua_State *L, int arg, Colour &color)
{
	double c[3];
	get_tuple(L, arg, c, 3);
	color = Colour(c[0], c[1], c[2]);
}

static void make_lib(lua_State *L,
		const char *lib_name,
		const char *lib_meta,
		const luaL_reg *lib_functions,
		const luaL_reg *lib_methods)
{
	luaL_newmetatable(L, lib_meta);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_openlib(L, 0, lib_methods, 0);
	luaL_openlib(L, lib_name, lib_functions, 0);
}


/////////////////////////////////////////////
/////////////// Scene Library ///////////////
/////////////////////////////////////////////


/////////////// Scene Library Functions ///////////////

extern "C"
int scene_f_node_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *data = (scene_node_ud*)lua_newuserdata(L, sizeof(scene_node_ud));
	data->node = 0;

	const char *name = luaL_checkstring(L, 1);
	data->node = new SceneNode(name);

	luaL_getmetatable(L, SCENE_META);
	lua_setmetatable(L, -2);

	return 1;
}

extern "C"
int scene_f_joint_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *data = (scene_node_ud*)lua_newuserdata(L, sizeof(scene_node_ud));
	data->node = 0;

	const char *name = luaL_checkstring(L, 1);
	data->node = new JointNode(name);

	luaL_getmetatable(L, SCENE_META);
	lua_setmetatable(L, -2);

	return 1;
}

extern "C"
int scene_f_light_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *data = (scene_node_ud*)lua_newuserdata(L, sizeof(scene_node_ud));
	data->node = 0;

	const char *name = luaL_checkstring(L, 1);
	data->node = new LightNode(name);

	luaL_getmetatable(L, SCENE_META);
	lua_setmetatable(L, -2);

	return 1;
}

extern "C"
int scene_f_camera_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *data = (scene_node_ud*)lua_newuserdata(L, sizeof(scene_node_ud));
	data->node = 0;

	const char *name = luaL_checkstring(L, 1);
	double cam_id = luaL_checknumber(L, 2);
	data->node = new CameraNode(name, cam_id);

	luaL_getmetatable(L, SCENE_META);
	lua_setmetatable(L, -2);

	return 1;
}


/////////////// Scene Library Methods ///////////////

extern "C"
int scene_m_gc_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *me = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, me != 0, 1, "Node expected");

	me->node = 0;
	return 0;
}

extern "C"
int scene_m_scale_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *me = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, me != 0, 1, "Node expected");

	SceneNode *node = me->node;
	Vector3D scale;
	get_tuple(L, 2, &scale[0], 3);

	node->scale(scale);
	return 0;
}

extern "C"
int scene_m_translate_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *me = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, me != 0, 1, "Node expected");

	SceneNode *node = me->node;
	Vector3D trans;
	get_tuple(L, 2, &trans[0], 3);

	node->translate(trans);
	return 0;
}

extern "C"
int scene_m_rotate_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *me = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, me != 0, 1, "Node expected");

	SceneNode *node = me->node;
	Vector3D trans;
	const char *axis_string = luaL_checkstring(L, 2);
	luaL_argcheck(L, axis_string && std::strlen(axis_string) == 1,
			2, "Single character expected");
	char axis = std::tolower(axis_string[0]);
	luaL_argcheck(L, axis >= 'x' && axis <= 'z', 2, "Axis must be x, y or z");
	double angle = luaL_checknumber(L, 3);

	node->rotate(axis, angle);
	return 0;
}

extern "C"
int scene_m_add_child_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *me = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, me != 0, 1, "Node expected");

	scene_node_ud *child = (scene_node_ud*)luaL_checkudata(L, 2, SCENE_META);
	luaL_argcheck(L, child != 0, 2, "Node expected");

	SceneNode *n = me->node;
	SceneNode *c = child->node;

	n->add_child(c);
	return 0;
}


static LightNode *get_light_node(lua_State *L)
{
	scene_node_ud *me = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, me != 0, 1, "Light expected");

	SceneNode *n = me->node;
	luaL_argcheck(L, n->get_type() == LIGHT, 1, "Light expected");

	return (LightNode*) n;
}

extern "C"
int scene_m_light_color_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	LightNode *light = get_light_node(L);
	get_color(L, 2, light->color);
	return 0;
}

extern "C"
int scene_m_light_pos_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	LightNode *light = get_light_node(L);
	get_tuple(L, 2, &light->position[0], 3);
	return 0;
}

extern "C"
int scene_m_light_falloff_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	LightNode *light = get_light_node(L);
	get_tuple(L, 2, light->falloff, 3);
	return 0;
}

static CameraNode *get_cam_node(lua_State *L)
{
	scene_node_ud *me = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, me != 0, 1, "Camera expected");

	SceneNode *n = me->node;
	luaL_argcheck(L, n->get_type() == CAMERA, 1, "Camera expected");

	return (CameraNode*) n;
}

extern "C"
int scene_m_cam_pos_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	CameraNode *cam = get_cam_node(L);
	get_tuple(L, 2, &cam->position[0], 3);
	cam->rel_pos = lua_toboolean(L, 3);
	return 0;
}

extern "C"
int scene_m_cam_up_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	CameraNode *cam = get_cam_node(L);
	get_tuple(L, 2, &cam->up[0], 3);
	cam->rel_up = lua_toboolean(L, 3);
	return 0;
}

extern "C"
int scene_m_cam_view_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	CameraNode *cam = get_cam_node(L);
	get_tuple(L, 2, &cam->view[0], 3);
	cam->rel_view = lua_toboolean(L, 3);
	return 0;
}

extern "C"
int scene_m_cam_fov_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	CameraNode *cam = get_cam_node(L);
	cam->fov = luaL_checknumber(L, 2);
	return 0;
}


/////////////// Make Library ///////////////


static const luaL_reg scenelib_functions[] = {
	{"node",	scene_f_node_cmd},
	{"joint",	scene_f_joint_cmd},
	{"light",	scene_f_light_cmd},
	{"camera",	scene_f_camera_cmd},
	// TODO - primitives
	{0, 0}
};

static const luaL_reg scenelib_methods[] = {
	{"__gc",	scene_m_gc_cmd},
	// Scene Node
	{"translate",		scene_m_translate_cmd},
	{"scale",		scene_m_scale_cmd},
	{"rotate",		scene_m_rotate_cmd},
	{"add_child",		scene_m_add_child_cmd},
	// Light Node
	{"light_color",		scene_m_light_color_cmd},
	{"light_position",	scene_m_light_pos_cmd},
	{"light_falloff",	scene_m_light_falloff_cmd},
	// Camera Node
	{"cam_position",	scene_m_cam_pos_cmd},
	{"cam_up",		scene_m_cam_up_cmd},
	{"cam_view",		scene_m_cam_view_cmd},
	{"cam_fov",		scene_m_cam_fov_cmd},
	{0, 0}
};


static void make_scene_lib(lua_State *L)
{
	make_lib(L, SCENE_LIB, SCENE_META,
			scenelib_functions,
			scenelib_methods);
}


//////////////////////////////////////////////
/////////////// Run Lua Script ///////////////
//////////////////////////////////////////////

bool run_lua(const std::string& filename)
{
	GRLUA_DEBUG("Importing scene from " << filename);
	
	// Start a lua interpreter
	lua_State* L = lua_open();

	GRLUA_DEBUG("Loading base libraries");
	
	// Load some base library
	luaL_openlibs(L);


	GRLUA_DEBUG("Setting up our functions");

	make_scene_lib(L);

	GRLUA_DEBUG("Parsing the scene");
	// Now parse the actual scene
	if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
	  std::cerr << "Error loading " << filename << ": " << lua_tostring(L, -1) << std::endl;
	  return false;
	}
	GRLUA_DEBUG("Closing the interpreter");
	
	// Close the interpreter, free up any resources not needed
	lua_close(L);

	return true;
}
