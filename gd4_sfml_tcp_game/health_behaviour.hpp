// Ruby White - D00255322 - Whole Class
#pragma once

#include "attachable_behaviour.hpp"

class HealthBehaviour : public AttachableBehaviour {
public:
    HealthBehaviour(float health=1.7f);

    void ChangeHealthBy(float health);

    void Update(sf::Time dt, CommandQueue& commands) override;

private:
    float hit_points_;
    float max_hit_points_;
};

