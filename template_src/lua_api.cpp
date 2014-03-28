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
#include "tracer.hpp"
#include "colormap.hpp"
#include "material.hpp"
#include "mesh.hpp"

// Uncomment the following line to enable debugging messages
//#define GRLUA_ENABLE_DEBUG

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

struct tracer_ud {
	RayTracer *tracer;
};

struct color_ud {
	ColorMap *color;
};

struct material_ud {
	Material *mat;
};


#define SCENE_LIB	"scene"
#define SCENE_META	"scene.scene"

#define TRACER_LIB	"tracer"
#define TRACER_META	"tracer.tracer"

#define COLOR_LIB	"color"
#define COLOR_META	"color.color"

#define MATERIAL_LIB	"material"
#define MATERIAL_META	"material.material"

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
	int cam_id = luaL_checkinteger(L, 2);
	data->node = new CameraNode(name, cam_id);

	luaL_getmetatable(L, SCENE_META);
	lua_setmetatable(L, -2);

	return 1;
}

extern "C"
int scene_f_csg_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *data = (scene_node_ud*)lua_newuserdata(L, sizeof(scene_node_ud));
	data->node = 0;

	const char *name = luaL_checkstring(L, 1);

	const char *opstr = luaL_checkstring(L, 2);
	luaL_argcheck(L, opstr, 2, "'u', 'i', 'd' expected");
	luaL_argcheck(L, strlen(opstr) == 1, 2, "'u', 'i', 'd' expected");

	CsgOp op;
	switch (opstr[0])
	{
	case 'u':
		op = UNION;
		break;
	case 'i':
		op = INTERSECTION;
		break;
	case 'd':
		op = DIFFERENCE;
		break;
	default:
		luaL_argcheck(L, false, 2, "'u', 'i', 'd' expected");
	}

	scene_node_ud *l = (scene_node_ud*)luaL_checkudata(L, 3, SCENE_META);
	luaL_argcheck(L, l != 0, 3, "Geometry/Csg expected");
	luaL_argcheck(L, l->node->get_type() == GEOMETRY ||
			l->node->get_type() == CSG, 3, "Geometry/Csg expected");

	scene_node_ud *r = (scene_node_ud*)luaL_checkudata(L, 4, SCENE_META);
	luaL_argcheck(L, r != 0, 4, "Geometry/Csg expected");
	luaL_argcheck(L, l->node->get_type() == GEOMETRY ||
			l->node->get_type() == CSG, 4, "Geometry/Csg expected");

	data->node = new CsgNode(name, op, l->node, r->node);

	luaL_getmetatable(L, SCENE_META);
	lua_setmetatable(L, -2);

	return 1;
}

extern "C"
int scene_f_mesh_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *data = (scene_node_ud*)lua_newuserdata(L, sizeof(scene_node_ud));
	data->node = 0;

	const char* name = luaL_checkstring(L, 1);

	std::vector<Point3D> verts;
	std::vector<Point2D> uvs;
	std::vector<std::vector<int> > faces;

	luaL_checktype(L, 2, LUA_TTABLE);
	int vert_count = luaL_getn(L, 2);

	luaL_argcheck(L, vert_count >= 1, 2, "Tuple of vertices expected");

	for (int i = 1; i <= vert_count; i++)
	{
		lua_rawgeti(L, 2, i);

		Point3D vertex;
		get_tuple(L, -1, &vertex[0], 3);
		verts.push_back(vertex);
		lua_pop(L, 1);
	}

	luaL_checktype(L, 3, LUA_TTABLE);
	int uv_count = luaL_getn(L, 3);

	for (int i = 1; i <= uv_count; ++i)
	{
		lua_rawgeti(L, 3, i);

		Point2D uv;
		get_tuple(L, -1, &uv[0], 2);
		uvs.push_back(uv);
		lua_pop(L, 1);
	}

	luaL_checktype(L, 4, LUA_TTABLE);
	int face_count = luaL_getn(L, 4);

	luaL_argcheck(L, face_count >= 1, 4, "Tuple of faces expected");

	faces.resize(face_count);

	for (int i = 1; i <= face_count; ++i)
	{
		lua_rawgeti(L, 4, i);

		luaL_checktype(L, -1, LUA_TTABLE);
		int index_count = luaL_getn(L, -1);

		luaL_argcheck(L, index_count >= 3, 4, "Tuple of indices expected");
		faces[i-1].resize(index_count);
		get_tuple(L, -1, &faces[i-1][0], index_count);

		lua_pop(L, 1);
	}

	Mesh *mesh = new Mesh(verts, uvs, faces);
	GRLUA_DEBUG(*mesh);
	data->node = new GeometryNode(name, mesh);

	luaL_getmetatable(L, SCENE_META);
	lua_setmetatable(L, -2);

	return 1;
}

