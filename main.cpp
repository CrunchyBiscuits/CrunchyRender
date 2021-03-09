#include <vector>
#include <iostream>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "gl.h"

Model* model = NULL;
const int width = 800;
const int height = 800;

Vec3f light_dir(1, 1, 1);
Vec3f       eye(1, 1, 6);
Vec3f    center(0, 0, 0);
Vec3f        up(0, 1, 0);

// 高露德shader逐顶点的进行渲染
struct GouraudShader : public IShader {
    // 通过顶点shader进行变换，最后传给片元shader
    Vec3f varying_intensity; 

    virtual Vec4f vertex(int iface, int nthvert) {
        // 来自tinyrenderer，读取object相关的数据
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        // MVP矩阵变换转换到屏幕
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;
        // 获取漫反射强度，这里只有漫反射
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert) * light_dir);
        return gl_Vertex;
    }

    // 片元shader，这里是根据不同的强度进行的一个粗暴的转换
    virtual bool fragment(Vec3f bar, TGAColor& color) {
        float intensity = varying_intensity * bar;
        if (intensity > .85) intensity = 1;
        else if (intensity > .60) intensity = .80;
        else if (intensity > .45) intensity = .60;
        else if (intensity > .30) intensity = .45;
        else if (intensity > .15) intensity = .30;
        else intensity = 0;
        color = TGAColor(255, 155, 0) * intensity;
        return false;
    }
};

struct Shader : public IShader {
    mat<2, 3, float> varying_uv;  // same as above
    mat<4, 4, float> uniform_M;   //  Projection*ModelView
    mat<4, 4, float> uniform_MIT; // (Projection*ModelView).invert_transpose()

    // 顶点shader的作用，主要就是MVP
    virtual Vec4f vertex(int iface, int nthvert) {
        // 这里来自tinyRenderer
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        // 将顶点转换到裁剪空间
        return Viewport * Projection * ModelView * gl_Vertex;
    }

    // 片元shader主要任务，着色
    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec2f uv = varying_uv * bar;
        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
        Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
        // 反射方向计算
        Vec3f r = (n * (n * l * 2.f) - l).normalize();
        // 高光计算
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
        // 漫反射计算
        float diff = std::max(0.f, n * l);
        // 颜色值
        TGAColor c = model->diffuse(uv);
        color = c;
        for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + c[i] * (diff + .6 * spec), 255);
        return false;
    }
};
/*
int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("obj/african_head.obj");
    }

    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());
    light_dir.normalize();

    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    // phong shader;
    Shader shader;
    shader.uniform_M = Projection * ModelView;
    shader.uniform_MIT = (Projection * ModelView).invert_transpose();
    for (int i = 0; i < model->nfaces(); i++) {
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangle(screen_coords, shader, image, zbuffer);
    }

    // 设置左下为起始点
    image.flip_vertically(); 
    zbuffer.flip_vertically();
    image.write_tga_file("output.tga");
    zbuffer.write_tga_file("zbuffer.tga");

    delete model;
    return 0;
}*/