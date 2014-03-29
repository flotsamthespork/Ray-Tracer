
package.path = package.path ..";../data/?.lua"

require('box')

box = make_box(2)

red = color.constant({0.3,0.3,0.3})
tex = color.texture("img.png")
white = color.constant({1,1,1})
mat = material.new()
mat:set_diffuse(tex)
mat:set_specular(white, 25)
--mat:set_reflection(0.8)

cyl = scene.cylinder('cyl', 0.5, 2)
box:add_child(cyl)
cyl:set_material(mat)
cyl:rotate('y', 0)
cyl:rotate('x', 70)

do_render(box, 8, 'b', 'img.png', 500, 500)