extern "C"
int scene_f_sphere_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	int nargs = lua_gettop(L);

	scene_node_ud *data = (scene_node_ud*)lua_newuserdata(L, sizeof(scene_node_ud));
	data->node = 0;

	const char *name = (nargs >= 1 ? luaL_checkstring(L, 1) : "");
	Point3D pos;
	if (nargs >= 2)
		get_tuple(L, 2, &pos[0], 3);

	double radius;
	if (nargs >= 3)
		radius = luaL_checknumber(L, 3);
	else
		radius = 1;

	Primitive *p = new Sphere(pos, radius);
	data->node = new GeometryNode(name, p);

	luaL_getmetatable(L, SCENE_META);
	lua_setmetatable(L, -2);

	return 1;
}

extern "C"
int scene_f_torus_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	int nargs = lua_gettop(L);

	scene_node_ud *data = (scene_node_ud*)lua_newuserdata(L, sizeof(scene_node_ud));
	data->node = 0;

	const char *name = (nargs >= 1 ? luaL_checkstring(L, 1) : "");
	double outer_rad;
	if (nargs >= 2)
		outer_rad = luaL_checknumber(L, 2);
	else
		outer_rad = 1;

	double inner_rad;
	if (nargs >= 3)
		inner_rad = luaL_checknumber(L, 3);
	else
		inner_rad = 1;

	Primitive *p = new Torus(inner_rad, outer_rad);
	data->node = new GeometryNode(name, p);

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

extern "C"
int scene_m_set_material_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	scene_node_ud *me = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, me != 0, 1, "Geometry expected");
	luaL_argcheck(L, me->node->get_type() == GEOMETRY, 1, "Geometry expected");

	GeometryNode *node = (GeometryNode*) me->node;

	material_ud *mat = (material_ud*)luaL_checkudata(L, 2, MATERIAL_META);
	luaL_argcheck(L, mat != 0, 2, "Material expected");

	node->material = mat->mat;
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
	{"csg",		scene_f_csg_cmd},
	{"sphere",	scene_f_sphere_cmd},
	{"torus",	scene_f_torus_cmd},
	{"mesh",	scene_f_mesh_cmd},
	{0, 0}
};

static const luaL_reg scenelib_methods[] = {
	{"__gc",	scene_m_gc_cmd},
	// Scene Node
	{"translate",		scene_m_translate_cmd},
	{"scale",		scene_m_scale_cmd},
	{"rotate",		scene_m_rotate_cmd},
	{"add_child",		scene_m_add_child_cmd},
	{"set_material",	scene_m_set_material_cmd},
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
/////////////// Tracer Library ///////////////
//////////////////////////////////////////////


/////////////// Tracer Library Functions ///////////////

extern "C"
int tracer_f_new_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;


	tracer_ud *data = (tracer_ud*)lua_newuserdata(L, sizeof(tracer_ud));
	data->tracer = 0;

	scene_node_ud *node = (scene_node_ud*)luaL_checkudata(L, 1, SCENE_META);
	luaL_argcheck(L, node != 0, 1, "Node expected");

	const char *isstr = luaL_checkstring(L, 2);

	IntersectionStrategyParams params;

	int len = strlen(isstr);
	if (!isstr || len == 0)
		luaL_argcheck(L, false, 2, "Invalid intersection strategy");
	else if (isstr[0] == 'b')
	{
		if (len != 1)
			luaL_argcheck(L, false, 2, "Brute force strategy has no options");
		params.type = BRUTE_FORCE;
	}
	else
		luaL_argcheck(L, false, 2, "Invalid intersection strategy");

	IntersectionStrategy *is = get_strategy(params);
	if (!is)
		luaL_argcheck(L, false, 2, "Failed to generate strategy");

	Scene *scene = new Scene();
	Scene::make_scene(node->node, scene);

	data->tracer = new RayTracer(scene, is);

	luaL_getmetatable(L, TRACER_META);
	lua_setmetatable(L, -2);

	return 1;
}

/////////////// Tracer Library Methods ///////////////


