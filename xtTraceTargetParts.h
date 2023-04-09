// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "xtCameraFunctionalParts.h"
#include "xtTraceTargetParts.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NEXTER_API UxtTraceTargetParts : public UxtCameraFunctionalParts
{
	GENERATED_BODY()
public:
	virtual ~UxtTraceTargetParts() override;
	virtual void Activate(float DeltaTime) override;
};
