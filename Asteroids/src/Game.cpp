#include "Game.h"
#include "Asteroid.h"
#include "Player.h"
#include "Particle.h"
#include "Saucer.h"
#include "Engine.h"

typedef GameObject::EObjectState EState;
typedef Hdata::EGameState EGameState;

Game::Game(const Engine* a_Engine) :
	m_pEngine(a_Engine),
	m_InputCooldown(0.5f),
	m_MaxScore(99990),				//Same to original game
	m_SaucerCooldown(30.f)
{	
	Init();	
}

Game::~Game()
{
	//Delete Player
	delete m_pPlayer;
	printf("[LOG] Destroyed Player \n");

	//Delete Saucer
	delete m_pSaucer;
	printf("[LOG] Destroyed Saucer \n");

	//Delete Asteroids
	for (int i = 0; i < Hdata::MaxAsteroids; i++)
	{
		delete m_pAsteroids[i];
	}
	printf("[LOG] Destroyed Asteroids \n");

	//Delete Bullets
	for (int i = 0; i < Hdata::MaxBullets; i++)
	{
		delete m_pBullets[i];
	}
	printf("[LOG] Destroyed Bullets \n");

	//Delete Particles
	for (int i = 0; i < Hdata::MaxParticles; i++)
	{
		delete m_pParticles[i];
	}
	printf("[LOG] Destroyed Particles \n");
}

void Game::Update(float a_Dt)
{
	//Prevents from spamming an Input (Pause/Unpause)
	//Timer Resets in case it approaches FLT_MAX (Prevents overflow)
	m_ElapsedInputTimer >= (FLT_MAX - 100.f) ? m_ElapsedInputTimer = 10.f : m_ElapsedInputTimer += a_Dt;

	//Process User Input
	CheckInput(a_Dt);

	switch (m_State)
	{
	case EGameState::Running:
	{
		//End condition
		if (m_Score >= m_MaxScore)
		{
			printf("[LOG] You Win! \n");
			m_State = EGameState::Won;
		}

		//Gain life on each Milestone
		if (m_Score >= m_ScoreMilestone)
		{
			m_RemainingLifes++;
			m_ScoreMilestone += 10000;
		}

		UpdatePlayer(a_Dt);
		UpdateSaucer(a_Dt);
		UpdateAsteroids(a_Dt);
		UpdateBullets(a_Dt);
		UpdateParticles(a_Dt);
		CheckCollisions();

		break;
	}

	default:
		break;
	}

}


const int Game::GetCurrentWave() const
{
	return m_CurrentWave;
}

const int Game::GetScore() const
{
	return m_Score;
}

const int Game::GetRemainingLifes() const
{
	return m_RemainingLifes;
}

const Hdata::EGameState& Game::GetGameState() const
{
	return m_State;
}

void Game::SetCurrentWave(int a_NewWave)
{
	if ((a_NewWave > 0) && (a_NewWave <= Hdata::MaxWaves))
	{
		m_CurrentWave = a_NewWave;
		printf("[LOG] Wave %i \n", m_CurrentWave);
	}
	else if (a_NewWave >= Hdata::MaxWaves + 1)
	{
		printf("[LOG] You Win! \n");
		m_State = EGameState::Won;
	}
	else
	{
		printf("[ERROR] Tried to Set Wave to the invalid value %i \n", a_NewWave);
	}
}

void Game::Init()
{
	printf("[LOG] Initializing Game \n");
	SetCurrentWave(1);

	m_RemainingLifes = Hdata::StartingLifes;
	m_ElapsedInputTimer = 0.f;
	m_ElapsedSaucerTimer = 0.f;
	m_Score = 0;
	m_ScoreMilestone = 10000;
	m_State = EGameState::Running;

	//Init Player
	m_pPlayer = new Player(m_pEngine->GetBrush());
	
	//Init Asteroids for current wave
	for (int i = 0; i < Hdata::AsteroidsPerWave * m_CurrentWave; i++)
	{
		m_pAsteroids[i] = new Asteroid(m_pEngine->GetBrush());	
	}
	
	printf("[LOG] Game Initialized \n");
}