extern "C"
int tracer_m_gc_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	tracer_ud *me = (tracer_ud*)luaL_checkudata(L, 1, TRACER_META);
	luaL_argcheck(L, me != 0, 1, "Ray-Tracer expected");

	me->tracer = 0;
	return 0;
}

extern "C"
int tracer_m_set_threads_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	tracer_ud *me = (tracer_ud*)luaL_checkudata(L, 1, TRACER_META);
	luaL_argcheck(L, me != 0, 1, "Ray-Tracer expected");

	int num_threads = luaL_checkinteger(L, 2);
	me->tracer->set_num_threads(num_threads);

	return 0;
}

extern "C"
int tracer_m_set_ambient_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	tracer_ud *me = (tracer_ud*)luaL_checkudata(L, 1, TRACER_META);
	luaL_argcheck(L, me != 0, 1, "Ray-Tracer expected");

	Colour c(0);
	get_color(L, 2, c);
	me->tracer->set_ambient(c);

	return 0;
}

extern "C"
int tracer_m_render_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	tracer_ud *me = (tracer_ud*)luaL_checkudata(L, 1, TRACER_META);
	luaL_argcheck(L, me != 0, 1, "Ray-Tracer expected");

	int cam_id = luaL_checkinteger(L, 2);
	const char *img_name = luaL_checkstring(L, 3);
	int width = luaL_checkinteger(L, 4);
	int height = luaL_checkinteger(L, 5);

	me->tracer->render(cam_id, img_name, width, height);
	return 0;
}


/////////////// Make Library ///////////////


static const luaL_reg tracerlib_functions[] = {
	{"new",		tracer_f_new_cmd},
	{0, 0}
};

static const luaL_reg tracerlib_methods[] = {
	{"__gc",		tracer_m_gc_cmd},
	{"set_threads",		tracer_m_set_threads_cmd},
	{"set_ambient",		tracer_m_set_ambient_cmd},
	{"render",		tracer_m_render_cmd},
	{0, 0}
};


static void make_tracer_lib(lua_State *L)
{
	make_lib(L, TRACER_LIB, TRACER_META,
			tracerlib_functions,
			tracerlib_methods);
}

//////////////////////////////////////////////
/////////////// Colour Library ///////////////
//////////////////////////////////////////////


/////////////// Color Library Functions ///////////////

extern "C"
int color_f_constant_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	color_ud *data = (color_ud*)lua_newuserdata(L, sizeof(color_ud));
	data->color = 0;

	Colour c(0);
	get_color(L, 1, c);

	data->color = new ConstantColorMap(c);

	luaL_getmetatable(L, COLOR_META);
	lua_setmetatable(L, -2);
	return 1;
}

extern "C"
int color_f_texture_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	color_ud *data = (color_ud*)lua_newuserdata(L, sizeof(color_ud));
	data->color = 0;

	std::string name = luaL_checkstring(L, 1);

	data->color = new TextureColorMap(name);

	luaL_getmetatable(L, COLOR_META);
	lua_setmetatable(L, -2);
	return 1;
}

/////////////// Color Library Methods ///////////////


extern "C"
int color_m_gc_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	color_ud *me = (color_ud*)luaL_checkudata(L, 1, COLOR_META);
	luaL_argcheck(L, me != 0, 1, "Color expected");

	me->color = 0;
	return 0;
}

static TextureColorMap*
get_texture_map(lua_State *L)
{
	color_ud *me = (color_ud*)luaL_checkudata(L, 1, COLOR_META);
	luaL_argcheck(L, me != 0, 1, "Texture expected");
	luaL_argcheck(L, me->color->get_type() == TEXTURE, 1, "Texture expected");

	return (TextureColorMap*)me->color;
}

extern "C"
int color_m_translate_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	double x, y;
	TextureColorMap *c = get_texture_map(L);
	x = luaL_checknumber(L,2);
	y = luaL_checknumber(L,3);

	c->translate(x, y);
	return 0;
}

extern "C"
int color_m_scale_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	double x, y;
	TextureColorMap *c = get_texture_map(L);
	x = luaL_checknumber(L,2);
	y = luaL_checknumber(L,3);

	c->scale(x,y);
	return 0;
}

extern "C"
int color_m_rotate_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	TextureColorMap *c = get_texture_map(L);
	double angle = luaL_checknumber(L,2);

	c->rotate(angle);
	return 0;
}


/////////////// Make Library ///////////////


