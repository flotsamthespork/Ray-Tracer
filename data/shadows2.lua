
require('box')
require('readobj')

box = make_box(2, true)

c = color.constant({0.84, 0.6, 0.53})
mat = material.new()
mat:set_diffuse(c)

s = 0.25

cow = scene.mesh('cow', readobj('../data/cow.obj'))
cow:set_material(mat)
cow:scale({s,s,s})

i1 = scene.node('i1')
i1:add_child(cow)
i1:translate({0,-1,0})
i1:rotate('x', 90)
box:add_child(i1)

for i=1,5 do
	do_render(box, 8, 's', 'shadows2_'..i..'.png', 500, 500, i)
end
