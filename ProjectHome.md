# Introduction #

Lua (http://www.lua.org) is a small, clean, dynamically typed and one of the fastest interpreted languages making it an ideal choice for scripting in games. Indeed Lua has been adopted by many game makers both hobbyists and also commercial game developers, although Lua's history has nothing to do with the game industry. However, as Lua's design goal was to make the language and runtime as simple as possible, some features like garbage collection are not as efficient as in some much more complex language implementations. This makes Lua rather slow in vector processing which typically require lots of small temp objects (vectors) to be allocated. Vector processing is increasingly more important for modern realtime games, even in the scripting context where game state such as object positions needs to be updated. The goal of this project is to implement high performance vectors for Lua which should make the language even more useful for game developers.

There are several ways to implement vectors for Lua. The easiest and slowest is to use tables to represent vectors and use metatables to overload arithmetic and indexing operators. Lua is not very fast in numerical calculations so this is not a good approach for realtime applications such as games. Also, tables are garbage collected so having lots of small tables is not a good idea. Another solution is to implement vectors in C using the Lua userdata mechanism. This is clearly faster (see Benchmarks in wiki) but userdata types are still garbage collected. Also because C functions for common operations such as vector addition, dot product, etc. are only a few instructions, function call overhead with this approach is high.

A third way involves a hack. There is a special data type in lua called 'light userdata'. Light userdata objects are basically like userdata objects without garbage collection. Under the hood, they are mere pointers which a C module may use to refer to any arbitrary data allocated on the heap. Because light userdata objects are not collected an explicit free mechanism is needed. One possibility is to have a pool of memory preallocated for vectors, and sequentially allocate new vectors from this pool. The pool would need to be released periodically, like once per frame in the main loop of a game. While light userdata vectors are very fast, they have a major disadvantage: vectors may not persist over frames making it impossible to store game state with vectors. Therefore none of the discussed approaches fit perfectly the requirements of a realtime game application. Enter lua-vec.

Lua-vec takes a drastically different path. Lua-vec extends the core language and virtual machine with a new datatype, a four element 128-bit wide vector (four 32-bit float values). The new datatype is a first class type, like regular number type is. This means that there is no penalty for creating new vectors and there is no need for garbage collection. Vectors also don't allocate memory dynamically when created because they're value types, not objects. Vectors are created with a special creator function that converts up to four numbers to a vector. After creation you can use standard arithmetic operators on them.

An example:

```
v1 = vec.new(1,2,3,4)  -- creates a new vector with elements (1,2,3,4)
v2 = vec.new(2,3,4,5)
print(v1 + v2)         -- prints 'vec(3, 5, 7, 9)'
```

See wiki for a full reference of supported vector operations.

There are a few drawbacks with our approach. First of all because all primary lua data types are stored in a single data structure in C, this data structure needs to grow to make space for our vectors. Previously the structure was 8 bytes wide (enough space to hold a double or a pointer on 64-bit targets). Now this has grown to 16 bytes (four 4-byte floats). This means that all value types are now twice as big in memory, i.e. all primary types such as numbers, booleans and strings (actually string references) are twice as big. This may or may not be a problem depending on how much data the application is using. This is also the reason why vector elements are single precision floats rather than doubles; four 64-bit wide doubles would increase memory usage of all data types by a factor of four. Care must be taken when creating new vectors from numbers because precision or range problems may appear.

Even with the increased memory consumption and other drawbacks we feel that lua-vec could be used in game context. We think that the speed up (see Benchmarks in wiki) is big enough in many cases to justify the drawbacks.

# Design Choices #

When implementing lua-vec we encountered a few design choices. The first was vector mutators. All vectors in lua-vec are immutable, meaning that once created you can not change their content. This is akin to Lua strings; you have to create a new vector with the changed elements. The reason for this is that we could not find a simple way to implement value type mutators. Assignment statements like `(e)[i] = (v)` are executed so that expression (e) is first evaluated and its value is stored in a register. Then the VM runs OP\_SETTABLE which gets the reference to a table, or in our case the vector value, in a register. We could have modified this value with a dirty hack in OP\_SETTABLE but this would have just changed the value in register not the actual variable holding the value. Therefore this would have only worked for vectors stored in local variables which Lua keeps in registers.

Another choice was with the API. Following Lua's keep it simple principle there is only a single vector type which can be used to store 1d, 2d, 3d and 4d vectors with the assumption that unused components are left to their default zero value. This way various arithmetic operations such as vector addition, multiplication, dot products etc. work as expected.

# Implementation Details #

All changes to original Lua source code are marked with 'LUA-VEC' comment. As Lua is a very clean piece of software, only a few places needed to be changed. The most notable changes are:

  * A new type define LUA\_TVEC was added.
  * Value union now has a new field `float vec[4]` which is used to store components of a vector.
  * Lua VM was patched to support vectors with various arithmeric opcodes and OP\_GETTABLE.
  * `lua_pushvec(), lua_tovec(), luaL_checkvec()` etc. were added to the C API.

# Building Lua-vec #

No extra steps are required to build lua-vec. Just follow the usual Lua building and installation instructions. Basically all you have to do is execute `make <platform>`. Note however that you may not want to install lua-vec on top of existing Lua installation (the executables currently have the same name in lua-vec).

# Project Status #

Lua-vec is a highly experimental project. Nevertheless, Lua-vec is fully functional and no extra functionality is planned. However the library is still in beta stage and has not been extensively tested.

# License #

Lua-vec is licensed under the terms of the same MIT license as Lua. See COPYRIGHT file in sources for more details.