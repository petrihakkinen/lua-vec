# Introduction #

Once created, vectors are immutable. A vector has always 4 elements; 1d, 2d, 3d and 4d vectors may be represented by leaving unused elements (if any) to their default zero value. Vector elements may be accessed either by an index or by a string. For example
```
v = vec.new(1, 2, 3, 4)
v[1]   -- returns 1
v.z    -- returns 3
v["y"] -- returns 2
```

New vectors can also be created by _swizzling_. For example
```
v = vec.new(1, 2, 3, 4)
v.xxyz -- returns vec(1, 1, 2, 3)
v.yzw  -- returns vec(2, 3, 4, 0)
v.xy   -- returns vec(1, 2, 0, 0)
```

The following arithmetic operators are supported (where v stands for vector, s for scalar):
  * `v+v, v-v`: addition and subtraction of two vectors.
  * `v*v`: componentwise multiplication.
  * `-v`: vector negation.
  * `v*s, s*v`: multiplication of a vector and a scalar.
  * `v/s`: vector divided by scalar.

# Vector API #

The vector module brings the following new functions into Lua:

  * `vec.new(x, y, z, w)`: Returns a new vector initialized with the given elements. All elements are optional; missing elements are set to zero.
  * `vec.dot(a, b)`: Returns the dot product of two given vectors. This works for 1d, 2d, 3d and 4d vectors as long as unused elements (if any) are left to zero.
  * `vec.cross(a, b)`: Returns the 3d cross product of two given vectors. The fourth component of the returned vector is always 0.
  * `vec.length(v)`: Returns the length of the given vector. This works for 1d, 2d, 3d and 4d vectors as long as unused elements (if any) are left to zero.
  * `vec.normalize(v)`: Returns the normalized vector of the given vector. This works for 1d, 2d, 3d and 4d vectors as long as unused elements (if any) are left to zero.

The module also contains the following predefined constants:

  * `vec.zero`: Equivalent to vec(0, 0, 0, 0).
  * `vec.one`: Equivalent to vec(1, 1, 1, 1).