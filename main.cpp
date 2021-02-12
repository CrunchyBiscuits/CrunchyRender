#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

// Bresenham直线算法
// 第一次尝试, 最简单的画线方法，数值微分法
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//	for (float t = 0.; t < 1; t+=.01)
//	{
//		int x = x0 + (x1 - x0) * t;
//		int y = y0 + (y1 - y0) * t;
//		image.set(x, y, color);
//	}
//}

// 第二次尝试
// 第一次尝试的代码的问题是线是否连续的问题，如果步进选大了就会出现断点
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//	for (int x = x0; x <=x1; x++)
//	{
//		float t = (x - x0) / (float)(x1-x0);
//		int y = y0 * (1. - t) + y1 * t;
//		image.set(x, y, color);
//	}
//}

// 第三次尝试
// 通过交换点的方式来防止象限出现问题
// 简单但是仍有不足，因为周边的界限有问题
// 并且误差项d仍然没有优化
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//	// 通过steep来判断斜率是否大于1
//	bool steep = false;
//
//	// 
//	if (std::abs(x0-x1)<std::abs(y0-y1))
//	{
//		std::swap(x0, y0);
//		std::swap(x1, y1);
//		steep = true;
//	}
//
//	if (x0>x1)
//	{
//		std::swap(x0, x1);
//		std::swap(y0, y1);
//	}
//
//	for (int x = x0; x <= x1; x++)
//	{
//		float t = (x - x0) / (float)(x1 - x0);
//		int y = y0 * (1. - t) + y1 * t;
//		if (steep)
//		{
//			image.set(y, x, color);
//		}
//		else
//		{
//			image.set(x, y, color);
//		}
//	}
//}

// 第四次尝试
// 加入误差项
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//	bool steep = false;
//
//	if (std::abs(x1-x0)<std::abs(y1-y0))
//	{
//		std::swap(x1, y1);
//		std::swap(x0, y0);
//		steep = true;
//	}
//
//	if (x0>x1)
//	{
//		std::swap(x0, x1);
//		std::swap(y0, y1);
//	}
//
//	int dx = x1 - x0;
//	int dy = y1 - y0;
//	float derror = std::abs(dy/(float)dx);
//	float error = 0;
//	int y = y0;
//	for (int x = x0; x <=x1; x++)
//	{
//		if (steep)
//		{
//			image.set(y, x, color);
//		}
//		else
//		{
//			image.set(x, y, color);
//		}
//		error += derror;
//		if (error>.5)
//		{
//			y += (y1 > y0 ? 1 : -1);
//			error -= 1.;
//		}
//	}
//}

// 第五次尝试
// 优化浮点计算
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	bool steep = false;
	// 判断斜率>1还是在0到1之间
	if (std::abs(x1 - x0) < std::abs(y1 - y0))
	{
		std::swap(x1, y1);
		std::swap(x0, y0);
		steep = true;
	}

	// 判断符号
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	// 优化后的误差项
	// 将误差项变成e*2*dx
	float derror = std::abs(dy)*2;
	float error = 0;
	int y = y0;
	// 优化判断的计算
	if (steep)
	{
		for (int x = x0; x <= x1; x++)
		{
			// 斜率>1结果需交换x和y
			image.set(y, x, color);
			error += derror;
			if (error > dx)
			{
				y += (y1 > y0 ? 1 : -1);
				error -= dx * 2;
			}
		}
	}
	else
	{
		for (int x = x0; x <= x1; x++)
		{
			image.set(x, y, color);
			error += derror;
			if (error > dx)
			{
				y += (y1 > y0 ? 1 : -1);
				error -= dx*2;
			}
		}
	}

}

int main(int argc, char** argv) {
	//line(13, 20, 80, 40, image, white);
	//image.set(52, 41, red);
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("african_head.obj");
	}
	TGAImage image(width, height, TGAImage::RGB);
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.) * width / 2.;
			int y0 = (v0.y + 1.) * height / 2.;
			int x1 = (v1.x + 1.) * width / 2.;
			int y1 = (v1.y + 1.) * height / 2.;
			line(x0, y0, x1, y1, image, white);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}
