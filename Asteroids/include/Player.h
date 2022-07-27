#pragma once
#include "DestructibleObject.h"
#include "Bullet.h"

class Player : public DestructibleObject
{
public:
	Player(ID2D1SolidColorBrush* a_Color);
	~Player() = default;

	void Update(float a_Dt) override;
	void Move(float a_Dt);
	void Rotate(float a_Dt, bool a_Clockwise);
	void Shoot(Bullet* a_Bullets[]);
	void Teleport();

	//Getters
	const int GetThrustOn() const;
	const std::vector<D2D_POINT_2F>& GetThrustVertices() const;

	//Setters
	void SetThrustOn(bool a_Value);

private:
	void Init() override;

private:
	std::vector<D2D_POINT_2F> m_ThrustVertices;

	const float m_ShootCooldown;
	const float m_TeleportCooldown;
	const float m_InvulnerabilityCooldown;

	float m_ElapsedShootTime;
	float m_ElapsedTeleportTime;
	float m_ElapsedInvulnerabilityTime;

	bool m_ThrustOn;
};

