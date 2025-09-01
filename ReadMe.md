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

# 2、GLSL中的矩阵

**GLSL中使用的矩阵是列主序，列主序在内存中是按列存储的，同时m\[i][j]的i是列索引，j是行索引；glm库也是列主序，m\[i][j]的i是行索引，j是列索引。因此可以直接把glm生成的正交投影矩阵和透视投影矩阵传递给GLSL的shader，不需要矩阵转置。**

经过MVP变换后，物体的坐标位于裁剪空间(clip space)，再经过透视除法后，物体的坐标变成裁剪坐标（坐标范围为[-1, 1]^3）。

之所以称为裁剪空间，是因为在这个阶段GPU会执行裁剪操作。裁剪（Clipping）是指 **移除那些位于视锥体（View Frustum）之外的顶点/三角形**，目的是：

- 避免渲染不可见的顶点或三角形
- 节省 GPU 的光栅化和片段处理开销

GPU 在 **裁剪阶段（Clipping Stage）** 对三角形进行判断：
$$
−w≤x,y,z≤w
$$
超出范围的顶点/三角形会被裁掉或截断

只保留视锥体内的三角形进行 **后续光栅化**。

## 裁剪和光栅化关系

1. **Vertex Shader** 输出**裁剪空间**顶点 `(x, y, z, w)`
2. **Clipping**：GPU 判断三角形是否在视锥内
   - 如果完全在外 → 丢弃
   - 如果部分在外 → 对三角形边进行裁剪，生成新的顶点
3. **Perspective Division / NDC**：对剩余顶点做透视除法 `(x/w, y/w, z/w)`
4. **Viewport Transform**：映射到屏幕空间

具体来说，从顶点着色器到片段着色器之间有一个光栅化步骤，该步骤执行如下操作：

**透视除法（Perspective Division）**

- 将裁剪空间坐标除以 `w` → 得到 NDC 坐标 `[-1,1]`

**视口变换（Viewport Transform）**

- 将 NDC 映射到屏幕像素坐标 `(screenX, screenY)`

**生成片段（Fragment Generation）**

- 对三角形覆盖的每个像素生成片段
- 对顶点着色器输出到片段着色器的变量（用out标识的变量）进行 **透视插值**
- 片段带有屏幕坐标、深度值、颜色/纹理坐标等信息

**裁剪 / 剔除**

- 屏幕外或不可见的片段丢弃
- 深度测试和模板测试在后续片段着色器中执行

在glsl中，`gl_Position`是顶点着色器中必须写入的**裁剪空间坐标**，`gl_FragCoord`是GPU管线在光栅化阶段生成并提供给片段着色器的内置只读变量，它的xy分量是经过透视除法和视口变换后的**屏幕坐标**，z分量是深度值，在OpenGL中范围为[0, 1]。视口大小是可以指定的，例如，在OpenGL中，可以通过`glViewport`定义视口的左下角坐标，以及宽度:

```C++
glViewport(0, 0, width, height);
```

视口的像素中心坐标范围如下：

$x\in[0.5,viewport\_width−0.5], y \in [0.5, viewport\_height - 0.5]$。

# 3、正交投影矩阵

首先明确，无论左手坐标系还是右手坐标系，X轴正方向都是屏幕右方向，Y轴正方向都是屏幕上方向。不管OpenGl还是D3D/Vulkan，我们要求用户传入近远平面距离相机的距离n和f必须为正数，因为它们表示距离，但在坐标系内部，实际上左手坐标系的近远平面均为正数，右手坐标系的近远平面均为负数。因此，投影矩阵的手性区分可以理解为API规定传入的n和f必须为正数，导致在右手坐标系下实际近远平面位置和用户传入参数不同！

对于D3D/Vulkan（左手坐标系），它的正交投影矩阵如下（代入n和f验证）：
$$
\begin{matrix}
\frac{2}{r-l}&0&0&-\frac{r+l}{r-l}\\
0&\frac{2}{t-b}&0&-\frac{t+b}{t-b}\\
0&0&\frac{1}{f-n}&-\frac{n}{f-n}\\
0&0&0&1
\end{matrix}
$$
对于OpenGL（右手坐标系），它的正交投影矩阵如下（代入-n和-f验证）：
$$
\begin{matrix}
\frac{2}{r-l}&0&0&-\frac{r+l}{r-l}\\
0&\frac{2}{t-b}&0&-\frac{t+b}{t-b}\\
0&0&-\frac{2}{f-n}&-\frac{f+n}{f-n}\\
0&0&0&1
\end{matrix}
$$
**它们的第三行的区别是由于D3D/Vulkan的z映射到[0, 1]而OpenGL映射到[-1, 1]。**

