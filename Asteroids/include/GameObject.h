#pragma once
#include "HelperData.h"

class GameObject
{
public:
 
	enum class EObjectState
	{
		Active = 0,
		Hit,
		Animating,
		Invulnerable,
		ToDestroy
	};

	GameObject(ID2D1SolidColorBrush* a_Color);
	virtual ~GameObject() {};
	virtual void Update(float a_Dt);

	//Getters
	const float& GetRotation() const;
	const float& GetSize() const;
	const float& GetVelocity() const;
	const float& GetMaxAcceleration() const;
	const float& GetRotationSpeed() const;
	const EObjectState& GetState() const;
	const Hdata::Vec2& GetPosition() const;
	const Hdata::Vec2& GetAcceleration() const;
	const std::vector<D2D_POINT_2F>& GetVertices() const;
	ID2D1SolidColorBrush* GetBrush() const;

	//Setters
	void SetState(EObjectState a_EState);

protected:
	virtual void Init();
	virtual void WrapScreen();

protected:

	Hdata::Vec2 m_Position;
	Hdata::Vec2 m_Acceleration;
	EObjectState m_EState;

	float m_Velocity;
	float m_MaxAcceleration;
	float m_RotationSpeed;
	float m_Rotation; 
	float m_Size;

	std::vector<D2D_POINT_2F> m_Vertices;
	ID2D1SolidColorBrush* m_pColor;
};

