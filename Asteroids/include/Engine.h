#pragma once
#include "HelperData.h"

class GameObject;
class Player;
class Asteroid;
class Bullet;
class Particle;
class Saucer;

/*
* Simple 'Engine' Class used for distinguishing/splitting the 
* 'Backend' and 'Frontend' (Game Class) for this project.
* Responsible for non-gameplay related functionality.
*/
class Engine
{
public:
	Engine();
	~Engine();		

	// Register the window class and call methods for instantiating drawing resources
	HRESULT Initialize();

	// Draw content.
	HRESULT StartDraw();
	void DrawPlayer(Player* a_Player);
	void DrawSaucer(Saucer* a_Saucer);
	void DrawAsteroids(Asteroid* a_Asteroids[]);
	void DrawBullets(Bullet* a_Bullets[]);
	void DrawParticles(Particle* a_Particles[]);
	void DrawUI(int a_Score, int a_Lifes, Hdata::EGameState a_GameState);
	HRESULT EndDraw();

	//Getters
	const HWND& GetHWND() const;
	ID2D1SolidColorBrush* GetBrush(Hdata::EBrushColor a_Color = Hdata::EBrushColor::White) const;

private:

	//Draw UI
	void DrawScore(int a_Score);
	void DrawLifes(int a_Lifes);

	// Initialize device-independent resources.
	HRESULT CreateDeviceIndependentResources();

	// Initialize device-dependent resources.
	HRESULT CreateDeviceResources();

	// Release device-dependent resource.
	void DiscardDeviceResources();

	// Resize the render target.
	void OnResize(UINT a_NewWidth, UINT a_NewHeight);

	// Initialize all Brushes.
	HRESULT& InitBrushes(HRESULT& a_hr);

	// The windows procedure.
	static LRESULT CALLBACK WndProc(HWND a_Hwnd, UINT a_Message, WPARAM a_wParam, LPARAM a_lParam);

private:

	HWND m_Hwnd;

	ID2D1Factory* m_pDirect2dFactory;
	IDWriteFactory* m_pDWriteFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	IDWriteTextFormat* m_pTextFormat;

	//Brushes used by Game Objects/Text
	ID2D1SolidColorBrush* m_pWhiteBrush;
	ID2D1SolidColorBrush* m_pBlueBrush;
	ID2D1SolidColorBrush* m_pYellowBrush;
	ID2D1SolidColorBrush* m_pRedBrush;
	ID2D1SolidColorBrush* m_pGreenBrush;

};

