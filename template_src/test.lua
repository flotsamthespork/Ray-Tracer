

spec1 = color.constant({0.5, 0.7, 0.5})
spec2 = color.constant({0.5, 0.4, 0.8})

diff1 = color.constant({0.7, 1.0, 0.7})
diff2 = color.constant({0.5, 0.5, 0.5})
diff3 = color.constant({1.0, 0.6, 0.1})
diff4 = color.constant({0.7, 0.6, 1.0})

tex1 = color.texture("bump.png")
tex2 = color.texture("earth.png")

mat1 = material.new()
mat1:set_diffuse(diff1)
mat1:set_bump(tex1)
-- TODO - specular

mat2 = material.new()
mat2:set_diffuse(tex2)

mat3 = material.new()
mat3:set_diffuse(diff3)

mat4 = material.new()
mat4:set_diffuse(diff4)

scene_root = scene.node("root")

s1 = scene.sphere("s1", {0, 0, -400}, 100)
--scene_root:add_child(s1)
s1:set_material(mat1)

s2 = scene.sphere("s2", {0, 0, 0}, 150)
--scene_root:add_child(s2)
s2:translate({200, 50, -100})
s2:rotate('x', 60)
s2:rotate('y', 180)
s2:set_material(mat1)

s3 = scene.sphere("s3", {0, -1200, -500}, 1000)
--scene_root:add_child(s3)
s3:set_material(mat2)

s4 = scene.sphere("s4", {-100, 25, -300}, 50)
--scene_root:add_child(s4)
s4:set_material(mat3)

s5 = scene.sphere("s5", {0, 100, -250}, 25)
--scene_root:add_child(s5)
s5:set_material(mat4)

t1 = scene.torus("t1", 100, 25)
--scene_root:add_child(t1)
t1:translate({0,0, 300})
--t1:rotate('y', 0.05)
t1:set_material(mat2)

csg_s1 = scene.sphere("csg_s1", {-200, 0, 0}, 100)
csg_s1:set_material(mat2)

csg_s2 = scene.torus("csg_s2", 100, 67)
csg_s2:translate({-200, 0, 0})
csg_s2:rotate('x', 90)
csg_s2:rotate('y', 4)
csg_s2:set_material(mat3)

csg = scene.csg("csg1", 'd', csg_s1, csg_s2)
scene_root:add_child(csg)
csg:rotate('y', -45)

white_light = scene.light("white_light");
scene_root:add_child(white_light)
white_light:light_position({-100, 150, 400})
white_light:light_color({0.9, 0.9, 0.9})
white_light:light_falloff({1, 0, 0})

orange_light = scene.light("orange_light");
scene_root:add_child(orange_light)
orange_light:light_position({400, 100, 150})
orange_light:light_color({0.7, 0.0, 0.7})
orange_light:light_falloff({1, 0, 0})

cam1 = scene.camera("cam1", 1)
scene_root:add_child(cam1)
cam1:cam_position({0, 0, 800}, false)
cam1:cam_up({0, 1, 0}, false)
cam1:cam_view({0, 0, -1}, false)
cam1:cam_fov(50)

for i=0,100 do
	s2:rotate('y', 1)
	s1:rotate('y', 1)

	-- 'b' for BruteForce
	rt = tracer.new(scene_root, 'b')
	rt:set_threads(1)
	rt:set_ambient({0.3, 0.3, 0.3})
	
	-- Camera ID, Output, width, height
	rt:render(1, "test"..i..".png", 500, 500)

	t1:rotate('y', 3)
	csg:rotate('y', 5)
end
