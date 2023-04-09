// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "xtFunctionalParts.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NEXTER_API UxtFunctionalParts : public UObject
{
	GENERATED_BODY()
public:
	virtual ~UxtFunctionalParts();
	virtual void Activate(float DeltaTime);
	virtual void SetTarget(class AxtBaseCamera* target);

protected:
	UPROPERTY(Transient)
	TObjectPtr<class AxtBaseCamera> _target;
};
