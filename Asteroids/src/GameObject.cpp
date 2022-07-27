#include "GameObject.h"

GameObject::GameObject(ID2D1SolidColorBrush* a_Color) :
	m_pColor(a_Color)
{
	Init();
}

void GameObject::Update(float a_Dt)
{
	a_Dt;
}



const float& GameObject::GetRotation() const
{
	return m_Rotation;
}

const float& GameObject::GetSize() const
{
	return m_Size;
}

const float& GameObject::GetVelocity() const
{
	return m_Velocity;
}

const float& GameObject::GetMaxAcceleration() const
{
	return m_MaxAcceleration;
}

const float& GameObject::GetRotationSpeed() const
{
	return m_RotationSpeed;
}

const GameObject::EObjectState& GameObject::GetState() const
{
	return m_EState;
}

const Hdata::Vec2& GameObject::GetPosition() const
{
	return m_Position;
}

const Hdata::Vec2& GameObject::GetAcceleration() const
{
	return m_Acceleration;
}

const std::vector<D2D_POINT_2F>& GameObject::GetVertices() const
{
	return m_Vertices;
}

ID2D1SolidColorBrush* GameObject::GetBrush() const
{
	return m_pColor;
}

void GameObject::SetState(EObjectState a_EState)
{
	m_EState = a_EState;
}

void GameObject::Init()
{
	m_EState = EObjectState::Active;
	m_Position = { 0.f, 0.f };
	m_Acceleration = { 0.f, 0.f };
	m_RotationSpeed = 0.f;
	m_Rotation = 0.f;
	m_Size = 0.f;
	m_Velocity = 0.0f;
	m_MaxAcceleration = 0.f;
}

void GameObject::WrapScreen()
{
	//Check Boundaries (Outside of Window)
	if (m_Position.m_X < 0.f)
	{
		m_Position.m_X = static_cast<float>(Hdata::ScreenWidth);
	}
	if (m_Position.m_X > Hdata::ScreenWidth)
	{
		m_Position.m_X = 0.f;
	}
	if (m_Position.m_Y < 0.f)
	{
		m_Position.m_Y = static_cast<float>(Hdata::ScreenHeight);
	}
	if (m_Position.m_Y > Hdata::ScreenHeight)
	{
		m_Position.m_Y = 0.f;
	}
}
