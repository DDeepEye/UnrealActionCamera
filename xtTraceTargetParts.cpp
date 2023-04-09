// Fill out your copyright notice in the Description page of Project Settings.
#include "xtTraceTargetParts.h"
#include "xtBaseCamera.h"

UxtTraceTargetParts::~UxtTraceTargetParts()
{

}

static const float s_leastMovingSize = 1.0f;
void UxtTraceTargetParts::Activate(float DeltaTime)
{
	if (_target.Get()->ChaseTarget == nullptr || !_target.Get()->IsTrace)
	{
		return;
	}

	AxtBaseCamera* target = _target.Get();

	FVector distVector = target->ChaseTarget.Get()->GetComponentLocation() - target->GetActorLocation();
	float dist = distVector.Size();
	if (dist > s_leastMovingSize)
	{
		FVector move;
		if (dist > target->LimitChaseTailDist + (target->MovingSpeed * DeltaTime))
		{
			float rate = target->LimitChaseTailDist / dist;
			move = target->ChaseTarget.Get()->GetComponentLocation() - (distVector * rate);
		}
		else
		{
			FVector origin = distVector;
			distVector.Normalize();
			move = distVector * DeltaTime * target->MovingSpeed;
			move.Z *= target->UpDownMorphRate;
			move = move.Size() > dist ? origin : move;
			move += target->Center.Get()->GetComponentLocation();
		}
		target->PrevLocation = target->GetActorLocation();
		target->SetActorTransform(FTransform(target->Center.Get()->GetComponentRotation(), move));
	}
}