void Game::RestartGame()
{
	ResetGameObjects();
	Init();
}

bool Game::DidWaveEnd()
{
	for (int i = 0; i < Hdata::MaxAsteroids; i++)
	{
		if (m_pAsteroids[i])
		{
			return false;
		}
	}
	return true;
}

void Game::NextWave()
{
	SetCurrentWave(m_CurrentWave + 1);
	if ((m_State == EGameState::Running) || (m_State == EGameState::Paused))
	{

		//Make Player Invurnerable
		m_pPlayer->SetState(GameObject::EObjectState::Invulnerable);

		//Reset Asteroids
		for (int i = 0; i < Hdata::MaxAsteroids; i++)
		{
			if (m_pAsteroids[i])
			{
				delete m_pAsteroids[i];
				m_pAsteroids[i] = nullptr;
			}
		}

		//Reset Bullets
		for (int i = 0; i < Hdata::MaxBullets; i++)
		{
			if (m_pBullets[i])
			{
				delete m_pBullets[i];
				m_pBullets[i] = nullptr;
			}
		}

		//Init Asteroids for current wave
		for (int i = 0; i < Hdata::AsteroidsPerWave * m_CurrentWave; i++)
		{
			m_pAsteroids[i] = new Asteroid(m_pEngine->GetBrush());
		}
	}
}

void Game::RepeatWave()
{
	m_RemainingLifes--;
	if (m_RemainingLifes == 0)
	{
		printf("[LOG] Game Over! \n");
		m_State = EGameState::Lost;		
	}
	else
	{
		//Reset Player
		delete m_pPlayer;
		m_pPlayer = new Player(m_pEngine->GetBrush());

		//Reset Bullets
		for (int i = 0; i < Hdata::MaxBullets; i++)
		{
			if (m_pBullets[i])
			{
				delete m_pBullets[i];
				m_pBullets[i] = nullptr;
			}
		}
	}
}

void Game::UpdatePlayer(float a_Dt)
{
	//Does Player Exist?
	if (m_pPlayer)
	{
		//Update Player
		m_pPlayer->Update(a_Dt);

		//Switch States
		if (m_pPlayer->GetState() == EState::Hit)
		{
			m_pPlayer->Explode(m_pParticles, Particle::EType::Line);
			m_pPlayer->SetState(EState::Animating);
		}
		else if (m_pPlayer->GetState() == EState::ToDestroy)
		{
			RepeatWave();
		}
	}
}

void Game::UpdateSaucer(float a_Dt)
{
	//Does Player Exist?
	if (m_pSaucer)
	{
		//Update Player
		m_pSaucer->Update(a_Dt);

		//Switch States
		switch (m_pSaucer->GetState())
		{
		case EState::Active:
		{
			//Make sure there is a valid Player to Shoot at
			if (m_pPlayer)
			{
				m_pSaucer->Shoot(m_pBullets, m_pPlayer->GetPosition());
				break;
			}		
		}

		case EState::Hit:
		{
			//Increase Score
			if (m_pSaucer->GetType() == Saucer::EType::Large)
				m_Score += 200;
			else
				m_Score += 1000;

			m_pSaucer->Explode(m_pParticles, Particle::EType::Line);
			m_pSaucer->SetState(EState::Animating);
			break;
		}

		case EState::ToDestroy:
		{
			m_pSaucer = nullptr;
			break;
		}

		default:
			break;
		}
	}
	//if not, Update timer and Spawn one
	else
	{
		m_ElapsedSaucerTimer += a_Dt;
		if (m_ElapsedSaucerTimer >= m_SaucerCooldown)
		{
			m_ElapsedSaucerTimer = 0.f;

			//Init Large Saucer if the score is below 10k (Same as original game)
			if (m_Score < 10000)
			{
				m_pSaucer = new Saucer(m_pEngine->GetBrush());
			}
			//Init Small Saucer
			else
			{			
				m_pSaucer = new Saucer(m_pEngine->GetBrush(), Saucer::EType::Small);
			}
			
		}
	}
}

