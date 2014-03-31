
require('box')

box = make_box(1)

black = color.constant({0.3, 0.3, 0.99})
spec = color.constant({0.6, 0.6, 0.6})
mat = material.new()
mat:set_diffuse(black)
mat:set_specular(spec, 25)
mat:set_refraction(1.5, false)

tex = color.texture("../data/ak47.png")
mat2 = material.new()
mat2:set_diffuse(tex)

plane = scene.mesh("mesh", {
			{-1.5, -1.5,  0},
			{-1.5,  1.5,  0},
			{ 1.5,  1.5,  0},
			{ 1.5, -1.5,  0}
		}, {
			{0, 1},
			{0, 0},
			{1, 0},
			{1, 1}
		}, {
			{{3,2,1,0},{3,2,1,0}}
		})
box:add_child(plane)
plane:set_material(mat2)
plane:translate({0,0,0.99})

s = scene.sphere('s', 0.25)
box:add_child(s)
s:set_material(mat)
s:scale({3,3,0.3})
--s:translate({-0.7,0,0})


do_render(box, 8, 's', 'img.png', 1000, 1000)

