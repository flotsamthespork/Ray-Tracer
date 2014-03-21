

x = scene.node("haha")
y = scene.node("haha")
x:translate({0,1,2})
x:scale({0,1,2})
x:rotate('x', 12)
x:add_child(y)

l = scene.light("l1")
l:light_position({0,0,0})
l:light_color({1,0,0})

s = scene.sphere("", {1,0,0}, 3)


-- 'b' for BruteForce
rt = tracer.new(x, 'b')
rt:set_threads(1)
rt:set_ambient({0.1, 0.1, 0.1})

-- Camera ID, Output, width, height
rt:render(1, "test.png", 256, 256)
