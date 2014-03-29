
require('box')

box = make_box(1)

black = color.constant({0.3, 0.3, 0.99})
spec = color.constant({0.6, 0.6, 0.6})
mat = material.new()
mat:set_diffuse(black)
mat:set_specular(spec, 25)
mat:set_refraction(1.5, false)

torus = scene.torus('torus', 0.5,0.1)
box:add_child(torus)
torus:set_material(mat)
torus:rotate('y', 45)
torus:rotate('x', 45)

torus = scene.torus('torus', 0.5,0.1)
box:add_child(torus)
torus:set_material(mat)
torus:translate({0.0,0.0,-0.45})
torus:rotate('y', -45)
torus:rotate('x', 45)

for i=0,100 do
	do_render(box, 8, 'b', 'img'..i..'.png', 700, 700)
	torus:rotate('x',3)
end

