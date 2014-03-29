
require('box')

box = make_box(2)

red = color.constant({1.0, 0.2, 0.15})
blue = color.constant({0.1, 0.09, 0.9})
white = color.constant({1.0,1.0,1.0})

mat1 = material.new()
mat1:set_diffuse(red)

mat2 = material.new()
mat2:set_diffuse(blue)

s1 = scene.sphere('s1', 0.3)
s1:set_material(mat1)
s1:scale({3,1,1})

s2 = scene.sphere('s2', 0.3)
s2:set_material(mat1)
s2:scale({1,1,3})

s3 = scene.sphere('s3', 0.3)
s3:set_material(mat1)
s3:scale({1,3,1})

csg = scene.csg('c', 'u', s1, s2)
csg = scene.csg('c', 'u', csg, s3)

s4 = scene.sphere('s4', 0.5)
s4:set_material(mat2)

csg = scene.csg('c', 'd', s4, csg)
box:add_child(csg)
csg:rotate('x', 45)
csg:rotate('y', 45)

do_render(box, 1, 'b', 'img.png', 500, 500)
