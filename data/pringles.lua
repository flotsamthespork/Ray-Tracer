
require('box')
require('readobj')

box = make_box(2)

tex = color.texture('../data/pringles.png')
mat = material.new()
mat:set_diffuse(tex)

s = 0.3

gun = scene.mesh('cow', readobj('../data/pringles.obj'))
gun:set_material(mat)
gun:scale({s,s,s})
gun:translate({0,-0.5,0})
gun:rotate('x', 45)
box:add_child(gun)


do_render(box, 8, 's', 'pringles_render.png', 1000, 1000)
