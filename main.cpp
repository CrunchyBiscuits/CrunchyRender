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
// void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
// 	bool steep = false;
// 	// 判断斜率>1还是在0到1之间
// 	if (std::abs(x1 - x0) < std::abs(y1 - y0))
// 	{
// 		std::swap(x1, y1);
// 		std::swap(x0, y0);
// 		steep = true;
// 	}

// 	// 判断符号
// 	if (x0 > x1)
// 	{
// 		std::swap(x0, x1);
// 		std::swap(y0, y1);
// 	}

// 	int dx = x1 - x0;
// 	int dy = y1 - y0;

// 	// 优化后的误差项
// 	// 将误差项变成e*2*dx
// 	float derror = std::abs(dy)*2;
// 	float error = 0;
// 	int y = y0;
// 	// 优化判断的计算
// 	if (steep)
// 	{
// 		for (int x = x0; x <= x1; x++)
// 		{
// 			// 斜率>1结果需交换x和y
// 			image.set(y, x, color);
// 			error += derror;
// 			if (error > dx)
// 			{
// 				y += (y1 > y0 ? 1 : -1);
// 				error -= dx * 2;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		for (int x = x0; x <= x1; x++)
// 		{
// 			image.set(x, y, color);
// 			error += derror;
// 			if (error > dx)
// 			{
// 				y += (y1 > y0 ? 1 : -1);
// 				error -= dx*2;
// 			}
// 		}
// 	}

// }

// 第六次尝试
// 转换成向量
void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
	bool steep = false;
	// 判断斜率>1还是在0到1之间
	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y))
	{
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}

	if (p0.x > p1.x)
	{
		std::swap(p0.x, p1.x);
		std::swap(p0.y, p1.y);
	}

	int dx = p1.x - p0.x;
	int dy = p1.y - p0.y;

	// 优化后的误差项
	// 将误差项变成e*2*dx
	float derror = std::abs(dy)*2;
	float error = 0;
	int y = p0.y;
	// 优化判断的计算
	if (steep)
	{
		for (int x = p0.x; x <= p1.x; x++)
		{
			// 斜率>1结果需交换x和y
			image.set(y, x, color);
			error += derror;
			if (error > dx)
			{
				// 根据上升还是下降对y进行取值
				y += (p1.y > p0.y ? 1 : -1);
				error -= dx * 2;
			}
		}
	}
	else
	{
		for (int x = p0.x; x <= p1.x; x++)
		{
			image.set(x, y, color);
			error += derror;
			if (error > dx)
			{
				y += (p1.y > p0.y ? 1 : -1);
				error -= dx*2;
			}
		}
	}
}

// 第二课 三角光栅化和背面剔除
// 第一次完整光栅化
//void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
//	// 将三个点进行升序排序
//	if (t0.y>t1.y)std::swap(t0, t1);
//	if (t0.y>t2.y)std::swap(t0, t2);
//	if (t1.y>t2.y)std::swap(t1, t2);
//	// 分成平顶和平底两部分进行光栅化
//	// 总的高度
//	int total_height = t2.y - t0.y;
//	// 底部三角
//	for (int y = t0.y; y <= t1.y; y++)
//	{
//		// 部分的高度
//		int segment_height = t1.y - t0.y + 1;
//		// 分别计算两边的插值
//		float alpha = (float) (y - t0.y) / total_height;
//		float beta = (float) (y - t0.y) / segment_height;
//		Vec2i A = t0 + (t2 - t0) * alpha;
//		Vec2i B = t0 + (t1 - t0) * beta;
//		// 保证从左往右扫描
//		if (A.x > B.x)std::swap(A, B);
//		// 渲染线
//		for (int i = A.x; i <= B.x ; i++)
//		{
//			image.set(i, y, color);
//		}
//	}
//	// 顶部三角
//	for (int y = t1.y; y <= t2.y; y++)
//	{
//		int segment_height = t2.y - t1.y;
//		// 对长的那条边始终是对总高度和t0作对比的
//		float alpha = (float)(y - t0.y) / total_height;
//		float beta = (float)(y - t1.y) / segment_height;
//		// alpha操作谨记t0
//		Vec2i A = t0 + (t2 - t0) * alpha;
//		Vec2i B = t1 + (t2 - t1) * beta;
//		// 渲染线
//		for (int i = A.x; i <= B.x; i++)
//		{
//			image.set(i, y, color);
//		}
//	}
//}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
	// 这不是三角形哈哈哈哈哈哈哈哈
	if (t0.y == t1.y && t0.y == t2.y)return;

	// 将三个点进行降序排序
	if (t0.y > t1.y)std::swap(t0, t1);
	if (t0.y > t2.y)std::swap(t0, t2);
	if (t1.y > t2.y)std::swap(t1, t2);

	// 总的高度
	int total_height = t2.y - t1.y;

	// 合并进行渲染
	for (int i = 0; i < total_height; i++)
	{
		// 判断是否到第二部分，或者开始三角形就是平底的
		bool second_half = i > (t1.y - t0.y) || t1.y==t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;

		float alpha = (float)i / total_height;
		// 如果是在第二部分，应当减去第一段的值
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;

		// 进行插值
		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;

		// 检查A，B位置
		if (A.x > B.x)std::swap(A, B);
		// 扫描
		for (int j = A.x; j <= B.x; j++)
		{
			// 从t0开始
			image.set(j, t0.y + i, color);
		}
	}
}


int main(int argc, char** argv) {
	//line(13, 20, 80, 40, image, white);
	//image.set(52, 41, red);
	//if (2 == argc) {
	//	model = new Model(argv[1]);
	//}
	//else {
	//	model = new Model("african_head.obj");
	//}
	//TGAImage image(width, height, TGAImage::RGB);
	//for (int i = 0; i < model->nfaces(); i++) {
	//	std::vector<int> face = model->face(i);
	//	for (int j = 0; j < 3; j++) {
	//		Vec3f v0 = model->vert(face[j]);
	//		Vec3f v1 = model->vert(face[(j + 1) % 3]);
	//		int x0 = (v0.x + 1.) * width / 2.;
	//		int y0 = (v0.y + 1.) * height / 2.;
	//		int x1 = (v1.x + 1.) * width / 2.;
	//		int y1 = (v1.y + 1.) * height / 2.;
	//		line(x0, y0, x1, y1, image, white);
	//	}
	//}
	TGAImage image(width, height, TGAImage::RGB);

	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

	triangle(t0[0], t0[1], t0[2], image, red);
	triangle(t1[0], t1[1], t1[2], image, white);
	triangle(t2[0], t2[1], t2[2], image, white);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}
