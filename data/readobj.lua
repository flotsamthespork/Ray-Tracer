-- Reads an Alias/Wavefront OBJ mesh
-- This is very simplistic and lacking many features in the OBJ spec,
-- and pretty much just handles vertices and faces.
--
-- If you want to handle vertex normals, texture coordinates, groups, etc.
-- you'll have to enhance this or write your own routine.
function readobj(filename)
   local file, error = io.open(filename, "r")
   
   if not file then
      print("Error opening " .. filename .. ": " .. error)
      return nil
   end

   local verts = {}
   local uvs = {}
   local faces = {}
   local norms = {}

   for line in file:lines() do
      _, _, command, rest = string.find(line, "^(%w*)%s*(.*)")

      if command == 'v' then
	 _, _, a, b, c = string.find(rest, "(%-?%d+%.?%d*e?-?%d*)%s*(%-?%d+%.?%d*e?-?%d*)%s*(%-?%d+%.?%d*e?-?%d*)")
	 table.insert(verts, {tonumber(a), tonumber(b), tonumber(c)})
      elseif command == 'vt' then
	 _, _, a, b = string.find(rest, "(%-?%d+%.?%d*e?-?%d*)%s*(%-?%d+%.?%d*e?-?%d*)")
--	 _, _, a, b = string.find(rest, "(%-?%d+%.?%d*)%s*(%-?%d+%.?%d*)")
	 table.insert(uvs, {tonumber(a), tonumber(b)})
      elseif command == 'vn' then
	 _, _, a, b, c = string.find(rest, "(%-?%d+%.?%d*e?-?%d*)%s*(%-?%d+%.?%d*e?-?%d*)%s*(%-?%d+%.?%d*e?-?%d*)")
	 table.insert(norms, {tonumber(a), tonumber(b), tonumber(c)})
      elseif command == 'f' then
      	 local face = {}
	 local face_v = {}
	 local face_vt = {}
	 local face_norms = {}
	 for list in string.gfind(rest, "([0-9/]+)%s*") do
	    -- to use texture coordinates or normals you will need to fetch _all_ indices here
	    _, _, value = string.find(list, "(%d+)/?")
	    _, _, vt = string.find(list, "/(%d+)/?")
	    _, _, vn = string.find(list, "/[0-9]+/(%d+)/?")

	    if (vt ~= nil) then
	    	table.insert(face_vt, tonumber(vt) - 1)
	    end

	    if (vn ~= nil) then
	    	table.insert(face_norms, tonumber(vn) - 1)
	    end

	    -- OBJ starts indexing at one, so need to subtract one to
	    -- get C-style indexing
	    table.insert(face_v, tonumber(value) - 1)
	 end
	 table.insert(face, face_v)
	 table.insert(face, face_vt)
	 table.insert(face, face_norms)
	 table.insert(faces, face)
      end
   end

   file:close()
   return verts, uvs, norms, faces
end

