// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "xtCameraFunctionalParts.h"
#include "xtLookUpParts.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NEXTER_API UxtLookUpParts : public UxtCameraFunctionalParts
{
	GENERATED_BODY()
public:
	virtual ~UxtLookUpParts();
	virtual void Activate(float DeltaTime);
};
