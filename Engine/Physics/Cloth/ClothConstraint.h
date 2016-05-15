#pragma once
#include "ClothParticle.h"
#include <memory>

class ClothConstraint
{
public:
	float restDistance;
	const std::shared_ptr<ClothParticle>& particle1;
	const std::shared_ptr<ClothParticle>& particle2;

	ClothConstraint(const std::shared_ptr<ClothParticle>& p1, const std::shared_ptr<ClothParticle>& p2) : particle1(p1), particle2(p2)
	{
		restDistance = length(particle1->pos - particle2->pos);
	}

	~ClothConstraint()
	{
	}

	void ApplyConstraint()
	{
		vec3 P1toP2 = particle2->pos - particle1->pos;
		float curDistance = length(P1toP2);

		vec3 correctionVec = P1toP2 * (1 - restDistance / curDistance);
		vec3 correctionVecHalf = correctionVec * 0.5f;

		particle1->OffsetPosition(correctionVecHalf);
		particle2->OffsetPosition(-correctionVecHalf);
	}
};

