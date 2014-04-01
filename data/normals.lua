
require('box')
require('readobj')

box = make_box(2, false)

mat2 = material.new()
mat2:set_diffuse(color.constant({0.5,0.7,0.5}))

cow = scene.mesh('cow', readobj('obj/horse.obj'))
box:add_child(cow)
cow:set_material(mat2)
s = 0.01
cow:scale({s,s,s})
cow:translate({-13,-170,-156})
cow:rotate('y', 90)
cow:rotate('z', 56)

do_render(box, 8, 's', 'normals2.png', 500, 500, 1)
