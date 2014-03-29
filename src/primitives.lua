
require('box')

box = make_box(2)

tex1 = color.constant({1.0, 0.4, 0.1})
spec1 = color.constant({1.0,1.0,1.0})
mat = material.new()
mat:set_diffuse(tex1)
mat:set_specular(spec1, 15)

torus = scene.torus('torus', 0.7, 0.3)
box:add_child(torus)
torus:set_material(mat)
torus:translate({1,0,0})
torus:rotate('x', 45)
torus:rotate('y', 45)

--box:scale({0.9,0.9,0.9})

do_render(box, 8, 'b', 'img.png', 500, 500)