glm中之所以有四种正交投影矩阵，是因为glm规定传入API的n和f必须为正数，因此是它的实现导致与以上公式不符，**投影矩阵的表示实际上是手性无关的。**在glm中，按手性（左手：D3D/Vulkan/Metal，右手：OpenGL）和深度值映射到[0, 1]还是[-1, 1]，可以有四种矩阵，左手[-1, 1]坐标系的构造如下：

```C++
	template<typename T>
	GLM_FUNC_QUALIFIER mat<4, 4, T, defaultp> orthoLH_NO(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		mat<4, 4, T, defaultp> Result(1);
		Result[0][0] = static_cast<T>(2) / (right - left);
		Result[1][1] = static_cast<T>(2) / (top - bottom);
		Result[2][2] = static_cast<T>(2) / (zFar - zNear);
		Result[3][0] = - (right + left) / (right - left);
		Result[3][1] = - (top + bottom) / (top - bottom);
		Result[3][2] = - (zFar + zNear) / (zFar - zNear);
		return Result;
	}
```

即：
$$
\begin{matrix}
\frac{2}{r-l}&0&0&-\frac{r+l}{r-l}\\
0&\frac{2}{t-b}&0&-\frac{t+b}{t-b}\\
0&0&\frac{2}{f-n}&-\frac{f+n}{f-n}\\
0&0&0&1
\end{matrix}
$$
但是看右手[-1, 1]正交投影矩阵如下：

```C++
	template<typename T>
	GLM_FUNC_QUALIFIER mat<4, 4, T, defaultp> orthoRH_NO(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		mat<4, 4, T, defaultp> Result(1);
		Result[0][0] = static_cast<T>(2) / (right - left);
		Result[1][1] = static_cast<T>(2) / (top - bottom);
		Result[2][2] = - static_cast<T>(2) / (zFar - zNear);
		Result[3][0] = - (right + left) / (right - left);
		Result[3][1] = - (top + bottom) / (top - bottom);
		Result[3][2] = - (zFar + zNear) / (zFar - zNear);
		return Result;
	}
```

即：
$$
\begin{matrix}
\frac{2}{r-l}&0&0&-\frac{r+l}{r-l}\\
0&\frac{2}{t-b}&0&-\frac{t+b}{t-b}\\
0&0&-\frac{2}{f-n}&-\frac{f+n}{f-n}\\
0&0&0&1
\end{matrix}
$$
如果直接代入zNear和zFar我们无法得到-1和1。我们需要代入-zNear和-zFar，这是因为**glm的正交和透视矩阵API要求传入的n和f为正值！但事实上在右手坐标系下这两个值是负的。**再次注意，这里的公式不同只是glm实现上认定n和f必须为正值导致的！

> 指定透视投影矩阵我们只需要直接指定6个参数：上下左右近远平面的位置。

# 4、透视投影矩阵

正交投影矩阵没有近大远小的效果，我们需要一个能够变换后产生透视效果的矩阵，这就是透视压缩矩阵，这个矩阵需要满足如下特性：

- 和原坐标相乘后，x和y坐标除以z，这样的矩阵有很多，因此需要第二个条件约束
- 近平面的点坐标不变，近平面和远平面上的点的z坐标不变（线性变形）

同样地，我们规定n和f必须大于0，对于D3D/Vulkan，**透视压缩矩阵**如下：
$$
\begin{matrix}
1&0&0&0\\
0&1&0&0\\
0&0&\frac{n+f}{n}&-f\\
0&0&\frac{1}{n}&0
\end{matrix}
$$
对于OpenGL，透视压缩矩阵如下：
$$
\begin{matrix}
1&0&0&0\\
0&1&0&0\\
0&0&\frac{n+f}{n}&f\\
0&0&-\frac{1}{n}&0
\end{matrix}
$$
有的资料给出的透视矩阵每个元素都乘上了n，这是由于齐次坐标的特性，可以对该矩阵乘上一个常数，保持意义不变。

以D3D/Vulkan为例，我们可以看到(x, y, z, 1)经过透视压缩矩阵变换后变为$(x, y, z\frac{n+f}{n}-f, \frac{z}{n})$，即$(\frac{nx}{z}, \frac{ny}{z}, n+f-\frac{nf}{z}, 1)$，则透视压缩矩阵对z轴的压缩曲线如下：

<img src="ReadMe-imgs/perspective_press_z.jpg" style="zoom:25%;" />

透视压缩矩阵的效果是**几乎所有点被压缩的更靠近远平面了**（除近、远平面上的点外）。**透视压缩仅和n、f相关，即只和物体举例我们的远近相关！**

