--[[

128x96, 10*4 samples:
lua 		1353s
lua jit	    142s
cpp 		8s

128x96, 4*4 samples:
lua			135.1s
lua-vec		48.3s    INCORRECT RESULT!

--]]

--[[
output_width = 1024/4
output_height = 768/4
num_samples = 10
--]]

output_width = 1024/8
output_height = 768/8
num_samples = 4

-- lua vec impl

luavec = {}

function luavec.new(x, y, z, w)
	local v = { x or 0, y or 0, z or 0, w or 0 }
	setmetatable(v, mt)
	return v
end

function luavec.normalize(v)
	local s = 1.0 / math.sqrt(v[1]*v[1] + v[2]*v[2] + v[3]*v[3] + v[4]*v[4]);
	return luavec.new(v[1]*s, v[2]*s, v[3]*s, v[4]*s)
end

function luavec.dot(v1, v2)
	return v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3]
end

function luavec.cross(v1, v2)
	return luavec.new(v1[2] * v2[3] - v1[3] * v2[2], v1[3] * v2[1] - v1[1] * v2[3], v1[1] * v2[2] - v1[2] * v2[1])
end

-- operator overloading
mt = {}
mt.__add = function(v1, v2) return luavec.new(v1[1] + v2[1], v1[2] + v2[2], v1[3] + v2[3], v1[4] + v2[4]) end
mt.__sub = function(v1, v2) return luavec.new(v1[1] - v2[1], v1[2] - v2[2], v1[3] - v2[3], v1[4] - v2[4]) end

mt.__mul = function(v1, v2)
	local s = tonumber(v2)
	if s then
		-- vector * scalar
		return luavec.new(v1[1] * s, v1[2] * s, v1[3] * s, v1[4] * s)
	else
		-- vector * vector
		return luavec.new(v1[1] * v2[1], v1[2] * v2[2], v1[3] * v2[3], v1[4] * v2[4])
	end
end

mt.__unm = function(v) return luavec.new(-v[1], -v[2], -v[3], -v[4]) end

-- select vec lib to use
--local veclib = luavec
--local veclib = vec
local veclib = gcvec
newvec = veclib.new
dot = veclib.dot
cross = veclib.cross
normalize = veclib.normalize

-- material types
DIFF = 1
SPEC = 2
REFR = 3

function ray(o_, d_)
	return { o = o_, d = d_ }
end

function sphere(rad_, p_, e_, c_, refl_)
	return { rad = rad_, p = p_, e = e_, c = c_, refl = refl_ }
end

function intersect(ray, sphere)
	-- Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	local op = sphere.p - ray.o
	--local eps = 1e-4
	local eps = 5e-2
	local b = dot(op, ray.d)
	local det = b * b - dot(op, op) + sphere.rad * sphere.rad
	
	if det < 0 then return nil end

	det = math.sqrt(det)
	
	local t = b - det
	if t > eps then return t end
	
	t = b + det
	if t > eps then return t end
	
	return nil
end

-- scene: radius, position, emission, color, material
spheres = 
{
	sphere(1e5,  newvec( 1e5+1, 40.8, 81.6),  newvec(),			newvec(.75, .25, .25),    DIFF), -- Left
	sphere(1e5,  newvec(-1e5+99, 40.8, 81.6), newvec(),			newvec(.25, .25, .75),    DIFF), -- Rght
	sphere(1e5,  newvec(50, 40.8, 1e5),       newvec(),			newvec(.75, .75, .75),    DIFF), -- Back
	sphere(1e5,  newvec(50, 40.8, -1e5+170),  newvec(),			newvec(),                 DIFF), -- Frnt
	sphere(1e5,  newvec(50,  1e5, 81.6),      newvec(),			newvec(.75, .75, .75),    DIFF), -- Botm
	sphere(1e5,  newvec(50, -1e5+81.6, 81.6), newvec(),			newvec(.75, .75, .75),    DIFF), -- Top
	sphere(16.5, newvec(27, 16.5, 47),        newvec(),			newvec(.999, .999, .999), SPEC), -- Mirr
--	sphere(16.5, newvec(73, 16.5, 78),        newvec(), 		newvec(.999, .999, .999), REFR), -- Glas
	sphere(600,  newvec(50, 681.6-0.27, 81.6), newvec(12,12,12), newvec(),                 DIFF)  -- Lite
}

