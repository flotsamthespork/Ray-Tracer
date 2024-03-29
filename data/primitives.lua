
require('box')

box = make_box(2, false)

tex1 = color.constant({1.0, 0.4, 0.1})
tex2 = color.constant({0.4, 1.0, 0.1})
tex3 = color.constant({0.77, 0.23,0.55})
spec1 = color.constant({1.0,1.0,1.0})
mat1 = material.new()
mat1:set_diffuse(tex1)
mat1:set_specular(spec1, 15)
mat2 = material.new()
mat2:set_diffuse(tex2)
mat2:set_specular(spec1, 15)
mat3 = material.new()
mat3:set_diffuse(tex3)
mat3:set_specular(spec1, 15)

torus = scene.torus('torus', 0.6, 0.2)
box:add_child(torus)
torus:set_material(mat1)
torus:translate({1,0,0})
torus:rotate('x', 45)
torus:rotate('y', 45)

cone = scene.cone('cone', 0.5, 2)
box:add_child(cone)
cone:set_material(mat2)
cone:translate({-0.3, 0.5, 0.9})
cone:rotate('x', 120)
cone:rotate('y', 15)

cylinder = scene.cylinder('cylinder', 0.4, 1.3)
box:add_child(cylinder)
cylinder:set_material(mat3)
cylinder:translate({-1.1,0,-0.3})
cylinder:rotate('x', 30)
cylinder:rotate('y', 60)

do_render(box, 8, 's', 'primitives.png', 500, 500, 1)