static const luaL_reg colorlib_functions[] = {
	{"constant",	color_f_constant_cmd},
	{"texture",	color_f_texture_cmd},
	{0, 0}
};

static const luaL_reg colorlib_methods[] = {
	{"__gc",		color_m_gc_cmd},
	{"translate",		color_m_translate_cmd},
	{"scale",		color_m_scale_cmd},
	{"rotate",		color_m_rotate_cmd},
	// TODO - texture transform commands
	{0, 0}
};


static void make_color_lib(lua_State *L)
{
	make_lib(L, COLOR_LIB, COLOR_META,
			colorlib_functions,
			colorlib_methods);
}


////////////////////////////////////////////////
/////////////// Material Library ///////////////
////////////////////////////////////////////////


/////////////// Material Library Functions ///////////////

extern "C"
int material_f_new_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	material_ud *data = (material_ud*)lua_newuserdata(L, sizeof(material_ud));
	data->mat = 0;

	data->mat = new Material();

	luaL_getmetatable(L, MATERIAL_META);
	lua_setmetatable(L, -2);
	return 1;
}

/////////////// Material Library Methods ///////////////


extern "C"
int material_m_gc_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	material_ud *me = (material_ud*)luaL_checkudata(L, 1, MATERIAL_META);
	luaL_argcheck(L, me != 0, 1, "Material expected");

	me->mat = 0;
	return 0;
}

extern "C"
int material_m_set_diffuse_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	material_ud *me = (material_ud*)luaL_checkudata(L, 1, MATERIAL_META);
	luaL_argcheck(L, me != 0, 1, "Material expected");

	color_ud *color = (color_ud*)luaL_checkudata(L, 2, COLOR_META);
	luaL_argcheck(L, color != 0, 2, "Color expected");

	me->mat->set_diffuse(color->color);
	return 0;
}

extern "C"
int material_m_set_specular_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	material_ud *me = (material_ud*)luaL_checkudata(L, 1, MATERIAL_META);
	luaL_argcheck(L, me != 0, 1, "Material expected");

	color_ud *color = (color_ud*)luaL_checkudata(L, 2, COLOR_META);
	luaL_argcheck(L, color != 0, 2, "Color expected");

	double shiny = luaL_checknumber(L, 3);

	me->mat->set_specular(color->color);
	me->mat->set_shininess(shiny);
	return 0;
}

extern "C"
int material_m_set_bump_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	material_ud *me = (material_ud*)luaL_checkudata(L, 1, MATERIAL_META);
	luaL_argcheck(L, me != 0, 1, "Material expected");

	color_ud *color = (color_ud*)luaL_checkudata(L, 2, COLOR_META);
	luaL_argcheck(L, color != 0, 2, "Color expected");

	me->mat->set_bump(color->color);
	return 0;
}

int material_m_set_refraction_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	material_ud *me = (material_ud*)luaL_checkudata(L, 1, MATERIAL_META);
	luaL_argcheck(L, me != 0, 1, "Material expected");

	double refraction_index = luaL_checknumber(L, 2);
	bool is_dielectric = lua_toboolean(L, 3);

	me->mat->set_refraction_index(refraction_index, is_dielectric);
	return 0;
}

int material_m_set_reflection_cmd(lua_State *L)
{
	GRLUA_DEBUG_CALL;

	material_ud *me = (material_ud*)luaL_checkudata(L, 1, MATERIAL_META);
	luaL_argcheck(L, me != 0, 1, "Material expected");

	double reflection = luaL_checknumber(L, 2);

	me->mat->set_mirror_coefficient(reflection);
	return 0;
}



/////////////// Make Library ///////////////


static const luaL_reg materiallib_functions[] = {
	{"new",		material_f_new_cmd},
	{0, 0}
};

static const luaL_reg materiallib_methods[] = {
	{"__gc",		material_m_gc_cmd},
	{"set_diffuse",		material_m_set_diffuse_cmd},
	{"set_specular",	material_m_set_specular_cmd},
	{"set_bump",		material_m_set_bump_cmd},
	{"set_refraction",	material_m_set_refraction_cmd},
	{"set_reflection",	material_m_set_reflection_cmd},
	{0, 0}
};


static void make_material_lib(lua_State *L)
{
	make_lib(L, MATERIAL_LIB, MATERIAL_META,
			materiallib_functions,
			materiallib_methods);
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
	make_tracer_lib(L);
	make_color_lib(L);
	make_material_lib(L);

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
