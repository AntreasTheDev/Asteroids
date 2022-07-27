#include "Player.h"
#include "Bullet.h"

Player::Player(ID2D1SolidColorBrush* a_Color) :
	DestructibleObject(a_Color),
	m_ShootCooldown(0.25f),
	m_TeleportCooldown(1.f),
	m_InvulnerabilityCooldown(2.f)
{
	Init();
}

void Player::Update(float a_Dt)
{
	switch (m_EState)
	{
	case EObjectState::Active:
	case EObjectState::Invulnerable:
	{
		//Update Timers
		m_ElapsedShootTime += a_Dt;
		m_ElapsedTeleportTime += a_Dt;

		//Exit Invulnerability State
		if (m_EState == EObjectState::Invulnerable)
		{
			m_ElapsedInvulnerabilityTime += a_Dt;
			if (m_ElapsedInvulnerabilityTime >= m_InvulnerabilityCooldown)
			{
				m_ElapsedInvulnerabilityTime = 0.f;
				m_EState = EObjectState::Active;
			}
		}

		//Move
		m_Position.m_X += m_Acceleration.m_X * a_Dt;
		m_Position.m_Y += m_Acceleration.m_Y * a_Dt;

		//Wrap if we reached the end of the window
		WrapScreen();

		//Adjust Ship Vertices based on the new position and rotation
		float Ax = (m_Size / 2.f) * sin(m_Rotation * Hdata::PI / 180.f);
		float Ay = (m_Size / 2.f) * cos(m_Rotation * Hdata::PI / 180.f);

		float Bx = (m_Size / 4.f) * sin((m_Rotation - 120) * Hdata::PI / 180.f);
		float By = (m_Size / 4.f) * cos((m_Rotation - 120) * Hdata::PI / 180.f);

		float Cx = (m_Size / 8.f) * sin((m_Rotation - 90) * Hdata::PI / 180.f);
		float Cy = (m_Size / 8.f) * cos((m_Rotation - 90) * Hdata::PI / 180.f);

		float Dx = (m_Size / 8.f) * sin((m_Rotation + 90) * Hdata::PI / 180.f);
		float Dy = (m_Size / 8.f) * cos((m_Rotation + 90) * Hdata::PI / 180.f);

		float Ex = (m_Size / 4.f) * sin((m_Rotation + 120) * Hdata::PI / 180.f);
		float Ey = (m_Size / 4.f) * cos((m_Rotation + 120) * Hdata::PI / 180.f);


		m_Vertices[0] = { m_Position.m_X + Ax, m_Position.m_Y - Ay };					//Top Vertex of Isosceles Triangle (A)
		m_Vertices[1] = { m_Position.m_X + Bx, m_Position.m_Y - By };					//Left Vertex (B)
		m_Vertices[2] = { m_Position.m_X + Cx, m_Position.m_Y - Cy };					//Creates the arrow-like look (Left Side)
		m_Vertices[3] = { m_Position.m_X + Dx, m_Position.m_Y - Dy };					//Creates the arrow-like look (Right Side)
		m_Vertices[4] = { m_Position.m_X + Ex, m_Position.m_Y - Ey };					//Right Vertex (E)

		if (m_ThrustOn)
		{
			//Adjust Thrust Vertices based on the new position and rotation

			float TAx = (m_Size / 8.f) * sin(m_Rotation * Hdata::PI / 180.f);
			float TAy = (m_Size / 8.f) * cos(m_Rotation * Hdata::PI / 180.f);

			float TBx = (m_Size / 16.f) * sin((m_Rotation - 90) * Hdata::PI / 180.f);
			float TBy = (m_Size / 16.f) * cos((m_Rotation - 90) * Hdata::PI / 180.f);

			float TCx = (m_Size / 16.f) * sin((m_Rotation + 90) * Hdata::PI / 180.f);
			float TCy = (m_Size / 16.f) * cos((m_Rotation + 90) * Hdata::PI / 180.f);

			m_ThrustVertices[0] = { m_Position.m_X - TAx, m_Position.m_Y + TAy };		//Top Vertex of Isosceles Triangle(A) - Opposite side of m_Vertices[0]
			m_ThrustVertices[1] = { m_Position.m_X + TBx, m_Position.m_Y - TBy };		//Left Vertex (B)
			m_ThrustVertices[2] = { m_Position.m_X + TCx, m_Position.m_Y - TCy };		//Right Vertex (C)
		}

		break;
	}

	case EObjectState::Animating:
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

void Player::Move(float a_Dt)
{
	m_ThrustOn = true;

	m_Acceleration.m_X += m_Velocity * a_Dt * sin(m_Rotation * Hdata::PI / 180);
	m_Acceleration.m_Y -= m_Velocity * a_Dt * cos(m_Rotation * Hdata::PI / 180);

	if (m_Acceleration.m_X > m_MaxAcceleration)
		m_Acceleration.m_X = m_MaxAcceleration;
	else if (m_Acceleration.m_X < -m_MaxAcceleration)
		m_Acceleration.m_X = -m_MaxAcceleration;

	if (m_Acceleration.m_Y > m_MaxAcceleration)
		m_Acceleration.m_Y = m_MaxAcceleration;
	else if (m_Acceleration.m_Y < -m_MaxAcceleration)
		m_Acceleration.m_Y = -m_MaxAcceleration;	

}

void Player::Rotate(float a_Dt, bool a_Clockwise)
{
	if (a_Clockwise)
	{
		m_Rotation += m_RotationSpeed * a_Dt;
	}
	else
	{
		m_Rotation -= m_RotationSpeed * a_Dt;
	}
}

void Player::Shoot(Bullet* a_Bullets[])
{
	if (m_ElapsedShootTime >= m_ShootCooldown)
	{
		m_ElapsedShootTime = 0.f;
		for (int i = 0; i < Hdata::MaxBullets; i++)
		{
			if (!a_Bullets[i])
			{
				a_Bullets[i] = new Bullet(m_pColor, m_Rotation, m_Position);
				break;
			}
		}
	}
}

void Player::Teleport()
{
	if (m_ElapsedTeleportTime >= m_TeleportCooldown)
	{
		m_ElapsedTeleportTime = 0.f;
		m_Position.m_X = static_cast<float>(rand() % Hdata::ScreenWidth);
		m_Position.m_Y = static_cast<float>(rand() % Hdata::ScreenHeight);
	}
}

const int Player::GetThrustOn() const
{
	return m_ThrustOn;
}

const std::vector<D2D_POINT_2F>& Player::GetThrustVertices() const
{
	return m_ThrustVertices;
}

void Player::SetThrustOn(bool a_Value)
{
	m_ThrustOn = a_Value;
}

void Player::Init()
{
	m_Vertices.reserve(5);			//Reserve space for the shape of the ship
	m_ThrustVertices.reserve(3);	//Thrust is a triangle

	m_Position.m_X = static_cast<float>(Hdata::ScreenWidth) / 2.f;
	m_Position.m_Y = static_cast<float>(Hdata::ScreenHeight) / 2.f;
	m_Size = 40.f;
	m_MaxAcceleration = 300.f;
	m_Velocity = 200.f;
	m_RotationSpeed = 300.f;
	m_ElapsedShootTime = 0.f;
	m_ElapsedTeleportTime = 0.f;
	m_ElapsedInvulnerabilityTime = 0.f;
	m_ExplodeCooldown = 3.f;
	m_ParticleAmount = 10;
	m_EState = GameObject::EObjectState::Invulnerable;

	//Push dummy values, will get overwritten by Update()
	for (int i = 0; i < 5; i++)
	{
		m_Vertices.push_back({ m_Position.m_X, m_Position.m_Y });
	}
	for (int i = 0; i < 3; i++)
	{
		m_ThrustVertices.push_back({ m_Position.m_X, m_Position.m_Y });
	}

	printf("[LOG] Player Created \n");
}
