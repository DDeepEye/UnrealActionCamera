// Fill out your copyright notice in the Description page of Project Settings.
#include "xtCameraEffectManager.h"
#include "xtCameraShake.h"
#include "xtCameraBias.h"
#include "xtCameraShakeRepeatSection.h"
#include "xtBaseCamera.h"
#include "UI/xtInputManager.h"
#include "Resource/xtResourceCenter.h"
#include "Resource/xtPathMaker.h"
#include "Main/World/xtGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "NexterCommon/Logger/xtLogger.h"
#include "Resource/xtAssetManager.h"

const TCHAR* ShakeResourcePath = TEXT("/Game/Main/LogicData/Camera/ShakeResource/");

bool IxtCameraEffect::_isFlag(uint8 flags, uint8 check)
{
	return (flags & check) > 0;
}


UxtCameraEffectManager::UxtCameraEffectManager()
:SkipShake(false)
,_mixCount(0)
,_gameMode(nullptr)
{
	for (uint8 i = 0; i < MixCapacity; ++i){ _effectBuffers[i] = nullptr; }
	_pool.Reserve(8);
}

UxtCameraEffectManager::~UxtCameraEffectManager()
{
}

void UxtCameraEffectManager::Tick(float deltaTime)
{
	if (_getGameMode() == nullptr || _mixCount <= 0)
		return;

	AxtBaseCamera* camera = _getGameMode()->GetFollowCamera();
	if (camera == nullptr)
		return;

	FVector shakeLoc(camera->GetBias());
	FRotator shakeRot(FRotator::ZeroRotator);
	float shakeFov(camera->GetFov());
	uint8 mixProcCount(0);
	uint8 mixCount(_mixCount);

	for (uint8 i = 0; i < MixCapacity; ++i)
	{
		if (_effectBuffers[i].GetInterface())
		{
			if (_effectBuffers[i].GetInterface()->IsActive())
			{
				_effectBuffers[i].GetInterface()->TickEffect(deltaTime, shakeLoc, shakeRot, shakeFov);
				++mixProcCount;
			}

			if (_effectBuffers[i].GetInterface()->IsFinished())
			{
				_effectBuffers[i]->CleanUp(shakeLoc, shakeRot, shakeFov);
				_effectBuffers[i] = nullptr;
				if (_mixCount > 0) --_mixCount;
			}

			if(mixProcCount >= mixCount) break;
		}
	}

	if (_mixCount <= 0) shakeLoc = _originBias;

	camera->SetBias(SkipShake ? _originBias : shakeLoc);
	camera->SetActorRotation(SkipShake ? camera->GetActorRotation() : camera->GetActorRotation() + shakeRot);
	camera->SetFov(shakeFov);
}

void UxtCameraEffectManager::Shake(FxtShakeInfo& info)
{
	if (info.Duration <= 0 || _getGameMode() == nullptr || _mixCount >= MixCapacity)
		return;

	UxtCameraShake* shake = nullptr;

	for (int i = 0; i < _pool.Num(); ++i)
	{
		if (_pool[i]->GetType() == CameraEffectType::Shake)
		{
			if (!_pool[i]->IsActive())
			{
				shake = Cast<UxtCameraShake>(_pool[i].GetInterface());
				break;
			}
		}
	}

	float scale = Arbitrary(info.ActorUID, info.AnimUID);

	if (!shake) shake = PreLoadShake();
	if (!_mixBufferCheck(shake))
		return;

	_keepOriginalBias();

	shake->LocationScale = scale;
	shake->RotationScale = scale;
	shake->FovScale = scale;
	shake->Start(info);
}

void UxtCameraEffectManager::Shake(const FString path, float locScale, float rotScale)
{
	if (_getGameMode() == nullptr)
		return;

	UxtCameraShake* shake = PreLoadShake(*path);
	if (shake == nullptr)
		return;

	Shake(shake, locScale, rotScale);
}

