# 1. OpenGL中的绑定操作

在 OpenGL 里，`bind` 函数出现得非常频繁，这其实是 **OpenGL 的状态机设计思想** 的体现。理解它的意义就可以解释为什么几乎所有资源都要先绑定。

OpenGL 是 **状态机（state machine）**：

- GPU 内部维护一堆状态：当前绑定的纹理、缓冲对象、VAO、shader program 等等。

  当你调用 `glBind*` 时，实际上是告诉 GPU：

  “接下来对这个类型的操作，都作用在这个对象上。”

  也就是说 **绑定对象 = 激活对象**。

以纹理对象为例：

```c++
glGenTextures(1, &texture);        // 创建纹理对象
glBindTexture(GL_TEXTURE_2D, texture); // 绑定纹理
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
```

`glBindTexture(GL_TEXTURE_2D, texture)` 之后：

- 所有针对 `GL_TEXTURE_2D` 的操作（上传数据、设置参数）都 **作用在 `texture` 对象上**。

如果你不 bind，GPU 就 **不知道你要操作哪个纹理**。

OpenGL 中几乎所有对象（VBO、VAO、Framebuffer、Texture、Shader Program）都遵循这个规则：

<img src="ReadMe-imgs/bind.png" style="zoom:50%;" />