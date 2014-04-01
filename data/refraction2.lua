
require('box')
require('readobj')

box = make_box(2, false)

mat2 = material.new()
mat2:set_refraction(1.33, false)
mat2:set_specular(color.constant({1,1,1}),25)

cow = scene.mesh('cow', readobj('textures/cow.obj'))
box:add_child(cow)
cow:set_material(mat2)
cow:translate({0,-2,0})
cow:rotate('y', 220)
cow:scale({1.5,1.5,1.5})

do_render(box, 8, 's', 'refraction3.png', 500, 500, 1)