void UxtCameraEffectManager::Shake(UxtCameraShake* shake, float locScale, float rotScale)
{
	if (!_mixBufferCheck(shake) || _getGameMode() == nullptr)
		return;

	UWaveOscillatorCameraShakePattern* pattern = Cast<UWaveOscillatorCameraShakePattern>(shake->GetRootShakePattern());
	if (pattern)
	{
		if (pattern->Duration <= 0)
		{
			return;
		}
	}

	_keepOriginalBias();

	shake->Start(locScale, rotScale);
}

void UxtCameraEffectManager::RepeatShake(struct FxtShakeRepeatSectionInfo& info)
{
	if (info.Duration <= 0 || _getGameMode() == nullptr || _mixCount >= MixCapacity)
		return;

	UxtCameraShakeRepeatSection* shake = nullptr;

	for (int i = 0; i < _pool.Num(); ++i)
	{
		if (_pool[i]->GetType() == CameraEffectType::RepeatPointShake)
		{
			if (!_pool[i]->IsActive())
			{
				shake = Cast<UxtCameraShakeRepeatSection>(_pool[i].GetInterface());
				break;
			}
		}
	}

	if (!shake) shake = PreLoadRepeatShake();

	if (!_mixBufferCheck(shake))
		return;

	_keepOriginalBias();

	shake->Start(info);
}

void UxtCameraEffectManager::Bias(struct FxtCameraBiasInfo& info)
{
	if (info.Duration <= 0 || _getGameMode() == nullptr || _mixCount >= MixCapacity)
		return;

	UxtCameraBias* bias = nullptr;

	for (int i = 0; i < _pool.Num(); ++i)
	{
		if (_pool[i]->GetType() == CameraEffectType::Bias)
		{
			if (!_pool[i]->IsActive())
			{
				bias = Cast<UxtCameraBias>(_pool[i].GetInterface());
				break;
			}
		}
	}

	float scale = Arbitrary(info.ActorUID, info.AnimUID);
	if (!bias) bias = PreLoadBias();
	if (!_mixBufferCheck(bias))
		return;

	_keepOriginalBias();
	
	bias->LocationScale = scale;
	bias->RotationScale = scale;
	bias->FovScale = scale;

	bias->Start(info);
}


UxtCameraShake* UxtCameraEffectManager::PreLoadShake(const TCHAR* fileName)
{
	UxtCameraShake* shake = nullptr;

	UClass* uclass = _assetToClass(fileName);
	if (uclass)
	{
		shake = NewObject<UxtCameraShake>(this, uclass);
		_pool.Add(shake);
	}
	return shake;
}

UxtCameraShake* UxtCameraEffectManager::PreLoadShake()
{
	UxtCameraShake* shake = nullptr;
	shake = NewObject<UxtCameraShake>(this);
	_pool.Add(shake);
	return shake;
}

void UxtCameraEffectManager::PreLoadShake(int32 loadCount)
{
	for (int32 i = 0; i < loadCount; ++i){ _pool.Add(NewObject<UxtCameraShake>(this)); }
}


UxtCameraShakeRepeatSection* UxtCameraEffectManager::PreLoadRepeatShake()
{
	UxtCameraShakeRepeatSection* shake = nullptr;
	shake = NewObject<UxtCameraShakeRepeatSection>(this);
	_pool.Add(shake);
	return shake;
}

void UxtCameraEffectManager::PreLoadRepeatShake(int32 loadCount)
{
	for (int32 i = 0; i < loadCount; ++i) { _pool.Add(NewObject<UxtCameraShakeRepeatSection>(this)); }
}

UxtCameraBias* UxtCameraEffectManager::PreLoadBias()
{
	UxtCameraBias* bias = nullptr;
	bias = NewObject<UxtCameraBias>(this);
	_pool.Add(bias);
	return bias;
}

void UxtCameraEffectManager::PreLoadBias(int32 loadCount)
{
	for (int32 i = 0; i < loadCount; ++i) { _pool.Add(NewObject<UxtCameraBias>(this)); }
}

