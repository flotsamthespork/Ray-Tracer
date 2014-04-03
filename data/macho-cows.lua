
require('readobj')

stone = material.new()
stone:set_diffuse(color.constant({0.8, 0.7, 0.7}))
grass = material.new()
grass:set_diffuse(color.constant({0.1, 0.7, 0.1}))
hide = material.new()
hide:set_diffuse(color.constant({0.84, 0.6, 0.53}))
hide:set_specular(color.constant({0.3, 0.3, 0.3}), 20)


inst = scene.node('inst')

arc = scene.node('arc')
inst:add_child(arc)
arc:translate({0, 0, -10})


p1 = scene.box('p1', 1)
arc:add_child(p1)
p1:set_material(stone)
p1:translate({-2.4, 0, -0.4})
p1:scale({0.8, 4, 0.8})

p2 = scene.box('p2', 1)
arc:add_child(p2)
p2:set_material(stone)
p2:translate({1.6, 0, -0.4})
p2:scale({0.8, 4, 0.8})

s = scene.sphere('s', 1)
arc:add_child(s)
s:set_material(stone)
s:translate({0, 4, 0})
s:scale({4, 0.6, 0.6})

cow_poly = scene.mesh('cow', readobj('cow.obj'))
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)

cow_poly:translate({0, -1, 0})
cow_poly:scale({factor,factor,factor})
cow_poly:translate({0, 3.637, 0})


scen = scene.node('scene')
scen:rotate('x', 23)

plane = scene.mesh('plane', {
			{-1,0,-1},
			{1,0,-1},
			{1,0,1},
			{-1,0,1}
		}, {}, {}, {
			{{3,2,1,0}}
		})
scen:add_child(plane)
plane:set_material(grass)
plane:scale({30,30,30})

require('buckyball')

scen:add_child(buckyball)
buckyball:set_material(stone)
buckyball:scale({1.5,1.5,1.5})

for _, pt in pairs({
		{{{1,1.3,14}},20},
		{{{5,1.3,-11}},180},
		{{{-5.5,1.3,-3}},-60}}) do
	cow_instance = scene.node('')
	scen:add_child(cow_instance)
	cow_instance:add_child(cow_poly)
	cow_instance:translate(unpack(pt[1]))
	cow_instance:rotate('y', pt[2])
	cow_instance:scale({1.4,1.4,1.4})
end

for i=1,6 do
	an_arc = scene.node('')
	an_arc:rotate('y', (i-1) * 60)
	scen:add_child(an_arc)
	an_arc:add_child(arc)
end

white_light = scene.light("white_light");
scen:add_child(white_light)
white_light:light_position({200,202,430})
white_light:light_color({0.8, 0.8, 0.8})
white_light:light_falloff({1, 0, 0})
--white_light:light_area({3,0,0}, {0,0,3})


cam1 = scene.camera("cam1", 1)
scen:add_child(cam1)
cam1:cam_up({0, 1, 0}, false)
cam1:cam_fov(50)
cam1:cam_position({0, 2,30}, false)
cam1:cam_view({0, 0, -1}, false)

rt = tracer.new(scen, 's')
rt:set_threads(8)
rt:set_ambient({0.4, 0.4, 0.4})
rt:set_shadow_samples(1)

-- Camera ID, Output, width, height
rt:render(1, "macho-cows.png", 1500, 1500)