做完透视压缩，我们还要进行一次正交投影才能把所有的点变换到裁剪空间，因此最终的透视投影矩阵为
$$
M_{perspective}=M_{Orthographic}*M_{Press}
$$
因此对于OpenGL，它的透视投影矩阵为：
$$
\begin{matrix}
\frac{2n}{r-l}&0&\frac{l+r}{l-r}&0\\
0&\frac{2n}{t-b}&\frac{b+t}{b-t}&0\\
0&0&-\frac{2}{f-n}&-\frac{f+n}{f-n}\\
0&0&1&0
\end{matrix}
$$
对于D3D/Vulkan，它的透视投影矩阵为（这里的fov均为fovy，即y轴方向的field of view）：

<img src="ReadMe-imgs/perspective.png" style="zoom:80%;" />

指定透视投影矩阵无法像正交投影那样指定六个平面位置，因为尽管透视压缩会对z轴进行线性压缩，但是x轴和y轴被压缩成什么样了我们不知道。通常我们通过如下几个参数指定透视投影矩阵：

- fovy（垂直方向fov，弧度值，有$tan(fovy/2)=\frac{t-b}{2n}$），aspect_ratio（宽高比，即$\frac{r-l}{t-b}$）,n(近平面)，f(远平面)

**注意，同样有左手系和右手系之分，由于要求n和f均大于0，只需要在对应的矩阵元素前取负即可。**根据深度值取[-1, 1]还是[0,1]以及左右手系不同，glm同样给出了四种不同的实现，可以根据需求使用。一般教程给出的是左手系的深度值映射到[-1, 1]

# 5、窗口DPI

1️⃣ 程序里指定的尺寸通常是 **逻辑像素**

在大多数跨平台窗口和 GUI 编程环境（GLFW、SDL、Qt、Win32 API 等）中：程序里指定的尺寸通常是 **逻辑像素**。

例如我们使用glfw创建窗口

```C++
glfwCreateWindow(w, h, "My Window", nullptr, nullptr);
```

`w` 和 `h` 是 **逻辑像素（Logical Pixels）**，也就是 UI 系统看到的尺寸，不直接等于物理像素。

逻辑像素 = 用户希望看到的“窗口大小”，独立于显示器 DPI。

2️⃣ 物理像素 vs 逻辑像素

| 类型     | 含义               | Windows 200%缩放  |
| -------- | ------------------ | ----------------- |
| 逻辑像素 | 程序指定的窗口大小 | w\*h=800\*600     |
| 物理像素 | 实际显示器上的像素 | 2w\*2h=1600\*1200 |

3️⃣ 帧缓冲大小（OpenGL 渲染时用的）

OpenGL 需要知道 **实际像素数量** 来正确设置视口：

```C++
int fb_w, fb_h;
glfwGetFramebufferSize(window, &fb_w, &fb_h);  // 返回物理像素
glViewport(0, 0, fb_w, fb_h);
```

**`glViewport` 的原理**

它把 **标准化设备坐标（Normalized Device Coordinates, NDC）** 转换成 **窗口坐标（Window Coordinates）**。

在 OpenGL 渲染管线中，顶点经过：

1. 模型变换（Model）
2. 视图变换（View）
3. 投影变换（Projection）

得到 **裁剪空间坐标** → 除以 w 得到 **标准化设备坐标 (x_ndc, y_ndc, z_ndc)**

- NDC 范围固定在[−1,1]

接下来需要把 NDC 映射到**实际屏幕窗口像素坐标**，这一步就是 `glViewport` 做的。假设调用：

```C++
glViewport(x, y, width, height);
```

- `(x, y)` → 左下角窗口坐标（窗口左下角相对于屏幕的偏移）

- `width, height` → 窗口尺寸

那么 NDC → Window 坐标的映射为：
$$
x_w=x+\frac{width}{2}*(x_{nd}+1)\\
y_w=y+\frac{height}{2}*(y_{nd}+1)
$$
注意 **左下角是原点**（OpenGL 默认窗口坐标系）。

**为什么我们需要这个函数？**

**窗口可调大小**时，OpenGL 默认不会自动调整视口。

如果不更新 `glViewport`，渲染出来的图形会 **拉伸或压缩**。要看到拉伸或压缩的效果需要我们自定义FBO，在不自定义FBO时，OpenGL绘制在默认帧缓冲内（调用glfwCreateWindow时创建），对于默认帧缓冲，有些平台/驱动会自动 **将渲染内容按窗口大小适配**，所以即便你不调用 `glViewport`，渲染内容看起来也不会拉伸。

**物理PPI/DPI**

