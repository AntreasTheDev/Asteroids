#include "Particle.h"

Particle::Particle(ID2D1SolidColorBrush* a_Color, Hdata::Vec2 a_Position, bool a_IsBullet, EType a_Type,
					float a_Velocity, float a_LifeTime, float a_Size, int a_MaxVertices) :
	GameObject(a_Color),
	m_EType(a_Type),
	m_MaxVertices(a_MaxVertices)
{
	m_Position = a_Position;
	m_Size = a_Size;
	m_LifeTime = a_LifeTime;
	m_Velocity = a_Velocity;
	m_ElapsedTime = 0.f;

	if (!a_IsBullet)
	{		
		Init();
	}
}

void Particle::Update(float a_Dt)
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

		//Move Particle
		m_Position.m_X += m_Velocity * a_Dt * sin(m_Rotation * Hdata::PI / 180);
		m_Position.m_Y -= m_Velocity * a_Dt * cos(m_Rotation * Hdata::PI / 180);

		//Destroy it goes out of bounds
		if ((m_Position.m_X <= 0) || (m_Position.m_X >= Hdata::ScreenWidth))
		{
			m_EState = EObjectState::ToDestroy;
		}
		if ((m_Position.m_Y <= 0) || (m_Position.m_Y >= Hdata::ScreenWidth))
		{
			m_EState = EObjectState::ToDestroy;
		}

		if (m_EType == EType::Circle)
		{
			//Same as Init()
			for (int i = 0; i < m_MaxVertices; i++)
			{
				float Angle = (static_cast<float>(i) / static_cast<float>(m_MaxVertices)) * 2 * Hdata::PI;
				m_Vertices[i] = { m_Position.m_X + ((m_Size / 2.0f) * sinf(Angle)), m_Position.m_Y + ((m_Size / 2.0f) * cosf(Angle)) };
			}
		}
		else
		{
			//Same as Init()
			float VertexAx = (m_Size / 2.0f) * sin(m_Rotation * Hdata::PI / 180.f);
			float VertexAy = (m_Size / 2.0f) * cos(m_Rotation * Hdata::PI / 180.f);
			m_Vertices[0] = { m_Position.m_X + VertexAx, m_Position.m_Y + VertexAy };
			m_Vertices[1] = { m_Position.m_X - VertexAx, m_Position.m_Y - VertexAy };
		
		}
		break;

	}
	default:
		break;
	}
}

void Particle::Init()
{

	m_Rotation = static_cast<float>(rand() % 360);

	//Generate Particle Shape based on EType
	switch (m_EType)
	{
	case EType::Circle:
	{
		m_Vertices.reserve(m_MaxVertices);
		
		//Generates a circle of vertices around the Particle Position
		for (int i = 0; i < m_MaxVertices; i++)
		{
			float Angle = (static_cast<float>(i) / static_cast<float>(m_MaxVertices)) * 2 * Hdata::PI;
			m_Vertices.push_back({ m_Position.m_X + (m_Size * sinf(Angle)), m_Position.m_Y + (m_Size * cosf(Angle)) });
		}
		break;
	}

	case EType::Line:
	{
		//Line consists of 2 points/vertices
		m_Vertices.reserve(2);

		//Vertex A
		float VertexAx = m_Size * sin(m_Rotation * Hdata::PI / 180.f);
		float VertexAy = m_Size * cos(m_Rotation * Hdata::PI / 180.f);
		m_Vertices.push_back({ m_Position.m_X + VertexAx, m_Position.m_Y + VertexAy });

		//Vertex B - Opposite of A
		m_Vertices.push_back({ m_Position.m_X - VertexAx, m_Position.m_Y - VertexAy });

		break;
	}
	default:
		break;
	}

}
