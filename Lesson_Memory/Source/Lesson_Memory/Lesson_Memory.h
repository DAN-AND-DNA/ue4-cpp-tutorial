// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/LowLevelMemStats.h"
#include "Runtime/Core/Public/HAL/LowLevelMemTracker.h"

enum class EMyLLMTag
{
	ProjectMax = (int)(ELLMTag::ProjectTagStart),
	DanMemProf,
};

class FLessonMemoryModule : public FDefaultGameModuleImpl {
public:
	virtual void StartupModule() override;
};