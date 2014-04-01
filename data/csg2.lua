
require('box')

box = make_box(2)

white = color.constant({1,1,1})

mat1 = material.new()
mat1:set_diffuse(color.constant({1,0.2,0.2}))
mat1:set_specular(white,25)
mat2 = material.new()
mat2:set_diffuse(color.constant({0.2,1.0,0.2}))
mat2:set_specular(white,25)
mat3 = material.new()
mat3:set_diffuse(color.constant({0.2,0.2,1.0}))
mat3:set_specular(white,25)
mat4 = material.new()
mat4:set_diffuse(color.constant({0.8,0.2,1.0}))
mat4:set_specular(white,25)
mat5 = material.new()
mat5:set_diffuse(color.constant({0.8,0.8,0.2}))
mat5:set_specular(white,25)

c1 = scene.cylinder('c1', 0.45, 2.5)
c1:set_material(mat1)
c2 = scene.cylinder('c2', 0.45, 2.5)
c2:set_material(mat2)
c2:rotate('x',90)
c3 = scene.cylinder('c3', 0.45, 2.5)
c3:set_material(mat3)
c3:rotate('y',90)

cross = scene.csg('', 'u', c1, c2)
cross = scene.csg('', 'u', cross, c3)

s1 = scene.sphere('s1', 1)
s1:set_material(mat4)

b1 = scene.box('b1', 1.6)
b1:set_material(mat5)
b1:translate({-0.8,-0.8,-0.8})

csg = scene.csg('', 'i', s1, b1)
csg = scene.csg('', 'd', csg, cross)
csg:rotate('x', 20)
csg:rotate('y', 45)
box:add_child(csg)

do_render(box, 8, 's', 'csg2.png', 1000, 1000)
