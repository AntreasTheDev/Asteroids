#pragma once
#include "GameObject.h"

class Particle : public GameObject
{
public:

	enum class EType
	{
		Circle = 0,
		Line
	};

	Particle(
		ID2D1SolidColorBrush* a_Color,  
		Hdata::Vec2 a_Position, 
		bool a_IsBullet = false,
		EType a_Type = EType::Circle,	
		float a_Velocity = 500.f,
		float a_LifeTime = 0.1f, 
		float a_Size = 2.f,	
		int a_MaxVertices = 2
	);

	~Particle() = default;
	virtual void Update(float a_Dt) override;

protected:
	virtual void Init() override;

protected:
	EType m_EType;
	float m_LifeTime;
	float m_ElapsedTime;
	const int m_MaxVertices;	//Always set to 2 in order to preserve Art Style
};

