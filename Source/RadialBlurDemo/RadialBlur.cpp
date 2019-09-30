// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialBlur.h"

#include "EngineUtils.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"

// Sets default values
ARadialBlur::ARadialBlur()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetTickGroup(ETickingGroup::TG_PostUpdateWork);
}

// Called when the game starts or when spawned
void ARadialBlur::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	// load material and create mid
	RadialBlurMat = LoadObject<UMaterial>(GetTransientPackage(), TEXT("/Game/M_RadialBlur.M_RadialBlur"));
	if (RadialBlurMat != nullptr)
	{
		RadialBlurMID = UMaterialInstanceDynamic::Create(RadialBlurMat, this, FName("RadialBlurMID"));
	}

	// find post process volume
	int32 num = GetWorld()->PostProcessVolumes.Num();
	if (num > 0)
	{
		PostProcessVolumeActor = (APostProcessVolume *)(World->PostProcessVolumes[0]);
		PostProcessVolumeActor->bEnabled = true;
		PostProcessVolumeActor->BlendWeight = 1.0f;
		PostProcessVolumeActor->bUnbound = true;
	}

	// find character
	for (TActorIterator<ACharacter> It(World); It; ++It)
	{
		Character = *It;
	}

	// find particle system emitter
	for (TActorIterator<AEmitter> It(World); It; ++It)
	{
		Emitter = *It;
	}

	// create scene capture 2d
	SceneCapture2D = World->SpawnActor<ASceneCapture2D>();

	FVector2D ViewportSize = FVector2D(1, 1);
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	USceneCaptureComponent2D* SceneCaptureComponent2D = SceneCapture2D->GetCaptureComponent2D();
	SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

	SceneCaptureComponent2D->TextureTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("SceneCaptureTextureTarget"));
	SceneCaptureComponent2D->TextureTarget->InitCustomFormat(ViewportSize.X, ViewportSize.Y, PF_A16B16G16R16, false);
	SceneCaptureComponent2D->TextureTarget->ClearColor = FLinearColor::Black;

	// set material RenderTexture
	RadialBlurMID->SetTextureParameterValue(TEXT("RenderTexture"), SceneCaptureComponent2D->TextureTarget);

	// disable capture fog
	FEngineShowFlagsSetting NewFlagSetting;
	NewFlagSetting.ShowFlagName = TEXT("Fog");
	NewFlagSetting.Enabled = false;
	SceneCaptureComponent2D->ShowFlagSettings.Add(NewFlagSetting);

	int32 SettingIndex = SceneCaptureComponent2D->ShowFlags.FindIndexByName(TEXT("Fog"));
	SceneCaptureComponent2D->ShowFlags.SetSingleFlag(SettingIndex, false);
}

// Called every frame
void ARadialBlur::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// update material params
	if (RadialBlurMID != nullptr)
	{
		RadialBlurMID->SetScalarParameterValue("CenterX", CenterX);
		RadialBlurMID->SetScalarParameterValue("CenterY", CenterY);
		RadialBlurMID->SetScalarParameterValue("Offset", Offset);
	}

	//update scene capture 2d
	APlayerCameraManager* PlayerCameraManager = nullptr;
	for (TActorIterator<APlayerController> It(GetWorld()); It; ++It)
	{
		PlayerCameraManager = (*It)->PlayerCameraManager;
	}

	if (PlayerCameraManager != nullptr)
	{
		SceneCapture2D->SetActorLocationAndRotation(PlayerCameraManager->GetCameraLocation(), PlayerCameraManager->GetCameraRotation());

		USceneCaptureComponent2D* SceneCaptureComponent2D = SceneCapture2D->GetCaptureComponent2D();
		SceneCaptureComponent2D->ProjectionType = PlayerCameraManager->bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
		SceneCaptureComponent2D->FOVAngle = PlayerCameraManager->GetFOVAngle();
		SceneCaptureComponent2D->OrthoWidth = PlayerCameraManager->GetOrthoWidth();
	}
}

void ARadialBlur::Trigger()
{
	USceneCaptureComponent2D* SceneCaptureComponent2D = SceneCapture2D->GetCaptureComponent2D();
	if (!SceneCaptureComponent2D)
	{
		return;
	}

	if (Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		Mesh->bOwnerNoSee = true;
		Mesh->MarkRenderStateDirty();
		
		SceneCaptureComponent2D->ShowOnlyActors.Add(Character);
	}

	if (Emitter)
	{
		Emitter->SetOwner(Character);

		UParticleSystemComponent* psc = Emitter->GetParticleSystemComponent();
		psc->bOwnerNoSee = true;
		psc->MarkRenderStateDirty();

		SceneCaptureComponent2D->ShowOnlyActors.Add(Emitter);
	}

	if (PostProcessVolumeActor != nullptr)
	{
		PostProcessVolumeActor->AddOrUpdateBlendable(RadialBlurMID, 1);
	}
}

void ARadialBlur::Shutdown()
{
	USceneCaptureComponent2D* SceneCaptureComponent2D = SceneCapture2D->GetCaptureComponent2D();
	if (!SceneCaptureComponent2D)
	{
		return;
	}

	SceneCaptureComponent2D->ShowOnlyActors.Empty();

	if (Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		Mesh->bOwnerNoSee = false;
		Mesh->MarkRenderStateDirty();		
	}

	if (Emitter)
	{
		Emitter->SetOwner(nullptr);

		UParticleSystemComponent* psc = Emitter->GetParticleSystemComponent();
		psc->bOwnerNoSee = false;
		psc->MarkRenderStateDirty();
	}

	if (PostProcessVolumeActor != nullptr)
	{
		PostProcessVolumeActor->AddOrUpdateBlendable(RadialBlurMID, 0);
	}
}


