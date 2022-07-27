#pragma once
#include "DestructibleObject.h"

class Bullet;

class Saucer : public DestructibleObject
{
public:
    enum class EType
    {
        Large = 0,
        Small
    };

    Saucer(ID2D1SolidColorBrush* a_Color, EType a_Type = EType::Large);
    ~Saucer() = default;

    void Update(float a_Dt) override;
    void Shoot(Bullet* a_Bullets[], Hdata::Vec2 a_target);

    //Getters
    const EType GetType() const;

private:
    void Init() override;
  
private:   
    const float m_RedirectCooldown;

    float m_ElapsedRedirectTime;
    float m_ShootCooldown;
    float m_ElapsedShootTime;

    float m_ShotInaccuracy;

    EType m_EType;
};

