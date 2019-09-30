// Fill out your copyright notice in the Description page of Project Settings.

#include "RadialBlurDemo.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

class FGameModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
		AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
	}

	virtual void ShutdownModule() override
	{
		ResetAllShaderSourceDirectoryMappings();
	}

	virtual bool IsGameModule() const override
	{
		return true;
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FGameModule, RadialBlurDemo, "RadialBlurDemo" );
