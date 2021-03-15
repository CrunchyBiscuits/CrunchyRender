# CrunchyRenderer
一个简易的渲染引擎，主要来自ssloy大佬的[tinyrenderer](https://github.com/ssloy/tinyrenderer)

文件主要实现在main.cpp、gl.cpp和old.cpp，其他文件来自ssloy。

开始进行数据结构重构，ssloy老师得有点难读TAT

## 待办

* 天空盒
* 环绕式相机
* 骨骼动画
* 背面剔除
* 阴影贴图
* PBR
* CVV
* MSAA

## 实现内容

gl.cpp内容：

1. MVP矩阵
2. 重心坐标
3. 三角光栅化

main.cpp内容：

1. phong shader

old.cpp内容：

1. Bresenham直线算法

 	2. 三角光栅化扫描线算法

效果展示：
1. shader
![](imgs/shaderS.jpg)
2. zbuffer
![](imgs/zbuffer.jpg)

