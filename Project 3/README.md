# Project 3(Haolan Xu:34326768)

## The video link

https://recordit.co/FpfBifjoVw

## The project

### CMakeLists.txt

Add

```c++
set(CMAKE_CXX_STANDARD 17)
```

In order to update the version of C++

### Common

Update **objloader.cpp** and **vboindexer.cpp** to load models' uv coordinates into OpenGL

### Use triangles instead of Quads to render models

As with a lot of other functionality, `GL_QUADS` was deprecated in version 3.0 and **removed** in **version 3.1**. Since the point is that your GPU renders triangles, not quads. And it is pretty much trivial to construct a rectangle from two triangles, so the API doesn't really need to be burdened with the ability to render quads natively. So I still use the `GL_TRIANGLES` to render my model

### The key 'P' and 'p'

Because we don't need to distinguish between 'P' and 'p', I just use the 'p' for task 3

### Picking

For picking, I use the **ray picking** for my project.

### The code share

I share my code with Pochuan Liang