function clamp(x)
	if x < 0 then
		return 0
	elseif x > 1 then
		return 1
	else
		return x
	end
end

function to_int(x)
	return math.pow(clamp(x), 1.0/2.2) * 255 + 0.5
end

function intersect_scene(r, t, id)
	local n = #spheres
	local inf = 1e20
	local t = inf
	
	for i,s in ipairs(spheres) do
		local d = intersect(r, s)
		if d and d < t then
			t = d
			id = i
		end
	end
	
	return t < inf, t, id
end

function radiance(r, depth)
	local hit, t, id
	
	hit, t, id = intersect_scene(r, t, id)
	if hit == false then return newvec(0, 0, 0) end
	
	local obj = spheres[id]
	
	local x = r.o + r.d * t
	local n = normalize(x - obj.p)
	local nl = n
	local f = obj.c
	
	if dot(n, r.d) > 0 then nl = -nl end
	
	local p = math.max(f[1], math.max(f[2], f[3])) -- max refl
	
	-- russian roulette
	depth = depth + 1
	if depth > 5 then
		if math.random() < p then
			f = f * (1.0 / p)
		else
			return obj.e
		end
	end
	
	if obj.refl == DIFF then
		-- ideal diffuse reflection
		local r1 = 2 * math.pi * math.random()
		local r2 = math.random()
		local r2s = math.sqrt(r2)
		local w = nl
		
		local u
		if math.abs(w[1]) > 0.1 then
			u = newvec(0, 1, 0)
		else
			u = newvec(1, 0, 0)
		end
		u = normalize(cross(u, w))
		
		local v = cross(w, u)
		local d = normalize(u * (math.cos(r1) * r2s) + v * (math.sin(r1) * r2s) + w * math.sqrt(1 - r2))
		return obj.e + f * radiance(ray(x, d), depth)
	elseif obj.refl == SPEC then
		-- ideal specular reflection
		local d = r.d - n * (2.0 * dot(n, r.d))
		return obj.e + f * radiance(ray(x, d), depth)
	end
	
	-- refraction not implemented!!
	return newvec()
end

function smallpt()
	local w = output_width
	local h = output_height
	local samps = num_samples 
	
	local cam = ray(newvec(50, 52, 295.6), normalize(newvec(0, -0.042612, -1))); -- cam pos, dir
	local cx = newvec(w * 0.5135 / h)
	local cy = normalize(cross(cx, cam.d)) * 0.5135
	local c = { }
		
	-- prealloc array
	for i=1,w*h do c[i]= true end
	
	print("Tracing...")
	
	local start_time = os.clock()
	
	for y = 1,h do
		print(math.floor(y/h*100).."%")
		
		for x = 1,w do
					
			local i = x + ((h-1)-(y-1)) * w;
			c[i] = newvec()
			
			for sy = 1,2 do
				for sx = 1,2 do
		
					local r = newvec()
		
					for s = 1,samps do
						local r1 = 2 * math.random()
						local r2 = 2 * math.random()
						
						local dx, dy
						if r1 < 1 then dx = math.sqrt(r1) - 1 else dx = 1 - math.sqrt(2-r1) end
						if r2 < 1 then dy = math.sqrt(r2) - 1 else dy = 1 - math.sqrt(2-r2) end
						
						local wx = (((sx + 0.5 + dx) / 2 + x) / w - 0.5)
						local wy = (((sy + 0.5 + dy) / 2 + y) / h - 0.5)
						local d = cx * wx + cy * wy + cam.d
						d = normalize(d)
						local rad = radiance(ray(cam.o + d*140, d), 0)
						r = r + rad * (1.0 / samps)
					end
					
					c[i] = c[i] + newvec(clamp(r[1]), clamp(r[2]), clamp(r[3])) * 0.25
				end
			end
		end
	end
	
	local end_time = os.clock()
	
	print("Tracing took "..(end_time - start_time).." seconds.")
	
	print("Done! Saving result...")
	
	-- save image
	local f = io.open("smallpt_lua.ppm", "wb")
	f:write(string.format("P3\n%d %d\n%d\n", w, h, 255))
	for i=1,w*h do
		f:write(string.format("%d %d %d ", to_int(c[i][1]), to_int(c[i][2]), to_int(c[i][3])))
	end
	f:close()
end

smallpt()
