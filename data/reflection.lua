
require('box')

box = make_box(1, false)

black = color.constant({0.1, 0.1, 0.1})
spec = color.constant({0.6, 0.6, 0.6})
mirror_mat = material.new()
mirror_mat:set_diffuse(black)
mirror_mat:set_reflection(0.8)
mirror_mat:set_specular(spec, 25)

sphere1 = scene.sphere('sphere1', 0.35)
box:add_child(sphere1)
sphere1:set_material(mirror_mat)
sphere1:translate({0.5,-0.5,0.5})

sphere2 = scene.sphere('sphere2', 0.35)
box:add_child(sphere2)
sphere2:set_material(mirror_mat)
sphere2:translate({-0.5,-0.5,0.0})

sphere3 = scene.sphere('sphere2', 0.5)
box:add_child(sphere3)
sphere3:set_material(mirror_mat)
sphere3:translate({0,0.5,1.0})

do_render(box, 8, 's', 'reflection.png', 500, 500, 1)