float UxtCameraEffectManager::Arbitrary(uint64 actorUID, uint64 animUID)
{
	if(_mixCount <= 0)
		return 1.0f;

	float scale = 1.0f;

	AxtBaseCamera* camera = _getGameMode()->GetFollowCamera();
	if (camera == nullptr)
		return scale;

	FVector shakeLoc(camera->GetBias());
	FRotator shakeRot(FRotator::ZeroRotator);
	float shakeFov(camera->GetFov());
	
	for (int i = 0; i < MixCapacity; ++i)
	{
		if (_effectBuffers[i].GetInterface())
		{
			if (_effectBuffers[i].GetInterface()->IsArbitraryTermination(actorUID, animUID, shakeLoc, shakeRot, shakeFov, scale))
			{
				_effectBuffers[i] = nullptr;
				if (_mixCount > 0) --_mixCount;
			}
		}
	}

	camera->SetBias(SkipShake ? _originBias : shakeLoc);
	camera->SetActorRotation(SkipShake ? camera->GetActorRotation() : camera->GetActorRotation() + shakeRot);
	camera->SetFov(SkipShake ? camera->GetFov() : shakeFov);

	if (_mixCount <= 0) camera->SetBias(_originBias);
	
	return scale;
}

void UxtCameraEffectManager::Aboart(uint64 actorUID)
{
	if (_mixCount <= 0)
		return;

	AxtBaseCamera* camera = _getGameMode()->GetFollowCamera();
	if (camera == nullptr)
		return;

	FVector shakeLoc(camera->GetBias());
	FRotator shakeRot(FRotator::ZeroRotator);
	float shakeFov(camera->GetFov());

	for (int i = 0; i < MixCapacity; ++i)
	{
		if (_effectBuffers[i].GetInterface())
		{
			if (_effectBuffers[i].GetInterface()->IsArbitraryTermination(actorUID, shakeLoc, shakeRot, shakeFov))
			{
				_effectBuffers[i] = nullptr;
				if (_mixCount > 0) --_mixCount;
			}
		}
	}

	camera->SetBias(SkipShake ? _originBias : shakeLoc);
	camera->SetActorRotation(SkipShake ? camera->GetActorRotation() : camera->GetActorRotation() + shakeRot);
	camera->SetFov(SkipShake ? camera->GetFov() : shakeFov);

	if (_mixCount <= 0) camera->SetBias(_originBias);
}


void UxtCameraEffectManager::PostCreated()
{
	PreLoadShake(2);
	PreLoadRepeatShake(2);
	PreLoadBias(2);
}

void UxtCameraEffectManager::PreDestroy()
{
}

AxtGameModeBase* UxtCameraEffectManager::_getGameMode()
{
	if (_gameMode == nullptr) _gameMode = GetWorld()->GetAuthGameMode< AxtGameModeBase >();
	return _gameMode;
}

UClass* UxtCameraEffectManager::_assetToClass(FString shakeAssetName)
{
	FString filePath = FString::Printf(TEXT("%s%s.%s_C"), ShakeResourcePath, *shakeAssetName, *shakeAssetName);
	UObject* object = UxtAssetManager::Instance().RequestSyncLoad(filePath, ExtLoadTypes::Class);

	return object ? Cast<UClass>(object) : nullptr;
}


bool UxtCameraEffectManager::_mixBufferCheck(UObject* effect)
{
	if (_mixCount >= MixCapacity)
		return false;

	int count = 0;
	while (count < MixCapacity && _mixCount > 0)
	{
		if (_effectBuffers[count] == effect)
			return false;

		++count;
	}

	count = 0;
	while (count < MixCapacity)
	{
		if (_effectBuffers[count].GetInterface() == nullptr)
		{
			_effectBuffers[count] = effect;
			++_mixCount;
			return true;
		}
		++count;
	}

	return false;
}

void UxtCameraEffectManager::_keepOriginalBias()
{
	if (_mixCount <= 0)
	{
		AxtBaseCamera* camera = _getGameMode()->GetFollowCamera();
		if (camera)
		{
			_originBias = _getGameMode()->GetFollowCamera()->GetBias();
		}
	}
}


