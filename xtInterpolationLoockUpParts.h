// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "xtCameraFunctionalParts.h"
#include "xtInterpolationLoockUpParts.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NEXTER_API UxtInterpolationLoockUpParts : public UxtCameraFunctionalParts
{
	GENERATED_BODY()
public:
	virtual ~UxtInterpolationLoockUpParts() override;
	virtual void Activate(float DeltaTime) override;
};
