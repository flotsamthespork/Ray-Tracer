BOARD_SIZE = 4
PIECE_WIDTH = BOARD_SIZE*0.022
PIECE_LENGTH = PIECE_WIDTH*(52/47)
PIECE_HEIGHT = PIECE_WIDTH*0.5
HOLDER_WIDTH = BOARD_SIZE*0.5
HOLDER_HEIGHT = BOARD_SIZE*0.035

require('readobj')

math.randomseed(12345)

root = scene.node('root')

-- TABLE NODE
node_table = scene.node('node:table')
root:add_child(node_table)

-- BOARD NODE
node_board = scene.node('node:board')
node_table:add_child(node_board)
node_board:rotate('y', -9)
node_board:translate({0,0.01,-0.3})

white = color.constant({1,1,1})
black = color.constant({0,0,0})

board_tex = color.texture('scrabble/board.png')
board_bump = color.texture('scrabble/board_bump.png')
board_bump:bump_magnitude(-100)
board_mat = material.new()
board_mat:set_diffuse(board_tex)
board_mat:set_bump(board_bump)

table_tex = color.texture('scrabble/table.png')
table_mat = material.new()
table_mat:set_diffuse(table_tex)

pieces_tex = color.texture('scrabble/pieces.png')
pieces_mat = material.new()
pieces_mat:set_diffuse(pieces_tex)

mirror_mat = material.new()
mirror_mat:set_diffuse(color.constant({0,0,0}))
mirror_mat:set_reflection(0.8)

mirrorframe_mat = material.new()
mirrorframe_mat:set_diffuse(color.constant({0.4,0.4,0.4}))

glass_mat = material.new()
glass_mat:set_diffuse(color.constant({0.5,0.5,0.5}))
glass_mat:set_refraction(1.52, true)

water_mat = material.new()
water_mat:set_diffuse(color.constant({0.4,0.4,1.0}))
--water_mat:set_refraction(1.1, true)

air_mat = material.new()
air_mat:set_diffuse(white)
air_mat:set_refraction(1.0, true)

holder_tex = color.texture('scrabble/holder.png')
holder_tex:scale(3,3)
holder_mat = material.new()
holder_mat:set_diffuse(holder_tex)

function make_mirror(w, h)
	local framesize = 0.05

	local mirror = scene.box('mirror', 1)
	mirror:set_material(mirror_mat)
	mirror:translate({framesize*0.3, framesize, 0})
	mirror:scale({framesize/2, w, h})
	mirror:translate({-0.5, 0, -0.5})


	local mirrorframe = scene.box('mirrorframe', 1)
	mirrorframe:set_material(mirrorframe_mat)
	mirrorframe:translate({-framesize,0,0})
	mirrorframe:scale({0.1+framesize, 2*framesize+w,
			2*framesize+h})
	mirrorframe:translate({-0.5, 0, -0.5})


	return scene.csg('mirror', 'u', mirror, mirrorframe)
end

function make_cup(rad, height)
--[[	local rim_size = 0.1
	local cup = scene.cylinder('cup', rad, height)
	cup:set_material(glass_mat)
	cup:translate({0,0,-height/2})

	local rim = scene.torus('cup', rad-rim_size/2, rim_size/2)
	rim:set_material(glass_mat)
	rim:translate({0, 0,-height})

	cup = scene.csg('cup', 'u', rim, cup)

	local air_height = height*0.85
	local air = scene.cylinder('cup', rad-rim_size, air_height)
	air:set_material(air_mat)
	air:translate({0, 0, air_height*0.5-height-0.01})

	local water_height = height*0.6
	local water = scene.cylinder('water', rad-rim_size-0.01, water_height)
	water:set_material(water_mat)
	water:translate({0, 0, -water_height*0.5-(height-air_height)})

	cup = scene.csg('cup', 'd', cup, air)
--	cup = scene.csg('cup', 'u', cup, water)
--]]

	local scale = 0.35
	cup = scene.mesh('cup', readobj('scrabble/cup.obj'))
	cup:set_material(glass_mat)
	cup:scale({scale,scale,scale})

	return cup
end


