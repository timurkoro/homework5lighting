#pragma once
#include "Image.h"
#include "Mesh.h"

struct Rect
{
	int xMin;
	int xMax;
	int yMin;
	int yMax;
};

inline void DrawLineX(Image* image, int row, int x0, int x1, Color color)
{
	// for x in x0 to x1:
	//		set pixel at x, row
	for (int x = x0; x <= x1; x++)
		SetPixel(image, x, row, color);
}

inline void DrawLineY(Image* image, int col, int y0, int y1, Color color)
{
	// for y in y0 to y1:
	//		set pixel at col, y
	for (int y = y0; y <= y1; y++)
		SetPixel(image, col, y, color);
}

inline void DrawLine(Image* image, int x0, int y0, int x1, int y1, Color color)
{
	x0 = Clamp(x0, 0.0f, image->width - 1);
	x1 = Clamp(x1, 0.0f, image->width - 1);
	y0 = Clamp(y0, 0.0f, image->height - 1);
	y1 = Clamp(y1, 0.0f, image->height - 1);

	// let dx = x1 - x0
	// let dy = y1 - y0
	int dx = x1 - x0;
	int dy = y1 - y0;

	// let steps = abs dx if abs dx > abs dy, else abs dy
	// abs = "absolute value"
	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	// let xStep = dx / steps as float
	// let yStep = dy / steps as float
	float xStep = dx / (float)steps;
	float yStep = dy / (float)steps;

	// x = x0
	// y = y0
	// set pixel at x, y
	// for i in steps:
	//		set pixel at x, y
	//		x += xStep
	//		y += yStep
	float x = x0;
	float y = y0;
	for (int i = 0; i <= steps; i++)
	{
		SetPixel(image, x, y, color);
		x += xStep;
		y += yStep;
	}
}

inline void DrawRect(Image* image, int x, int y, int w, int h, Color color)
{
	// Option 1:
	// for ry in y to y + h:
	//		for rx in x to x + h:
	//			set pixel at rx, ry
	for (int ry = y; ry <= y + h; ry++)
	{
		for (int rx = x; rx <= x + w; rx++)
		{
			SetPixel(image, rx, ry, color);
		}
	}

	// Option 2:
	// Render vertical lines for all columns
	//for (int col = x; col <= x + w; col++)
	//	DrawLineY(image, col, y, y + h, color);

	// Option 3:
	// Render horizontal lines for all rows
	//for (int row = y; row <= y + h; row++)
	//	DrawLineX(image, row, x, x + w, color);
}

inline void DrawRectLines(Image* image, int x, int y, int w, int h, Color color)
{
	// Render vertical lines at x and x + w
	// Render horizontal lines at y and y + h
	DrawLineX(image, y + 0, x, x + w, color);
	DrawLineX(image, y + h, x, x + w, color);
	DrawLineY(image, x + 0, y, y + h, color);
	DrawLineY(image, x + w, y, y + h, color);
}

inline void DrawCircle(Image* image, int cx, int cy, int cr, Color color)
{
	int x = 0;
	int y = cr;
	int d = 3 - 2 * cr;

	auto line = [&](int lx, int ly, int l)
		{
			for (int dx = -l; dx <= l; dx++)
				SetPixel(image, lx + dx, ly, color);
		};

	while (y >= x)
	{
		line(cx, cy + y, x);
		line(cx, cy - y, x);
		line(cx, cy + x, y);
		line(cx, cy - x, y);

		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
		{
			d = d + 4 * x + 6;
		}
	}
}

inline void DrawEllipse(Image* img, int xc, int yc, int a, int b, Color color) {
	int x = 0;
	int y = b;

	// Region 1: where slope > -1
	int a2 = a * a;
	int b2 = b * b;
	int fa2 = 4 * a2;
	int fb2 = 4 * b2;

	// Initial decision parameter for region 1
	int d1 = b2 - (a2 * b) + (0.25f * a2);
	int dx = 2 * b2 * x;
	int dy = 2 * a2 * y;

	// Store previous points for line drawing
	int prevX = x, prevY = y;

	// Region 1: Connect points horizontally as we step x
	while (dx < dy) {
		// Draw horizontal line segments connecting symmetric points
		DrawLine(img, xc - x, yc + y, xc + x, yc + y, color);
		DrawLine(img, xc - x, yc - y, xc + x, yc - y, color);

		if (d1 < 0) {
			x++;
			dx += fb2;
			d1 += dx + b2;
		}
		else {
			x++;
			y--;
			dx += fb2;
			dy -= fa2;
			d1 += dx - dy + b2;
		}
	}

	// Region 2: where slope <= -1
	int d2 = b2 * (x + 0.5f) * (x + 0.5f) + a2 * (y - 1) * (y - 1) - a2 * b2;

	// Region 2: Connect points horizontally as we step y
	while (y >= 0) {
		// Draw horizontal line segments connecting symmetric points
		DrawLine(img, xc - x, yc + y, xc + x, yc + y, color);
		DrawLine(img, xc - x, yc - y, xc + x, yc - y, color);

		if (d2 > 0) {
			y--;
			dy -= fa2;
			d2 += a2 - dy;
		}
		else {
			x++;
			y--;
			dx += fb2;
			dy -= fa2;
			d2 += dx - dy + a2;
		}
	}
}

