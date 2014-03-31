
require('box')
require('readobj')

box = make_box(2)

tex = color.texture('../data/ak47.png')
mat = material.new()
mat:set_diffuse(tex)
mat:set_refraction(2.5, false)

s = 0.0003

gun = scene.mesh('cow', readobj('../data/ak47.obj'))
gun:set_material(mat)
gun:rotate('x', 45)
gun:translate({0,-1.5,0})
gun:scale({s,s,s})
box:add_child(gun)


do_render(box, 8, 's', 'gun_render.png', 1000, 1000)
