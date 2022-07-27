#include "Saucer.h"
#include "Bullet.h"

Saucer::Saucer(ID2D1SolidColorBrush* a_Color, EType a_Type) :
	DestructibleObject(a_Color),
	m_RedirectCooldown(5.f),
	m_EType(a_Type)
{
	Init();
}

void Saucer::Update(float a_Dt)
{
	switch (m_EState)
	{
	case GameObject::EObjectState::Active:
	{
		//Update Timers
		m_ElapsedShootTime += a_Dt;
		m_ElapsedRedirectTime += a_Dt;

		//Change Direction from time to time
		if (m_ElapsedRedirectTime >= m_RedirectCooldown)
		{
			m_ElapsedRedirectTime = 0.f;

			//Random Direction
			m_Acceleration = { Hdata::RandomNumber(-1.f, 1.f), Hdata::RandomNumber(-1.f, 1.f) };
		}

		//Move
		m_Position.m_X += m_Acceleration.m_X * m_Velocity * a_Dt;
		m_Position.m_Y += m_Acceleration.m_Y * m_Velocity * a_Dt;

		WrapScreen();

		//Adjust Ship Vertices based on the new position and rotation - Shape is UFO
		//1, 11
		float Ax = (m_Size / 2.0f) * sin((m_Rotation - 45) * Hdata::PI / 180.f);
		float Ay = (m_Size / 2.0f) * cos((m_Rotation - 45) * Hdata::PI / 180.f);

		//2
		float Bx = (m_Size / 1.5f) * sin((m_Rotation - 20) * Hdata::PI / 180.f);
		float By = (m_Size / 1.5f) * cos((m_Rotation - 20) * Hdata::PI / 180.f);

		//3
		float Cx = (m_Size / 1.5f) * sin((m_Rotation + 20) * Hdata::PI / 180.f);
		float Cy = (m_Size / 1.5f) * cos((m_Rotation + 20) * Hdata::PI / 180.f);

		//0, 4
		float Dx = (m_Size / 2.0f) * sin((m_Rotation + 45) * Hdata::PI / 180.f);
		float Dy = (m_Size / 2.0f) * cos((m_Rotation + 45) * Hdata::PI / 180.f);

		//5, 9
		float Ex = (m_Size) * sin((m_Rotation + 90) * Hdata::PI / 180.f);
		float Ey = (m_Size) * cos((m_Rotation + 90) * Hdata::PI / 180.f);

		//6
		float Fx = (m_Size / 2.f) * sin((m_Rotation + 120) * Hdata::PI / 180.f);
		float Fy = (m_Size / 2.f) * cos((m_Rotation + 120) * Hdata::PI / 180.f);

		//7
		float Gx = (m_Size / 2.f) * sin((m_Rotation - 120) * Hdata::PI / 180.f);
		float Gy = (m_Size / 2.f) * cos((m_Rotation - 120) * Hdata::PI / 180.f);

		//8, 10
		float Hx = (m_Size) * sin((m_Rotation - 90) * Hdata::PI / 180.f);
		float Hy = (m_Size) * cos((m_Rotation - 90) * Hdata::PI / 180.f);


		m_Vertices[0] = { m_Position.m_X + Dx, m_Position.m_Y - Dy };				//Top Vertex of Isosceles Triangle (A)
		m_Vertices[1] = { m_Position.m_X + Ax, m_Position.m_Y - Ay };				//Left Vertex (B)
		m_Vertices[2] = { m_Position.m_X + Bx, m_Position.m_Y - By };				//Creates the arrow-like look (Left Side)
		m_Vertices[3] = { m_Position.m_X + Cx, m_Position.m_Y - Cy };				//Creates the arrow-like look (Right Side)
		m_Vertices[4] = { m_Position.m_X + Dx, m_Position.m_Y - Dy };				//Right Vertex (E)
		m_Vertices[5] = { m_Position.m_X + Ex, m_Position.m_Y - Ey };
		m_Vertices[6] = { m_Position.m_X + Fx, m_Position.m_Y - Fy };
		m_Vertices[7] = { m_Position.m_X + Gx, m_Position.m_Y - Gy };
		m_Vertices[8] = { m_Position.m_X + Hx, m_Position.m_Y - Hy };
		m_Vertices[9] = { m_Position.m_X + Ex, m_Position.m_Y - Ey };
		m_Vertices[10] = { m_Position.m_X + Hx, m_Position.m_Y - Hy };
		m_Vertices[11] = { m_Position.m_X + Ax, m_Position.m_Y - Ay };

		break;
	}

	case GameObject::EObjectState::Animating:
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

const Saucer::EType Saucer::GetType() const
{
	return m_EType;
}

void Saucer::Init()
{
	
	m_Vertices.reserve(12);			//Reserve space for the shape of the saucer
	
	m_ElapsedShootTime = 0.f;
	m_ShotInaccuracy = 60.f;
	m_ExplodeCooldown = 1.f;
	m_ParticleAmount = 10;
	m_EState = GameObject::EObjectState::Active;

	//Push dummy values, will get overwritten by Update()
	for (int i = 0; i < 12; i++)
	{
		m_Vertices.push_back({ m_Position.m_X, m_Position.m_Y });
	}

	//Init Type-Specific Values
	switch (m_EType)
	{
	case Saucer::EType::Large:
	{
		m_Size = 25.f;
		m_ShootCooldown = 1.0f;
		m_Velocity = Hdata::RandomNumber(60.f, 100.f);

		float Randomizer = Hdata::RandomNumber(0.f, 1.f);
		if (Randomizer < 0.5f)
		{
			//Spawn on the left side of the window
			m_Position = { 0.f, Hdata::RandomNumber(0.f , static_cast<float>(Hdata::ScreenHeight)) };

			//Random Direction to the right
			m_Acceleration = { 1.f, Hdata::RandomNumber(-1.f, 1.f) };

		}
		else
		{
			//Spawn on the right side of the window
			m_Position = { static_cast<float>(Hdata::ScreenWidth), Hdata::RandomNumber(0.f ,  static_cast<float>(Hdata::ScreenHeight)) };

			//Random Direction to the left
			m_Acceleration = { -1.f , Hdata::RandomNumber(-1.f, 1.f) };
		}

		printf("[LOG] Large Saucer Created \n");
		break;
	}	

	case Saucer::EType::Small:
	{
		m_Size = 15.f;
		m_ShootCooldown = 0.5;
		m_Velocity = Hdata::RandomNumber(150.f, 250.f);

		float Randomizer = Hdata::RandomNumber(0.f, 1.f);
		if (Randomizer < 0.5f)
		{
			//Spawn on the left side of the window
			m_Position = { 0.f, Hdata::RandomNumber(0.f , static_cast<float>(Hdata::ScreenHeight)) };

			//Random Direction to the right
			m_Acceleration = { 1.f, Hdata::RandomNumber(-1.f, 1.f) };

		}
		else
		{
			//Spawn on the right side of the window
			m_Position = { static_cast<float>(Hdata::ScreenWidth), Hdata::RandomNumber(0.f ,  static_cast<float>(Hdata::ScreenHeight)) };

			//Random Direction to the left
			m_Acceleration = { -1.f , Hdata::RandomNumber(-1.f, 1.f) };
		}

		printf("[LOG] Small Saucer Created \n");
		break;
	}

	default:
		break;
	}

}

void Saucer::Shoot(Bullet* a_Bullets[], Hdata::Vec2 a_target)
{
	
	if (m_ElapsedShootTime >= m_ShootCooldown)
	{
		m_ElapsedShootTime = 0.f;
		

		for (int i = 0; i < Hdata::MaxBullets; i++)
		{
			if (!a_Bullets[i])
			{
				if (m_EType == EType::Large)
				{
					//Shoot Randomly
					float Angle = Hdata::RandomNumber(0.f, 360.);
					a_Bullets[i] = new Bullet(m_pColor, Angle + 90, m_Position, Bullet::EType::Saucer);
				}
				else
				{
					//Shoot Accurately (with some in Inaccuracy which gets reduced by each bullet)
					float Angle = atan2(a_target.m_Y - m_Position.m_Y, a_target.m_X - m_Position.m_X) * 180 / Hdata::PI;
					Angle = Hdata::RandomNumber(Angle - m_ShotInaccuracy, Angle + m_ShotInaccuracy);
					a_Bullets[i] = new Bullet(m_pColor, Angle + 90, m_Position, Bullet::EType::Saucer);

					//Reduce Inaccuracy per bullet (machine learning at its finest)
					if (m_ShotInaccuracy > 0.f)
					{
						m_ShotInaccuracy--;
					}					
				}
				
				break;
			}
		}
	}

}