function make_tile_holder()
	local box = scene.box('holder', 1)
	box:set_material(holder_mat)

	local cyl_1 = scene.cylinder('holder', 0.8, 2.1)
	cyl_1:set_material(holder_mat)
	cyl_1:translate({1,1,0})

	local h1 = scene.csg('holder', 'd', box, cyl_1)

	local cyl_t = scene.cylinder('holder', 0.1, 1)
	cyl_t:set_material(holder_mat)
	cyl_t:translate({0.1,1,0.5})

	local cyl_b = scene.cylinder('holder', 0.15, 1)
	cyl_b:set_material(holder_mat)
	cyl_b:translate({1,0.15,0.5})

	local holder = scene.node('holder')
	holder:add_child(h1)
	holder:add_child(cyl_t)
	holder:add_child(cyl_b)

	holder:scale({HOLDER_HEIGHT,HOLDER_HEIGHT,HOLDER_WIDTH})
	holder:translate({-0.5,0,-0.5})

	return holder
end

tile_holder = make_tile_holder()

function make_piece(piece)
	local c = string.byte(piece,1) - string.byte('a',1)
	if piece == ' ' then
		c = 26
	end
	local pw = (1/8)
	local ph = (1/4)
	local x = c % 8;
	local y = (c - x)/8
	x = x*pw
	y = (3-y)*ph

	local w = PIECE_WIDTH
	local l = PIECE_LENGTH
	local h = PIECE_HEIGHT

	local pieces_mesh = scene.mesh(piece, {
			{-w, h, -l},
			{-w, h,  l},
			{ w, h,  l},
			{ w, h, -l},
			{-w, 0, -l},
			{-w, 0,  l},
			{ w, 0,  l},
			{ w, 0, -l}
		}, {
			{x+pw, y},
			{x+pw, y+ph},
			{x, y+ph},
			{x, y},
			{pw*4, ph*0},
			{pw*4, ph*1},
			{pw*3, ph*1},
			{pw*3, ph*0},
		}, {}, {
			{{0,1,2,3}, {0,1,2,3}},
			{{7,6,5,4}, {4,5,6,7}},
			{{0,3,7,4}, {4,5,6,7}},
			{{2,1,5,6}, {4,5,6,7}},
			{{3,2,6,7}, {4,5,6,7}},
			{{1,0,4,5}, {4,5,6,7}},
		})
	pieces_mesh:set_material(pieces_mat)
	return pieces_mesh
end


piece = {}
for i=string.byte('a',1),string.byte('z',1) do
	piece[string.char(i)] = make_piece(string.char(i))
end
piece[' '] = make_piece(' ')

function make_tile_rack(word, scale)
	local instance = scene.node('holder_'..word)
	local holder = scene.node('')
	holder:add_child(tile_holder)
	holder:scale({1,1,scale})
	instance:add_child(holder)

	local len = word:len()
	local w = PIECE_WIDTH*1.1*(len-1)*2

	local x = -w/2

	for c in word:gmatch('.') do
		local p = piece[c]
		if p ~= nil then
			local pn = scene.node(word..'_'..c)
			pn:add_child(p)
			instance:add_child(pn)
			pn:translate({-0.1*HOLDER_HEIGHT,
					0.8*HOLDER_HEIGHT,
					-x+0.07*(0.5-math.random())})
			pn:rotate('y', -90)
			pn:rotate('x', -55)
		end
		x = x + PIECE_WIDTH*1.1*2
	end

	return instance
end


function add_word(word, x, y, horizontal)
	local sx = 1
	local sy = 0
	if not horizontal then
		sx = 0
		sy = 1
	end

	for c in word:gmatch('.') do
		local p = piece[c]
		if p ~= nil then
			local pn = scene.node(word..'_'..c)
			pn:add_child(p)
			node_board:add_child(pn)
			pn:translate({(0.5-0.237)*BOARD_SIZE,0,
					(0.5-0.078)*BOARD_SIZE})
			pn:translate({-x*2.23*PIECE_WIDTH,0,
					-y*2.415*PIECE_LENGTH})
		
			pn:translate({0.2*PIECE_WIDTH*(0.5-math.random()), 0,
					0.2*PIECE_LENGTH*(0.5-math.random())})
			pn:rotate('y', (0.5-math.random())*10)
		end
		x = x + sx
		y = y + sy
	end
