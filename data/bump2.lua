
require('box')
require('readobj')

box = make_box(2)

tex1 = color.texture('textures/cow.png')
bump1 = color.texture('bump/bump2.png')
bump1:bump_magnitude(-0.1)

mat1 = material.new()
mat1:set_refraction(2.5, false)
mat1:set_specular(color.constant({1,1,1}),25)
mat1:set_bump(bump1)

mat2 = material.new()
mat2:set_refraction(2.5, false)
mat2:set_specular(color.constant({1,1,1}),25)

mat6 = material.new()
mat6:set_diffuse(tex1)


b1 = scene.box('b', 1)
box:add_child(b1)
b1:set_material(mat1)
b1:translate({0.9,0,-1.5})
b1:rotate('x', 30)
b1:scale({1.5,3,0.5})
b1:translate({-0.5,-0.5,-0.5})

b1 = scene.box('b', 1)
box:add_child(b1)
b1:set_material(mat2)
b1:translate({-0.9,0,-1.5})
b1:rotate('x', 30)
b1:scale({1.5,3,0.5})
b1:translate({-0.5,-0.5,-0.5})



cow = scene.mesh('cow', readobj('textures/cow.obj'))
box:add_child(cow)
cow:set_material(mat6)
cow:translate({0,-2,0.5})
cow:rotate('y', 220)
cow:scale({1.3,1.3,1.3})

do_render(box, 8, 's', 'bump3.png', 500, 500)
