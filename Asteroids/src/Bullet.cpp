#include "Bullet.h"

Bullet::Bullet(ID2D1SolidColorBrush* a_Color, float a_Rotation, Hdata::Vec2 a_Position, EType a_Type) :
	Particle(a_Color, a_Position, true),
	m_EType(a_Type)
{ 
	m_Rotation = a_Rotation;
	Init();
}

void Bullet::Update(float a_Dt)
{
	switch (m_EState)
	{
	case EObjectState::Active:
	{
		//Increase Time being Active
		m_ElapsedTime += a_Dt;
		if (m_ElapsedTime >= m_LifeTime)
		{
			m_EState = EObjectState::ToDestroy;
			break;
		}

		//Move Bullet
		m_Position.m_X += m_Velocity * a_Dt * sin(m_Rotation * Hdata::PI / 180);
		m_Position.m_Y -= m_Velocity * a_Dt * cos(m_Rotation * Hdata::PI / 180);

		//Wrap if we reached the end of the window
		WrapScreen();

		//Update Vertices based on new Position
		for (int i = 0; i < Hdata::BulletVertices; i++)
		{
			float Angle = (static_cast<float>(i) / static_cast<float>(Hdata::BulletVertices)) * 2 * Hdata::PI;
			m_Vertices[i] = { m_Position.m_X + ((m_Size / 2.0f) * sinf(Angle)), m_Position.m_Y + ((m_Size / 2.0f) * cosf(Angle)) };
		}
		break;
	}
	default:
		break;
	}
}

const Bullet::EType& Bullet::GetType() const
{
	return m_EType;
}

void Bullet::Init()
{
	//Bullet Specific Values	
	m_LifeTime = 1.5f;
	m_Velocity = 600.f;

	m_Vertices.reserve(Hdata::BulletVertices);

	//Generates a circle of vertices around the Particle Position
	for (int i = 0; i < Hdata::BulletVertices; i++)
	{
		float Angle = (static_cast<float>(i) / static_cast<float>(Hdata::BulletVertices)) * 2 * Hdata::PI;
		m_Vertices.push_back({ m_Position.m_X + ((m_Size / 2.0f) * sinf(Angle)), m_Position.m_Y + ((m_Size / 2.0f) * cosf(Angle)) });
	}
}
