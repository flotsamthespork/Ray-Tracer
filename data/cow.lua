
require('box')
require('readobj')

box = make_box(2)

c = color.constant({0.84, 0.6, 0.53})
mat = material.new()
mat:set_diffuse(c)
mat:set_refraction(1.2, false)

s = 0.35

cow = scene.mesh('cow', readobj('../data/cow.obj'))
cow:set_material(mat)
cow:scale({s,s,s})

i1 = scene.node('i1')
i1:add_child(cow)
i1:translate({0,0,1})
box:add_child(i1)

i2 = scene.node('i2')
i2:add_child(cow)
i2:translate({1,0,1})
--box:add_child(i2)

for i=0,200 do
	do_render(box, 8, 's', 'cow'..i..'.png', 1000, 1000)
	i1:rotate('y', 3)
end
