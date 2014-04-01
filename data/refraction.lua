
require('box')

box = make_box(1, false)

black = color.constant({0.3, 0.3, 0.99})
spec = color.constant({0.6, 0.6, 0.6})
mat1 = material.new()
mat1:set_diffuse(black)
mat1:set_specular(spec, 25)
mat1:set_refraction(1.01, false)
mat2 = material.new()
mat2:set_diffuse(black)
mat2:set_specular(spec, 25)
mat2:set_refraction(1.33, false)
mat3 = material.new()
mat3:set_diffuse(black)
mat3:set_specular(spec, 25)
mat3:set_refraction(1.52, false)
mat4 = material.new()
mat4:set_diffuse(black)
mat4:set_specular(spec, 25)
mat4:set_refraction(2.49, false)

tex1 = color.texture('textures/sandstone.png')
mat_tex = material.new()
mat_tex:set_diffuse(tex1)

b = scene.box('b', 1)
box:add_child(b)
b:set_material(mat_tex)
b:translate({0,0,0.9})
b:scale({2,2,0.01})
b:translate({-0.5,-0.5, 0})

sphere = scene.sphere('sphere', 0.4)
box:add_child(sphere)
sphere:set_material(mat1)
sphere:translate({0.4,0.4,-0.4})

sphere = scene.sphere('sphere', 0.4)
box:add_child(sphere)
sphere:set_material(mat2)
sphere:translate({-0.4,0.4,-0.5})

sphere = scene.sphere('sphere', 0.4)
box:add_child(sphere)
sphere:set_material(mat3)
sphere:translate({0.4,-0.4,-0.4})

sphere = scene.sphere('sphere', 0.4)
box:add_child(sphere)
sphere:set_material(mat4)
sphere:translate({-0.4,-0.4,-0.5})

do_render(box, 8, 's', 'refraction.png', 500, 500, 1)

