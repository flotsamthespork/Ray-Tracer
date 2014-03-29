
require('box')

box = make_box(1)

black = color.constant({0.4, 0.4, 0.4})
spec = color.constant({0.6, 0.6, 0.6})
mirror_mat = material.new()
mirror_mat:set_diffuse(black)
mirror_mat:set_reflection(0.65)
mirror_mat:set_specular(spec, 25)

sphere1 = scene.sphere('sphere1', 0.35)
box:add_child(sphere1)
sphere1:set_material(mirror_mat)
sphere1:translate({0.5,-0.5,0.5})

sphere2 = scene.sphere('sphere2', 0.35)
box:add_child(sphere2)
sphere2:set_material(mirror_mat)
sphere2:translate({-0.5,-0.5,0.0})

do_render(box, 8, 'b', 'img.png', 500, 500)

