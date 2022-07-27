#include "Asteroid.h"

Asteroid::Asteroid(ID2D1SolidColorBrush* a_Color, EType a_Type) :
	DestructibleObject(a_Color),
	m_EType(a_Type)
{
	//Generate random position somewhere within the window
	m_Position.m_X = static_cast<float>(rand() % Hdata::ScreenWidth);
	m_Position.m_Y = static_cast<float>(rand() % Hdata::ScreenHeight);

	Init();
}

Asteroid::Asteroid(ID2D1SolidColorBrush* a_Color, Hdata::Vec2 a_Position, EType a_Type) :
	DestructibleObject(a_Color),
	m_EType(a_Type)
{
	m_Position = a_Position;

	Init();
}

void Asteroid::Update(float a_Dt)
{
	switch (m_EState)
	{
	case EObjectState::Active:
	{
		//Move
		m_Position.m_X += (m_Acceleration.m_X * m_Velocity * a_Dt);
		m_Position.m_Y -= (m_Acceleration.m_Y * m_Velocity * a_Dt);
		
		//Wrap if we reached the end of the window
		WrapScreen();

		//Update Vertices based on new Position
		for (int i = 0; i < Hdata::AsteroidVertices; i++)
		{
			float Angle = (static_cast<float>(i) / static_cast<float>(Hdata::AsteroidVertices)) * 2 * Hdata::PI;
			float VertexX = m_Position.m_X + ((m_Size / 2.0f )* sinf(Angle)) + m_VertexWeight.at(i).x;
			float VertexY = m_Position.m_Y + ((m_Size / 2.0f) * cosf(Angle)) + m_VertexWeight.at(i).y;
			m_Vertices[i] = { VertexX, VertexY };
		}
		break;
	}

	case EObjectState::Animating:
	{
		m_ElapsedExplodeTime += a_Dt;
		if (m_ElapsedExplodeTime >= m_ExplodeCooldown)
		{
			m_ElapsedExplodeTime = 0.f;
			m_EState = EObjectState::ToDestroy;
		}
		break;
	}

	default:
		break;
	}
}

void Asteroid::Init()
{
	//Global values
	m_Vertices.reserve(Hdata::AsteroidVertices);
	m_VertexWeight.reserve(Hdata::AsteroidVertices);

	m_ExplodeCooldown = 1.0f;
	m_ParticleAmount = 10;

	m_MaxAcceleration = 30.f;
	m_Acceleration = { Hdata::RandomNumber(-m_MaxAcceleration, m_MaxAcceleration), Hdata::RandomNumber(-m_MaxAcceleration, m_MaxAcceleration) };
	
	//Type-Specific values
	switch (m_EType)
	{
	case EType::Large:
	{
		m_Size = 80.f;
		m_Velocity = 3.f;
		break;
	}

	case EType::Medium:
	{
		m_Size = 40.f;
		m_Velocity = 4.f;
		break;
	}

	case EType::Small:
	{
		m_Size = 20.f;
		m_Velocity = 5.f;
		break;
	}
	default:
		break;
	}

	//Generates random weights for each Vertex resulting a randomized Asteroid shape
	for (int i = 0; i < Hdata::AsteroidVertices; i++)
	{
		float WeightX = Hdata::RandomNumber((-m_Size / 7.f), (m_Size / 7.f));
		float WeightY = Hdata::RandomNumber((-m_Size / 7.f), (m_Size / 7.f));
		m_VertexWeight.push_back({ WeightX, WeightY });
	}

	//Generates a circle based on size and amount of vertices
	for (int i = 0; i < Hdata::AsteroidVertices; i++)
	{
		float Angle = (static_cast<float>(i) / static_cast<float>(Hdata::AsteroidVertices)) * 2 * Hdata::PI;
		float VertexX = m_Position.m_X + ((m_Size / 2.0f) * sinf(Angle)) + m_VertexWeight.at(i).x;
		float VertexY = m_Position.m_Y + ((m_Size / 2.0f) * cosf(Angle)) + m_VertexWeight.at(i).y;
		m_Vertices.push_back({ VertexX, VertexY });
	}
}

void Asteroid::SplitAsteroid(Asteroid* a_Asteroids[], int& a_Score)
{
	//Split to a lower level Asteroid
	switch (m_EType)
	{
	case EType::Large:
	{
		//Increase Score
		a_Score += 20;

		//Create 2 Medium Asteroids
		int Spawned = 0;
		for (int i = 0; i < Hdata::MaxAsteroids; i++)
		{
			if (!a_Asteroids[i])
			{
				Spawned++;
				a_Asteroids[i] = new Asteroid(m_pColor, m_Position, EType::Medium);
			}
			if (Spawned == 2)
			{
				//Mark Asteroid for destruction
				m_EState = EObjectState::Hit;
				break;
			}
		}
		break;
	}

	case EType::Medium:
	{
		//Increase Score
		a_Score += 50;

		//Create 2 Small Asteroids
		int Spawned = 0;
		for (int i = 0; i < Hdata::MaxAsteroids; i++)
		{
			if (!a_Asteroids[i])
			{
				Spawned++;
				a_Asteroids[i] = new Asteroid(m_pColor, m_Position, EType::Small);
			}
			if (Spawned == 2)
			{
				//Mark Asteroid for destruction
				m_EState = EObjectState::Hit;
				break;
			}
		}
		break;
	}

	case EType::Small:
	{
		//Increase Score
		a_Score += 100;

		//Cannon split anymore, destroy
		m_EState = EObjectState::Hit;
		break;
	}
	default:
		break;
	}
}
