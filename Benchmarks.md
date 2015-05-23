# Benchmarking Results #

Here we compare lua-vec against other implementations: standard lua, garbage collected userdata vectors, LuaJIT (Lua just in time compiler, see http://luajit.org/) and C++ implementation. We have chosen smallpt (http://kevinbeason.com/smallpt/), an open source global illumination renderer as the benchmark. Smallpt uses vectors heavily so we expect the execution time to be bottlenecked by vector operations.

We have ported smallpt to lua (see test/smallpt.lua in the source distribution) which supports three different vector implementations. In order to keep computation times low, we have chosen to run the benchmark in 256x192 resolution with 8\*4 samples per pixel.

Here are the results (smaller rendering times are better):

| **Implementation** | **Time** | **Notes** |
|:-------------------|:---------|:----------|
| std lua            | 1217s    | Standard lua 5.1.4 with vectors implemented in lua (see below for source code) |
| gcvec              | 770s     | Standard lua & C vector module (garbage collected userdata) |
| lua-vec            | 274s     | Lua with vectors (our implementation) |
| luajit             | 234s     | LuaJIT 2.0.0-beta4 |
| luajit+lua-vec     | ???      |           |
| cpp                | 10s      | Full C++ implementation of smallpt |

As can be seen lua-vec is almost 4.5 times faster than standard lua. Lua-vec also clearly beats a userdata implementation, and is even quite near to LuaJIT's performance. Full C++ implementation is still an order of magnitude faster. LuaJIT's performance (which obviously uses vectors implemented in Lua) is probably totally bottlenecked by garbage collection.

### Lua vector implementation ###

```
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
```