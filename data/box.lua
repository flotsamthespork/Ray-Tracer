

function make_box(size)
	box_cm_white = color.constant({0.76, 0.75, 0.50})
	box_cm_red   = color.constant({0.63, 0.06, 0.04})
	box_cm_green = color.constant({0.15, 0.48, 0.09})

	box_mat_white = material.new()
	box_mat_white:set_diffuse(box_cm_white)
	box_mat_red = material.new()
	box_mat_red:set_diffuse(box_cm_red)
	box_mat_green = material.new()
	box_mat_green:set_diffuse(box_cm_green)

	box = scene.node('box')

	box_ceiling = scene.mesh('ceiling', {
				{-size,  size, -size*4},
				{-size,  size,  size},
				{ size,  size,  size},
				{ size,  size, -size*4},
			}, {
			}, {
				{3,2,1,0}
			})
	box:add_child(box_ceiling)
	box_ceiling:set_material(box_mat_white)

	box_floor = scene.mesh('floor', {
				{-size, -size, -size*4},
				{-size, -size,  size},
				{ size, -size,  size},
				{ size, -size, -size*4},
			}, {
			}, {
				{0,1,2,3}
			})
	box:add_child(box_floor)
	box_floor:set_material(box_mat_white)

	box_wleft = scene.mesh('left', {
				{ size, -size*1.01, -size*4},
				{ size, -size*1.01,  size},
				{ size,  size*1.01,  size},
				{ size,  size*1.01, -size*4},
			}, {
			}, {
				{0,1,2,3}
			})
	box:add_child(box_wleft)
	box_wleft:set_material(box_mat_red)

	box_wright = scene.mesh('left', {
				{-size, -size*1.01, -size*4},
				{-size, -size*1.01,  size},
				{-size,  size*1.01,  size},
				{-size,  size*1.01, -size*4},
			}, {
			}, {
				{3,2,1,0}
			})
	box:add_child(box_wright)
	box_wright:set_material(box_mat_green)

	box_wback = scene.mesh('back', {
				{-size, -size*1.01,  size},
				{ size, -size*1.01,  size},
				{ size,  size*1.01,  size},
				{-size,  size*1.01,  size},
			}, {
			}, {
				{3,2,1,0}
			})
	box:add_child(box_wback)
	box_wback:set_material(box_mat_white)

	box_wbehind = scene.mesh('behind', {
				{-size, -size*1.01, -size*4},
				{ size, -size*1.01, -size*4},
				{ size,  size*1.01, -size*4},
				{-size,  size*1.01, -size*4},
			}, {
			}, {
				{0,1,2,3}
			})
	box:add_child(box_wbehind)
	box_wbehind:set_material(box_mat_white)

	light = scene.light('light1')
	box:add_child(light)
	light:light_position({0,0.95*size,0.01*size})
	light:light_color({1.0,1.0,1.0})
	light:light_falloff({1,0,0})

	cam = scene.camera('camera', 1)
	box:add_child(cam)
	cam:cam_position({0,0,-size*3.0}, true)
	cam:cam_up({0,1,0}, true)
	cam:cam_view({0,0,size*0.5}, true)
	cam:cam_fov(45)

	return box
end


function do_render(box, num_threads, intersection_strat,
		img_name, width, height)
	rt = tracer.new(box, intersection_strat)
	rt:set_threads(num_threads)
	rt:set_ambient({0.3, 0.3, 0.3})

	rt:render(1, img_name, width, height)
end

