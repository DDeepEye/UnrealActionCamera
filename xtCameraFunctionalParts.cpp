// Fill out your copyright notice in the Description page of Project Settings.
#include "xtCameraFunctionalParts.h"


UxtCameraFunctionalParts::~UxtCameraFunctionalParts()
{
}

void UxtCameraFunctionalParts::SetTarget(class AxtBaseCamera* target)
{
	_target = target;
}

void UxtCameraFunctionalParts::Activate(float DeltaTime)
{
}
