#include "SkinningScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "Geometry.h"
#include "ImageLoader.h"

struct Planet
{
	Vector3 position;
	Vector3 color;

	float radius;
	float rotationSpeed;
	float orbitSpeed;
};

static const int PLANET_COUNT = 9;
static Planet planets[PLANET_COUNT];
static bool fRaster = true;
static bool fDepth = false;
static float fFov = PI * 0.5f;
static float fNear = 0.1f;
static float fFar = 500.0f;

static Vector3 planetColors[PLANET_COUNT];		// Both
static Matrix planetWorld[PLANET_COUNT];		// Rasterization world matrix
static Matrix planetNormal[PLANET_COUNT];		// Raster only
static Matrix planetMvp[PLANET_COUNT];			// Raster only

static Framebuffer fFbo;
static Texture2D fColorRT;
static Texture2D fDepthRT;


void DrawPlanetsRaster();


void SkinningScene::OnLoad()
{

	gCamera = FromView(LookAt({ 200.0f, 100.0f, 150.0f }, V3_ZERO, V3_UP));
	SetMousePosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	SetMouseState(MOUSE_STATE_NORMAL);

	CreateTexture2D(&fColorRT, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
	CreateTexture2D(&fDepthRT, SCREEN_WIDTH, SCREEN_HEIGHT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST);

	// A depth texture must be attached in order for depth-testing and depth-writing to be performed
	CreateFramebuffer(&fFbo, SCREEN_WIDTH, SCREEN_HEIGHT);
	fFbo.colors[0] = &fColorRT;
	fFbo.depth = &fDepthRT;
	CompleteFramebuffer(&fFbo);

	// Sun
	planets[0].radius = 5.0f;
	planets[0].rotationSpeed = 24.0f;
	planets[0].orbitSpeed = 0.0f;
	planets[0].position = V3_ZERO;
	planets[0].color = { 0.97f, 0.38f, 0.09f };

	// Mercury
	planets[1].radius = 0.5f;
	planets[1].rotationSpeed = 58.7f;
	planets[1].orbitSpeed = 0.5f;
	planets[1].position = V3_RIGHT * 10.0f;
	planets[1].color = { 0.45f, 0.07f, 0.006f };

	// Venus
	planets[2].radius = 1.0f;
	planets[2].rotationSpeed = 243.0f;
	planets[2].orbitSpeed = 0.8f;
	planets[2].position = V3_RIGHT * 20.0f;
	planets[2].color = { 0.6f, 0.32f, 0.006f };

	// Earth
	planets[3].radius = 2.0f;
	planets[3].rotationSpeed = 1.0f;
	planets[3].orbitSpeed = 1.0f;
	planets[3].position = V3_RIGHT * 30.0f;
	planets[3].color = { 0.07f, 0.028f, 0.61f };

	// Mars
	planets[4].radius = 1.5f;
	planets[4].rotationSpeed = 1.1f;
	planets[4].orbitSpeed = 2.0f;
	planets[4].position = V3_RIGHT * 40.0f;
	planets[4].color = { 0.79f, 0.07f, 0.006f };

	// Jupiter
	planets[5].radius = 4.0f;
	planets[5].rotationSpeed = 0.4f;
	planets[5].orbitSpeed = 11.0f;
	planets[5].position = V3_RIGHT * 50.0f;
	planets[5].color = { 0.32f, 0.13f, 0.13f };

	// Saturn
	planets[6].radius = 3.0f;
	planets[6].rotationSpeed = 0.5f;
	planets[6].orbitSpeed = 29.4f;
	planets[6].position = V3_RIGHT * 60.0f;
	planets[6].color = { 0.45f, 0.45f, 0.21f };

	// Uranus
	planets[7].radius = 3.5f;
	planets[7].rotationSpeed = 0.8f;
	planets[7].orbitSpeed = 84.07f;
	planets[7].position = V3_RIGHT * 70.0f;
	planets[7].color = { 0.13f, 0.13f, 0.32f };

	// Neptune
	planets[8].radius = 4.0f;
	planets[8].rotationSpeed = 0.9f;
	planets[8].orbitSpeed = 164.81f;
	planets[8].position = V3_RIGHT * 80.0f;
	planets[8].color = { 0.21f, 0.028f, 0.79f };

	for (int i = 0; i < PLANET_COUNT; i++)
	{
		planetColors[i] = planets[i].color;
	}
}

void SkinningScene::OnUnload()
{
	DestroyFramebuffer(&fFbo);
	DestroyTexture2D(&fDepthRT);
	DestroyTexture2D(&fColorRT);
}

void SkinningScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(fFov, SCREEN_ASPECT, fNear, fFar);

	const float tt = TotalTime();
	const float rotSelfScale = 0.0004f * 1000.0f;
	const float rotOrbitZScale = 0.0001f * 1000.0f;

	for (int i = 0; i < PLANET_COUNT; i++)
	{
		Planet& planet = planets[i];
		Matrix scale = Scale(V3_ONE * planet.radius);
		Matrix rotationSelf = RotateY(tt / planet.rotationSpeed * 0.5f);
		Matrix rotationOrbit = RotateZ(tt / planet.orbitSpeed * rotOrbitZScale);
		rotationOrbit = planet.orbitSpeed > 0.0f ? rotationOrbit : MatrixIdentity();
		Matrix translation = Translate(planet.position);

		planetWorld[i] = scale * rotationSelf * translation * rotationOrbit;
		planetNormal[i] = NormalMatrix(planetWorld[i]);
		planetMvp[i] = planetWorld[i] * gView * gProj;
	}

}

void SkinningScene::OnDraw()
{
	BindFramebuffer(fFbo);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (fRaster)
		DrawPlanetsRaster();

	UnbindFramebuffer(fFbo);
	if (fDepth)
		DrawDepth(fFbo);
	else
		DrawColor(fFbo, 0);
}


void DrawPlanetsRaster()
{

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	SetPipelineState(gPipelineDefault);
	BindShader(&gShaderPlanetsRaster);
	SendMat4Array("u_mvp", planetMvp, PLANET_COUNT);
	SendMat4Array("u_world", planetWorld, PLANET_COUNT);
	SendMat3Array("u_normal", planetNormal, PLANET_COUNT);
	SendVec3Array("u_color", planetColors, PLANET_COUNT);
	SendVec3("u_sunPos", planets[0].position);
	DrawMeshInstanced(gMeshSphere, PLANET_COUNT);
	UnbindShader();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}