PPI（Pixels per inch）通常使用于计算机屏幕，DPI (Dots per inch)通常使用于打印机；但是这两者之间通常被混用，需要在具体的上下文去理解其含义。

以一块4K 27英寸显示器为例，27英寸是对角线，它的宽约为23.5英寸，高为13.2英寸，物理尺寸和像素宽高比都是16:9，每个像素基本为正方形。
$$
PPI=\frac{水平像素数}{屏幕宽度（英寸）}=\frac{垂直像素数}{屏幕高度（英寸）}
$$
因此这块显示器的PPI为$\frac{3840}{23.5} \approx 163$。

## 4️⃣ 总结

- **窗口大小、UI 元素尺寸** → 逻辑像素（程序指定）
- **帧缓冲大小、OpenGL 渲染尺寸** → 物理像素（可能大于逻辑像素，受 DPI 缩放影响）
- **DPI 缩放** 是逻辑像素到物理像素的桥梁，程序通常不直接处理物理像素，除非涉及精确渲染。

# 6、OpenGL DSA接口

从4.5版本开始，OpenGL引入了一系列DSA接口，让我们在操作OpenGL中的一系列object时不需要先选中再操作，在我们知道对象的id的情况下可以直接一行代码搞定。

以创建纹理为例，在4.5之前，我们使用如下的代码创建纹理：

```C++
    glGenTextures(1, &m_renderer_id);
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_local_buffer);
```

在4.5+，代码如下：

```C++
    glCreateTextures(GL_TEXTURE_2D, 1, &m_renderer_id);
    glBindTextureUnit(slot, m_renderer_id);

    glTextureParameteri(m_renderer_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_renderer_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(m_renderer_id, 1, GL_RGBA8, m_width, m_height); // 分配存储（现代 DSA，替代 glTexImage2D）
    glTextureSubImage2D(m_renderer_id, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE,
                        m_local_buffer); // 上传像素数据
```

有三个原因让我们使用DSA接口

1. **不容易出错**

   相比于传统的状态机接口， `glTexParameteri` 操作的对象完全取决于当前绑定的 `GL_TEXTURE_2D`，这带来几个问题：

   - 需要不断 bind / unbind，容易出错。
   - 调用顺序对正确性影响很大。

   DSA接口`glTextureParameteri`则直接指定对象，不需要再考虑 “现在谁绑在 GL_TEXTURE_2D 上”，逻辑更清晰。

2. ### 减少状态切换（性能更好）

   绑定操作本身是有开销的（驱动层需要修改上下文状态）。
    在复杂场景下，频繁的 `glBindTexture`、`glBindBuffer`、`glBindFramebuffer` 都会拖慢性能。
    DSA 避免了这种多余的绑定。

3. 更符合现代 API 设计

   现代图形 API（如 Vulkan、D3D12、Metal）都放弃了全局状态机，采用“直接操作资源”的模型。DSA 让 OpenGL 的代码风格更接近这些 API，学习和迁移更容易。

但是需要注意，OpenGL 的核心仍然是 **全局状态机**（context + state machine）-所有渲染、资源操作最终都会修改 context 中的某些状态，GPU 执行时依赖这些状态，这一点在 DSA 出现后也没有变。

#### 传统接口 vs DSA 的区别

- **传统接口**（pre-4.5）：
   需要先 `glBindXxx(target, id)`，再调用 `glXxxParameteri(target, ...)`。
   驱动内部会查“当前 target 绑定的对象”，然后修改它。
- **DSA 接口**（4.5+）：
   直接传入对象 ID，例如 `glTextureParameteri(tex, pname, param)`。
   驱动内部会根据传入的 ID 定位到对象，然后修改它。

👉 **区别仅在于：驱动查找对象的方式不同**

- 传统接口：从绑定点查找。
- DSA 接口：直接用对象 ID。

最终效果：都是修改了 GPU 上的对象数据，渲染流水线和运行模式没有本质变化。

#### 为什么要引入 DSA

引入 DSA 并不是因为 GPU 的工作模式变了，而是为了：

- **减少冗余的状态绑定**（性能 + 清晰度）
- **降低开发者心智负担**（不用再担心当前绑定的是哪个对象）
- **与现代 API 接轨**（Vulkan、D3D12 没有全局状态机，都是显式对象操作）

#### 驱动层实现

很多驱动在实现 DSA 时，**内部依然会走绑定路径**：

- 例如 `glTextureParameteri(tex, ...)` 可能会在驱动里临时绑定 `tex`，调用旧的逻辑，再解绑。
- 只是在 API 层隐藏了这些细节，对开发者透明。
- 所以它并不是 GPU 硬件层面上的“革命性变化”，而是 API 的改进。