void Game::UpdateAsteroids(float a_Dt)
{
	//Go over all Asteroids
	for (int i = 0; i < Hdata::MaxAsteroids; i++)
	{
		//Does Asteroid Exist?
		if (m_pAsteroids[i])
		{
			//Update Asteroid
			m_pAsteroids[i]->Update(a_Dt);

			//Switch States
			if (m_pAsteroids[i]->GetState() == EState::Hit)
			{
				m_pAsteroids[i]->Explode(m_pParticles);
				m_pAsteroids[i]->SetState(EState::Animating);
			}
			else if (m_pAsteroids[i]->GetState() == EState::ToDestroy)
			{
				m_pAsteroids[i] = nullptr;

				//Check if there are any other remaining Asteroids
				if (DidWaveEnd())
				{
					NextWave();
				}
			}
		}
	}
}

void Game::UpdateBullets(float a_Dt)
{
	//Go over all Bullets
	for (int i = 0; i < Hdata::MaxBullets; i++)
	{
		//Does Bullet Exist?
		if (m_pBullets[i])
		{
			//Update Bullet
			m_pBullets[i]->Update(a_Dt);

			//Switch States
			if (m_pBullets[i]->GetState() == EState::ToDestroy)
			{
				m_pBullets[i] = nullptr;
			}
		}
	}
}

void Game::UpdateParticles(float a_Dt)
{
	//Go over all Particles
	for (int i = 0; i < Hdata::MaxParticles; i++)
	{
		//Does Particle Exist?
		if (m_pParticles[i])
		{
			//Update Particle
			m_pParticles[i]->Update(a_Dt);

			//Switch States
			if (m_pParticles[i]->GetState() == EState::ToDestroy)
			{
				m_pParticles[i] = nullptr;
			}
		}
	}
}

void Game::CheckInput(float a_Dt)
{
	switch (m_State)
	{
	case EGameState::Running:
	{

		//Was Esc Pressed? - Pause Game
		if ((GetKeyState(VK_ESCAPE) & 0x8000) && (m_ElapsedInputTimer >= m_InputCooldown))
		{
			printf("[LOG] Game Paused \n");
			m_ElapsedInputTimer = 0.f;
			m_State = EGameState::Paused;
		}

		//Does Player Exist?
		if (m_pPlayer)
		{
			//Player Actions

			//W Key - Thrust
			if (GetKeyState('W') & 0x8000)
				m_pPlayer->Move(a_Dt);
			else
				m_pPlayer->SetThrustOn(false);

			//A Key - Rotate Left
			if (GetKeyState('A') & 0x8000)
				m_pPlayer->Rotate(a_Dt, false);

			//D Key - Rotate Right
			if (GetKeyState('D') & 0x8000)
				m_pPlayer->Rotate(a_Dt, true);

			//M Key - Teleport
			if (GetKeyState('M') & 0x8000)
				m_pPlayer->Teleport();

			//Space Key - Shoot
			if (GetKeyState(VK_SPACE) & 0x8000)
				m_pPlayer->Shoot(m_pBullets);
		}
		break;
	}

	case EGameState::Paused:
	{
		//Was Esc Pressed? - Resume Game
		if ((GetKeyState(VK_ESCAPE) & 0x8000) && (m_ElapsedInputTimer >= m_InputCooldown))
		{
			printf("[LOG] Game Resumed \n");
			m_ElapsedInputTimer = 0.f;
			m_State = EGameState::Running;
		}

		//Was Space Pressed? - Restart Game
		if ((GetKeyState(VK_SPACE) & 0x8000) && (m_ElapsedInputTimer >= m_InputCooldown))
		{
			printf("[LOG] Restarting Game \n");
			RestartGame();
		}
		break;
	}

	case EGameState::Won:
	{
		//Was Esc Pressed? - Exit Game
		if ((GetKeyState(VK_ESCAPE) & 0x8000) && (m_ElapsedInputTimer >= m_InputCooldown))
		{
			printf("[LOG] Exiting Game \n");
			m_State = EGameState::Quit;
		}

		//Was Space Pressed? - Restart Game
		if ((GetKeyState(VK_SPACE) & 0x8000) && (m_ElapsedInputTimer >= m_InputCooldown))
		{
			printf("[LOG] Restarting Game \n");
			RestartGame();
		}			
		break;
	}

	case EGameState::Lost:
	{
		//Was Esc Pressed? - Exit Game
		if ((GetKeyState(VK_ESCAPE) & 0x8000) && (m_ElapsedInputTimer >= m_InputCooldown))
		{
			printf("[LOG] Exiting Game \n");
			m_State = EGameState::Quit;
		}

		//Was Space Pressed? - Restart Game
		if ((GetKeyState(VK_SPACE) & 0x8000) && (m_ElapsedInputTimer >= m_InputCooldown))
		{
			printf("[LOG] Restarting Game \n");
			RestartGame();
		}
		break;
	}

	default:
		break;
	}
}

