#include "DestructibleObject.h"

DestructibleObject::DestructibleObject(ID2D1SolidColorBrush* a_Color) :
	GameObject(a_Color)
{
	m_ParticleAmount = 10;
	m_ExplodeCooldown = 1.0f;
	m_ElapsedExplodeTime = 0.f;
}

void DestructibleObject::Explode(Particle* a_Particles[], Particle::EType a_Type)
{
	switch (a_Type)
	{
	case Particle::EType::Circle:
	{
		for (int i = 0; i < m_ParticleAmount; i++)
		{
			if (!a_Particles[i])
			{
				a_Particles[i] = new Particle(m_pColor, m_Position);
			}
		}
		break;
	}
	case Particle::EType::Line:
	{
		for (int i = 0; i < m_ParticleAmount; i++)
		{
			//Find a Rand Position around the Exploding Target
			const float ExplosionRange = 25;
			Hdata::Vec2 Position;
			Position.m_X = Hdata::RandomNumber(-ExplosionRange + m_Position.m_X, ExplosionRange + m_Position.m_X);
			Position.m_Y = Hdata::RandomNumber(-ExplosionRange + m_Position.m_Y, ExplosionRange + m_Position.m_Y);

			if (!a_Particles[i])
			{
				a_Particles[i] = new Particle(
					m_pColor, 
					Position, 
					false, 
					Particle::EType::Line,
					10.f,
					2.f,
					5.f
				);
			}
		}
		break;
	}
	default:
		break;
	}
}

const int DestructibleObject::GetParticleAmount() const
{
	return m_ParticleAmount;
}
