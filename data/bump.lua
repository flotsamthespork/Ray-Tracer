
require('box')

box = make_box(2, false)

bump1 = color.texture('bump/bump2.png')
bump1:bump_magnitude(-5)
--bump1:scale(0.5,0.5)

mat1 = material.new()
mat1:set_diffuse(color.constant({1,0.2,0.2}))
mat1:set_specular(color.constant({1,1,1}),25)
mat1:set_bump(bump1)

mat2 = material.new()
mat2:set_diffuse(color.constant({1,0.2,0.2}))
mat2:set_specular(color.constant({1,1,1}),25)


b1 = scene.box('b', 1)
box:add_child(b1)
b1:set_material(mat2)
b1:translate({1,0,1})
b1:rotate('x', 45)
b1:scale({1.5,3,0.01})
b1:translate({-0.5,-0.5,-0.5})

b2 = scene.box('b', 1)
box:add_child(b2)
b2:set_material(mat2)
b2:translate({-1,0,1})
b2:rotate('x', 15)
b2:scale({1.5,3,0.01})
b2:translate({-0.5,-0.5,-0.5})

do_render(box, 8, 's', 'bump1.png', 500, 500, 1)
b1:set_material(mat1)
b2:set_material(mat1)
do_render(box, 8, 's', 'bump2.png', 500, 500, 1)
