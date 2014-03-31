
package.path = package.path ..";../data/?.lua"

require('box')

box = make_box(2)

red = color.constant({1.0,0.0,0.0})
tex = color.texture("cow.png")
white = color.constant({1,1,1})
mat = material.new()
mat:set_diffuse(tex)
mat:set_specular(white, 25)
--mat:set_refraction(1.4,false)

cyl = scene.cone('cyl', 0.5, 0.4)
--box:add_child(cyl)
cyl:set_material(mat)
--cyl:rotate('x', -45)
cyl:rotate('y', 0)
cyl:rotate('x', 40)

b = scene.box('b', 1)
box:add_child(b)
b:set_material(mat)
b:rotate('x', -30)
b:rotate('z', 45)

for i=0,4 do
	do_render(box, 8, 's', 'img'..i..'.png', 500, 500)
	b:rotate('y', 90)
end
