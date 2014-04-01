
box = scene.node('')

white = color.constant({1,1,1})

green = material.new()
green:set_diffuse(color.constant({0.2,1.0,0.2}))
blue = material.new()
blue:set_diffuse(color.constant({0.2,0.2,1.0}))

torus = scene.torus('', 1, 0.5)
box:add_child(torus)
torus:set_material(green)
torus:translate({0.5, 0, 2})

torus = scene.sphere('', 1.5, 0.5)
box:add_child(torus)
torus:set_material(blue)
torus:translate({-0.75, 0.75, 1.5})
torus:scale({1,1,0.95})

torus = scene.torus('', 1, 0.5)
box:add_child(torus)
torus:set_material(green)
torus:translate({-0.75, -0.75, 0.75})


light = scene.light('light1')
box:add_child(light)
light:light_position({0,0.95*2,0.01*2})
light:light_color({1.0,1.0,1.0})
light:light_falloff({1,0,0})

cam = scene.camera('camera', 1)
box:add_child(cam)
cam:cam_position({-0.5,-0.1,-2*2.3}, true)
cam:cam_up({0,1,0}, true)
cam:cam_view({0,0,2*0.5}, true)
cam:cam_fov(45)

rt = tracer.new(box, 's')
rt:set_threads(8)
rt:set_ambient({0.3, 0.3, 0.3})

rt:render(1, 'aa.png', 500, 500, 1)
