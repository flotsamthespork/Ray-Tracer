
require('box')

box = make_box(1, false)

black = color.constant({0.1, 0.1, 0.1})
spec = color.constant({0.6, 0.6, 0.6})
mirror_mat = material.new()
mirror_mat:set_diffuse(black)
mirror_mat:set_reflection(0.9)
mirror_mat:set_specular(spec, 25)

b = scene.box('b', 1)
b:set_material(mirror_mat)
--b:translate({0,0,-10})
b:scale({10,10,0.01})
b:translate({-0.5,-0.5, 0})

b1 = scene.node('')
b1:add_child(b)
box:add_child(b1)
b1:translate({-1,0,-4})
b1:rotate('y', 45)

b2 = scene.node('')
b2:add_child(b)
box:add_child(b2)
b2:translate({1,0,-4})
b2:rotate('y', -45)

b2 = scene.node('')
b2:add_child(b)
box:add_child(b2)
b2:translate({0,-1,-4})
b2:rotate('x', -45)

b2 = scene.node('')
b2:add_child(b)
box:add_child(b2)
b2:translate({0,1,-4})
b2:rotate('x', 45)

b2 = scene.node('')
b2:add_child(b)
box:add_child(b2)
b2:translate({0,0,0.9})
b2:rotate('y', 2)

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

do_render(box, 8, 's', 'reflection2.png', 1000, 1000, 1)

