
box = scene.node('')

white = color.constant({1,1,1})

mat1 = material.new();
mat1:set_diffuse(color.constant({1,0.2,0.2}))
mat1:set_specular(white, 25)
mat2 = material.new();
mat2:set_diffuse(color.constant({0.2,0.2,1}))
mat2:set_specular(white, 25)

s1 = scene.sphere('s1', 0.3)
s1:set_material(mat1)
s1:translate({0.1,0,0})

s2 = scene.sphere('s2', 0.3)
s2:set_material(mat2)
s2:translate({-0.1,0,0})

u = scene.csg('union', 'u', s1, s2)
i = scene.csg('intersection', 'i', s1, s2)
d1 = scene.csg('diff1', 'd', s1, s2)
d2 = scene.csg('diff2', 'd', s2, s1)

function add(csg, y, z)
	local rx = 0
	local ry = -25
	local rz = 0
	for x=-1.5,1.5,0.75 do
		local node = scene.node('')
		node:add_child(csg)
		box:add_child(node)
		node:translate({-x, y, z})
		node:rotate('x', rx)
		node:rotate('y', ry)
		node:rotate('z', rz)

		rx = rx + -45
		ry = ry + 90
	end
end

add(u,  1.2, 1)
add(i,  0.3, 0.5)
add(d1, -0.5, 0)
add(d2, -1.5, -0.5)

--do_render(box, 8, 's', 'csg.png', 500, 500)
light = scene.light('light1')
box:add_child(light)
light:light_position({0,0.95*2,0.01*2})
light:light_color({1.0,1.0,1.0})
light:light_falloff({1,0,0})

cam = scene.camera('camera', 1)
box:add_child(cam)
cam:cam_position({0,0,-2*3.0}, true)
cam:cam_up({0,1,0}, true)
cam:cam_view({0,0,2*0.5}, true)
cam:cam_fov(45)

rt = tracer.new(box, 's')
rt:set_threads(8)
rt:set_ambient({0.3, 0.3, 0.3})

rt:render(1, 'csg.png', 1000, 1000, 1)
