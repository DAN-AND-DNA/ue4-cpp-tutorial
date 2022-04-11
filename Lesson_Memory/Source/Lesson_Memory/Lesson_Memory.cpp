// Copyright Epic Games, Inc. All Rights Reserved.

#include "Lesson_Memory.h"
#include "Modules/ModuleManager.h"



IMPLEMENT_PRIMARY_GAME_MODULE(FLessonMemoryModule, Lesson_Memory, "Lesson_Memory" );

DECLARE_LLM_MEMORY_STAT(TEXT("DanMemProf"), STAT_DanMemProfLLM, STATGROUP_LLMFULL);


void FLessonMemoryModule::StartupModule(){
	LLM(FLowLevelMemTracker::Get().RegisterProjectTag((int32)EMyLLMTag::DanMemProf, TEXT("DanMemProf"), GET_STATFNAME(STAT_DanMemProfLLM), NAME_None));
}

