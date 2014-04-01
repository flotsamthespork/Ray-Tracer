
require('box')
require('readobj')

box = make_box(2, true)

c = color.constant({0.84, 0.6, 0.53})
mat = material.new()
mat:set_diffuse(c)

torus = scene.torus('t', 0.5, 0.2)
torus:set_material(mat)
torus:rotate('x', 60)
torus:rotate('y', 12)

t1 = scene.node('')
t1:add_child(torus)
box:add_child(t1)
t1:translate({1,-1,0})

t1 = scene.node('')
t1:add_child(torus)
box:add_child(t1)
t1:translate({-1,0,0})

for i=1,5 do
	do_render(box, 8, 's', 'shadows1_'..i..'.png', 500, 500, i)
end
