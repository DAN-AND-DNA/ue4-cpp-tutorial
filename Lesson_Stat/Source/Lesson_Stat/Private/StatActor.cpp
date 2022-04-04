// Fill out your copyright notice in the Description page of Project Settings.



#include "StatActor.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Lesson_Stat/Lesson_Stat.h"




// 定义统计数据
DECLARE_CYCLE_STAT(TEXT("Do fake op"), STAT_FakeOP, STATGROUP_DanProf);	// 统计操作
DECLARE_MEMORY_STAT(TEXT("Do fake memory alloc"), STAT_FakeMemoryAlloc, STATGROUP_DanProf); // 统计内存



// Sets default values
AStatActor::AStatActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AStatActor::FakeOP(float InMs) const {
	
	{ // 统计开始
		SCOPE_CYCLE_COUNTER(STAT_FakeOP)

		float DurationMs = 1.f / 1000.f;

		// cost 7ms to finish
		FPlatformProcess::Sleep(InMs * DurationMs);
	} // 统计结束
}

void AStatActor::FakeMemoryAlloc(int32 InId) {
	{
		auto PtrLeak = new AStatActor::A(InId);
		INC_MEMORY_STAT_BY(STAT_FakeMemoryAlloc, sizeof(*PtrLeak));
		this->RawPtrs.Add(InId, PtrLeak);
	}
}

void AStatActor::FakeMemoryClean() {

	FString Msg(TEXT("start do mem clean"));
	UKismetSystemLibrary::PrintString(nullptr, Msg, true, false, FLinearColor{ 1,0,1,1 });

	for (TPair<int32, AStatActor::A*>& PairPtr : this->RawPtrs) {
		auto DelMemSize = sizeof(*(PairPtr.Value));
		delete PairPtr.Value;
		PairPtr.Value = nullptr;
		DEC_MEMORY_STAT_BY(STAT_FakeMemoryAlloc, DelMemSize);
	}
	this->RawPtrs.Empty();
	
}

// Called when the game starts or when spawned
void AStatActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 文档: https://docs.unrealengine.com/4.27/zh-CN/TestingAndOptimization/PerformanceAndProfiling/StatCommands/
	// 生成的文件位置: \Saved\Profiling\UnrealStats

	FString Msg(TEXT("please input stat startfile"));
	UKismetSystemLibrary::PrintString(nullptr, Msg, true, false, FLinearColor{1,0,1,1});
}

// Called every frame
void AStatActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	static float TimePassed = 0;
	TimePassed += DeltaTime;
	
	// 做一些操作
	this->FakeOP(7);
	
	// 分配内存
	this->FakeMemoryAlloc (this->BaseId++);

	// 每17秒清理一次内存
	if (TimePassed >= 17.f) {
		TimePassed = 0;
		this->FakeMemoryClean();
	}
}