inline void DrawCircleLines(Image* image, int cx, int cy, int cr, Color color)
{
	int x = 0;
	int y = cr;
	int d = 3 - 2 * cr;

	while (y >= x)
	{
		SetPixel(image, cx + x, cy + y, color);
		SetPixel(image, cx - x, cy + y, color);
		SetPixel(image, cx + x, cy - y, color);
		SetPixel(image, cx - x, cy - y, color);
		SetPixel(image, cx + y, cy + x, color);
		SetPixel(image, cx - y, cy + x, color);
		SetPixel(image, cx + y, cy - x, color);
		SetPixel(image, cx - y, cy - x, color);

		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
		{
			d = d + 4 * x + 6;
		}
	}
}

inline void DrawFaceWireframes(Image* image, Vector3* positions, size_t face, Color color = WHITE)
{
	size_t vertex = face * 3;
	for (size_t i = 0; i < 3; i++)
	{
		Vector3 v0 = positions[vertex + i];
		Vector3 v1 = positions[vertex + ((i + 1) % 3)];

		v0.x = Remap(v0.x, -1.0f, 1.0f, 0, image->width  - 1);
		v0.y = Remap(v0.y, -1.0f, 1.0f, 0, image->height - 1);
		v1.x = Remap(v1.x, -1.0f, 1.0f, 0, image->width  - 1);
		v1.y = Remap(v1.y, -1.0f, 1.0f, 0, image->height - 1);

		// Toggle far-plane clipping (approximation):
		//if (v0.z > 1.0f || v1.z > 1.0) return;

		DrawLine(image, v0.x, v0.y, v1.x, v1.y, color);
	}
}

inline void DrawMesh(Image* image, Mesh mesh, Matrix world, Matrix mvp)
{
	std::vector<Vector3> vertices, positions, normals;
	vertices.resize(mesh.count);
	positions.resize(mesh.count);
	normals.resize(mesh.count);

	Matrix normal_matrix = NormalMatrix(world);
	for (size_t i = 0; i < mesh.count; i++)
	{
		int index = mesh.indices.empty() ? i : mesh.indices[i];

		Vector3 position = world * mesh.positions[index];
		Vector3 normal = normal_matrix * mesh.normals[index];
		Vector3 screen = Clip(mvp, mesh.positions[index]);

		screen.x = Remap(screen.x, -1.0f, 1.0f, 0.0f, image->width - 1.0f);
		screen.y = Remap(screen.y, -1.0f, 1.0f, 0.0f, image->height - 1.0f);
		screen.z = screen.z;

		positions[i] = position;
		vertices[i] = screen;
		normals[i] = normal;
	}

	// Triangle AABBs
	std::vector<Rect> rects;
	rects.resize(mesh.count / 3);
	for (size_t face = 0; face < mesh.count / 3; face++)
	{
		// Ensure min & max get overwritten
		int xMin = image->width - 1;
		int yMin = image->height - 1;
		int xMax = 0;
		int yMax = 0;

		// Determine min & max of face, ensure its on-screen
		const size_t vertex = face * 3;
		for (size_t i = 0; i < 3; i++)
		{
			int x = vertices[vertex + i].x;
			int y = vertices[vertex + i].y;
			xMin = std::max(0, std::min(xMin, x));
			yMin = std::max(0, std::min(yMin, y));
			xMax = std::min(image->width - 1, std::max(xMax, x));
			yMax = std::min(image->height - 1, std::max(yMax, y));
		}

		rects[face].xMin = xMin;
		rects[face].xMax = xMax;
		rects[face].yMin = yMin;
		rects[face].yMax = yMax;
	}

	for (size_t face = 0; face < mesh.count / 3; face++)
	{
		for (int x = rects[face].xMin; x <= rects[face].xMax; x++)
		{
			for (int y = rects[face].yMin; y <= rects[face].yMax; y++)
			{
				size_t vertex = face * 3;
				Vector3 v0 = vertices[vertex + 0];
				Vector3 v1 = vertices[vertex + 1];
				Vector3 v2 = vertices[vertex + 2];

				// Tri-linear interpolation, ensure 0.0 >= uvw <= 1.0
				Vector3 bc = Barycenter({ (float)x, (float)y, 0.0f }, v0, v1, v2);
				bool low = bc.x < 0.0f || bc.y < 0.0f || bc.z < 0.0f;
				bool high = bc.x > 1.0f || bc.y > 1.0f || bc.z > 1.0f;

				// Tip: trilinear-interpolation is A * u + B * v + C * w
				float depth = v0.z * bc.x + v1.z * bc.y + v2.z * bc.z;

				// Discard if pixel not in triangle
				if (low || high)
					continue;

				if (depth > GetDepth(*image, x, y))
					continue;
				SetDepth(image, x, y, depth);

				// let p0 = face position 0
				// let p1 = face position 1
				// let p2 = face position 2
				// let p = trilinear-interpolation(p0, p1, p2, bc)
				Vector3 p0 = positions[vertex + 0];
				Vector3 p1 = positions[vertex + 1];
				Vector3 p2 = positions[vertex + 2];
				Vector3 p = p0 * bc.x + p1 * bc.y + p2 * bc.z;

				// let n0 = face normal 0
				// let n1 = face normal 1
				// let n2 = face normal 2
				// let n = trilinear-interpolation(n0, n1, n2, bc)
				Vector3 n0 = normals[vertex + 0];
				Vector3 n1 = normals[vertex + 1];
				Vector3 n2 = normals[vertex + 2];
				Vector3 n = n0 * bc.x + n1 * bc.y + n2 * bc.z;

				//Vector3 d = V3_ONE * depth;
				Color color = Float3ToColor(&n.x);
				SetPixel(image, x, y, color);
			}
		}
	}
}
