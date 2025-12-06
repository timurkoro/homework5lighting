#include "RasterizationScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Window.h"
#include "Camera.h"
#include "Time.h"
#include "Mesh.h"

void Example1();
void Example2();
void Example3();
std::vector<Vector3> VerticesFromIndices(std::vector<Vector3> unique_vertices, std::vector<uint16_t> indices);

void RasterizationScene::OnLoad()
{
}

void RasterizationScene::OnUnload()
{
}

void RasterizationScene::OnUpdate(float dt)
{
	ClearColor(&gImageCPU, BLACK);
	ClearDepth(&gImageCPU, 1.0f);
	//Example1();
	Example2();
	//Example3();
}

// Wireframe clip-test
void Example1()
{
	float tt = TotalTime();
	Matrix rx = RotateX(500.0f * tt * DEG2RAD);
	Matrix ry = RotateY(250.0f * tt * DEG2RAD);
	Matrix rz = RotateZ(100.0f * tt * DEG2RAD);
	Matrix tx = Translate(5.0f, 0.0f, 0.0f);
	Matrix tz = Translate(0.0f, 0.0f, 3.0f + sinf(tt) * 3.0f);

	Matrix world = rx * ry * tx * rz * tz;
	Matrix view = LookAt({ 0.0f, 0.0f, 10.0f }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90.0f * DEG2RAD, 1.0f, 0.1f, 100.0f);
	Matrix mvp = world * view * proj;

	Mesh mesh = gMeshHead;
	for (int i = 0; i < mesh.positions.size(); i++)
		mesh.positions[i] = Clip(mvp, mesh.positions[i]);

	if (!mesh.indices.empty())
		mesh.positions = VerticesFromIndices(mesh.positions, mesh.indices);

	for (int i = 0; i < mesh.positions.size() / 3; i++)
		DrawFaceWireframes(&gImageCPU, mesh.positions.data(), i, GREEN);
}

// Vertex attribute fragment interpolation test
void Example2()
{
	float tt = TotalTime();

	Matrix world = RotateY(50.0f * tt * DEG2RAD) * Translate(0.0f, 0.0f, 7.0f);
	Matrix view = LookAt({ 0.0f, 0.0f, 10.0f }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90.0f * DEG2RAD, 1.0f, 0.1f, 100.0f);

	Matrix mvp = world * view * proj;
	DrawMesh(&gImageCPU, gMeshSphere, world, mvp);
}

// Fill-rate stress-test
void Example3()
{
	float tt = TotalTime();

	Matrix world = Scale(40.0f, 40.0f, 1.0f) + Translate(0.0f, 0.0f, 1.0f);
	Matrix view = LookAt({ 0.0f, 0.0f, 10.0f }, V3_ZERO, V3_UP);
	Matrix proj = Ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	
	for (int i = 0; i < 4; i++)
	{
		Matrix mvp = world * view * proj;
		DrawMesh(&gImageCPU, gMeshPlane, world, mvp);
	}
}

std::vector<Vector3> VerticesFromIndices(std::vector<Vector3> unique_vertices, std::vector<uint16_t> indices)
{
	std::vector<Vector3> positions;
	positions.resize(indices.size());

	for (int i = 0; i < positions.size(); i++)
		positions[i] = unique_vertices[indices[i]];

	return positions;
}
