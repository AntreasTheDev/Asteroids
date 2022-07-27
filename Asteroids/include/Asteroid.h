#pragma once
#include "DestructibleObject.h"
#include "Particle.h"

class Asteroid : public DestructibleObject
{
public:
	//Needs to be defined before Constructor
	enum class EType
	{
		Large = 0,
		Medium,
		Small
	};

	Asteroid(ID2D1SolidColorBrush* a_Color, EType a_Type = EType::Large);
	~Asteroid() = default;

	void Update(float a_Dt) override;
	void SplitAsteroid(Asteroid* a_Asteroids[], int& a_Score);

private:
	Asteroid(ID2D1SolidColorBrush* a_Color, Hdata::Vec2 a_Position, EType a_Type = EType::Large);
	void Init() override;

private:

	EType m_EType;
	std::vector<D2D_POINT_2F> m_VertexWeight;	//Helps creating a random Asteroid Shape
	std::vector<Particle> m_Particles;
};