void Game::CheckCollisions()
{

	//Check if an Asteroid collides with Player or a Bullet
	for (int aIndex = 0; aIndex < Hdata::MaxAsteroids; aIndex++)								//Asteroid Index
	{
		//Does Asteroid Exist and is it Active
		if ((m_pAsteroids[aIndex]) && (m_pAsteroids[aIndex]->GetState() == EState::Active))
		{
			const float aX = m_pAsteroids[aIndex]->GetPosition().m_X;
			const float aY = m_pAsteroids[aIndex]->GetPosition().m_Y;

			//Check against Player
			//Does Player Exist and is it Active
			if ((m_pPlayer) && (m_pPlayer->GetState() == EState::Active))
			{
				const float pX = m_pPlayer->GetPosition().m_X;
				const float pY = m_pPlayer->GetPosition().m_Y;

				//Check if Asteroid Radius overlaps Player Position
				float DistanceAP = static_cast<float>(sqrt(
					pow((pY - aY), 2) +
					pow((aX - pX), 2)));

				//If it does overlap, Player is Hit
				if ((m_pAsteroids[aIndex]->GetSize()/2.0f) >= DistanceAP)
				{
					m_pPlayer->SetState(EState::Hit);
					continue;
				}
			}

			//Check against all Bullets
			for (int bIndex = 0; bIndex < Hdata::MaxBullets; bIndex++)							//Bullet Index
			{
				//Does Bullet Exist, is it Active and does it belong to the Player
				if ((m_pBullets[bIndex]) && (m_pBullets[bIndex]->GetState() == EState::Active) && (m_pBullets[bIndex]->GetType() == Bullet::EType::Player))
				{
					const float bX = m_pBullets[bIndex]->GetPosition().m_X;
					const float bY = m_pBullets[bIndex]->GetPosition().m_Y;

					//Check if Asteroid Radius overlaps a Bullet Position
					float DistanceAB = static_cast<float>(sqrt(
						pow((bY - aY), 2) +
						pow((aX - bX), 2)));

					//If it does overlap, Split Asteroid and Destroy Bullet
					if ((m_pAsteroids[aIndex]->GetSize()/2.f) >= DistanceAB)
					{
						//m_pAsteroids[aIndex]->SetState(EObjectState::Hit);
						m_pAsteroids[aIndex]->SplitAsteroid(m_pAsteroids, m_Score);
						m_pBullets[bIndex]->SetState(EState::ToDestroy);
						continue;
					}
				}
			}
		}
	}


	//Check if a Bullet collides with Saucer or Player
	for (int bIndex = 0; bIndex < Hdata::MaxBullets; bIndex++)									//Bullet Index
	{
		//Does Bullet Exist and is it Active
		if ((m_pBullets[bIndex]) && (m_pBullets[bIndex]->GetState() == EState::Active))
		{
			const float bX = m_pBullets[bIndex]->GetPosition().m_X;
			const float bY = m_pBullets[bIndex]->GetPosition().m_Y;

			//Does it overlap a Saucer? - Does Saucer exist, is it active and is the Bullet not its own?
			if ((m_pSaucer) && (m_pSaucer->GetState() == EState::Active) && !(m_pBullets[bIndex]->GetType() == Bullet::EType::Saucer))
			{
				const float sX = m_pSaucer->GetPosition().m_X;
				const float sY = m_pSaucer->GetPosition().m_Y;

				//Check if Saucer Radius overlaps a Bullet Position
				float DistanceSB = static_cast<float>(sqrt(
					pow((bY - sY), 2) +
					pow((sX - bX), 2)));

				//If it does overlap, change states
				if ((m_pSaucer->GetSize() / 2.f) >= DistanceSB)
				{
					m_pSaucer->SetState(EState::Hit);
					m_pBullets[bIndex]->SetState(EState::ToDestroy);
					continue;
				}
			}

			//Does it overlap a Player? - Does Player exist, is it active and is the Bullet not its own?
			else if ((m_pPlayer) && (m_pPlayer->GetState() == EState::Active) && !(m_pBullets[bIndex]->GetType() == Bullet::EType::Player))
			{
				const float pX = m_pPlayer->GetPosition().m_X;
				const float pY = m_pPlayer->GetPosition().m_Y;

				//Check if Player Radius overlaps a Bullet Position
				float DistancePB = static_cast<float>(sqrt(
					pow((bY - pY), 2) +
					pow((pX - bX), 2)));

				//If it does overlap, change states
				if ((m_pPlayer->GetSize() / 2.f) >= DistancePB)
				{
					m_pPlayer->SetState(EState::Hit);
					m_pBullets[bIndex]->SetState(EState::ToDestroy);
					continue;
				}
			}			
		}
	}

	//Check Player vs Saucer
	if ((m_pPlayer) && (m_pPlayer->GetState() == EState::Active) && (m_pSaucer) && (m_pSaucer->GetState() == EState::Active))
	{
		const float pX = m_pPlayer->GetPosition().m_X;
		const float pY = m_pPlayer->GetPosition().m_Y;
		const float sX = m_pSaucer->GetPosition().m_X;
		const float sY = m_pSaucer->GetPosition().m_Y;

		//Check if Player Radius overlaps a Bullet Position
		float DistancePS = static_cast<float>(sqrt(
			pow((sY - pY), 2) +
			pow((pX - sX), 2)));

		//If it does overlap, change states
		if ((m_pPlayer->GetSize() / 2.f) >= DistancePS)
		{
			m_pPlayer->SetState(EState::Hit);
			m_pSaucer->SetState(EState::Hit);
		}
	}
}

void Game::ResetGameObjects()
{
	//Delete Player
	m_pPlayer = nullptr;

	//Delete Saucer
	m_pSaucer = nullptr;

	//Delete Asteroids
	for (int i = 0; i < Hdata::MaxAsteroids; i++)
	{
		m_pAsteroids[i] = nullptr;
	}

	//Delete Bullets
	for (int i = 0; i < Hdata::MaxBullets; i++)
	{
		m_pBullets[i] = nullptr;
	}

	//Delete Particles
	for (int i = 0; i < Hdata::MaxParticles; i++)
	{
		m_pParticles[i] = nullptr;
	}
	printf("[LOG] Reset all Game Objects \n");
}
