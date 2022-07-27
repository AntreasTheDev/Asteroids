#pragma once
#include "Particle.h"

class Bullet : public Particle
{
public:
	enum class EType
	{
		Player = 0,
		Saucer
	};

	Bullet(ID2D1SolidColorBrush* a_Color, float a_Rotation, Hdata::Vec2 a_Position, EType a_Type = EType::Player);
	~Bullet() = default;

	void Update(float a_Dt) override;

	//Getters
	const EType& GetType() const;

private:
	void Init() override;

private:
	EType m_EType;
};