end


add_word('csg', 0, 4, false)
add_word('textures', 3, 5, false)
add_word('cone', 5, 11, false)
add_word('antialiasing', 9, 1, false)
add_word('bumpm_pping', 4, 1, true)
add_word('_orus', 9, 3, true)
add_word('_of_', 0, 5, true)
add_word('sh_dows', 7, 5, true)
add_word('spat_al', 5, 7, true)
add_word('s_bdivi_ion', 2, 9, true)
add_word('_yli_der', 5, 11, true)

reflection = make_tile_rack('reflection',1)
node_table:add_child(reflection)
reflection:translate({-0.1*BOARD_SIZE,0,0.65*BOARD_SIZE})
reflection:rotate('y', -140)

refraction = make_tile_rack('refraction',1)
node_table:add_child(refraction)
refraction:translate({0.1*BOARD_SIZE,0,-0.65*BOARD_SIZE})
refraction:rotate('y', 75)

patrick = make_tile_rack('by__patrick__dobson',2)
node_table:add_child(patrick)
patrick:translate({-0.6*BOARD_SIZE, 0, -0.15*BOARD_SIZE})

mirror = make_mirror(2,3)
root:add_child(mirror)
mirror:rotate('y', 30)
mirror:translate({-0.8*BOARD_SIZE,0,0.25*BOARD_SIZE})

cup = make_cup(0.5,2)
root:add_child(cup)
cup:translate({1.9*BOARD_SIZE,0,-1.75*BOARD_SIZE})
cup:rotate('x', -90)


table = scene.box('table', 1)
table:set_material(table_mat)
table:scale({8, 1, 8})
table:translate({-0.5,-1,-0.5})
node_table:add_child(table)


board = scene.mesh("mesh", {
			{-(BOARD_SIZE/2), 0, -(BOARD_SIZE/2)},
			{-(BOARD_SIZE/2), 0,  (BOARD_SIZE/2)},
			{ (BOARD_SIZE/2), 0,  (BOARD_SIZE/2)},
			{ (BOARD_SIZE/2), 0, -(BOARD_SIZE/2)},
		}, {
			{1, 0},
			{1, 1},
			{0, 1},
			{0, 0}
		}, {}, {
			{{0,1,2,3}, {0,1,2,3}}
		})
node_board:add_child(board)
board:set_material(board_mat)

white_light = scene.light("white_light");
root:add_child(white_light)
white_light:light_position({0,3,3})
white_light:light_color({0.9, 0.9, 0.9})
white_light:light_falloff({1, 0, 0})
white_light:light_area({1,0,0}, {0,0,1})

--[[
white_light = scene.light("white_light");
scene_root:add_child(white_light)
white_light:light_position({-100, 150, 400})
white_light:light_color({0.9, 0.9, 0.9})
white_light:light_falloff({1, 0, 0})

orange_light = scene.light("orange_light");
scene_root:add_child(orange_light)
orange_light:light_position({400, 100, 150})
orange_light:light_color({0.7, 0.0, 0.7})
orange_light:light_falloff({1, 0, 0})
--]]
cam1 = scene.camera("cam1", 1)
root:add_child(cam1)
cam1:cam_up({0, 1, 0}, false)
cam1:cam_fov(50)
--cam1:cam_position({2.2, 3.3, -4.4}, false)
--cam1:cam_view({-0.5, -0.8, 1}, false)
cam1:cam_position({5.2, 3.3, -3.4}, false)
cam1:cam_view({-1, -0.8, 0.5}, false)

--rt = tracer.new(root, 'b')
rt = tracer.new(root, 's')
--rt = tracer.new(root, 'od5s10')
rt:set_threads(8)
rt:set_ambient({0.3, 0.3, 0.3})
rt:set_shadow_samples(1)
rt:set_background(color.texture('gun_render.png'))

-- Camera ID, Output, width, height
rt:render(1, "final_scene0.png", 1000, 1000)

