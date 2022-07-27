#pragma once
#include "HelperData.h"

class Asteroid;
class Bullet;
class Player;
class Particle;
class Saucer;
class Engine;

class Game
{
public:

	Game(const Engine* a_Engine);
	~Game();

	//Functions
	void Update(float a_Dt);

	//Getters
	const int GetCurrentWave() const;
	const int GetScore() const;
	const int GetRemainingLifes() const;
	const Hdata::EGameState& GetGameState() const;

	//Setters
	void SetCurrentWave(int a_NewWave);

	//Game Objects
	Player* m_pPlayer;
	Saucer* m_pSaucer;
	Asteroid* m_pAsteroids[Hdata::MaxAsteroids];
	Bullet* m_pBullets[Hdata::MaxBullets];
	Particle* m_pParticles[Hdata::MaxParticles];

private:
	void Init();
	void RestartGame();

	void NextWave();
	void RepeatWave();

	void UpdatePlayer(float a_Dt);
	void UpdateSaucer(float a_Dt);
	void UpdateAsteroids(float a_Dt);
	void UpdateBullets(float a_Dt);
	void UpdateParticles(float a_Dt);

	void CheckInput(float a_Dt);
	void CheckCollisions();

	void ResetGameObjects();
	bool DidWaveEnd();

private:
	int m_CurrentWave;
	int m_RemainingLifes;
	int m_Score;
	int m_ScoreMilestone;
	const int m_MaxScore;
	
	float m_ElapsedInputTimer;
	float m_ElapsedSaucerTimer;
	const float m_InputCooldown;
	const float m_SaucerCooldown;

	Hdata::EGameState m_State;

	const Engine* m_pEngine;
};

