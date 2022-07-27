#pragma once
#include "GameObject.h"
#include "Particle.h"

class DestructibleObject : public GameObject
{
public:
	DestructibleObject(ID2D1SolidColorBrush* a_Color);
	~DestructibleObject() = default;

	virtual void Explode(Particle* a_Particles[], Particle::EType a_Type = Particle::EType::Circle);

	//Getters
	const int GetParticleAmount() const;

protected:
	int m_ParticleAmount;
	float m_ExplodeCooldown;
	float m_ElapsedExplodeTime;
};

