// Fill out your copyright notice in the Description page of Project Settings.



#include "StatActor.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Lesson_Stat/Lesson_Stat.h"




// ����ͳ������
DECLARE_CYCLE_STAT(TEXT("Do fake op"), STAT_FakeOP, STATGROUP_DanProf);	// ͳ�Ʋ���
DECLARE_MEMORY_STAT(TEXT("Do fake memory alloc"), STAT_FakeMemoryAlloc, STATGROUP_DanProf); // ͳ���ڴ�



// Sets default values
AStatActor::AStatActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AStatActor::FakeOP(float InMs) const {
	
	{ // ͳ�ƿ�ʼ
		SCOPE_CYCLE_COUNTER(STAT_FakeOP)

		float DurationMs = 1.f / 1000.f;

		// cost 7ms to finish
		FPlatformProcess::Sleep(InMs * DurationMs);
	} // ͳ�ƽ���
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
	
	// �ĵ�: https://docs.unrealengine.com/4.27/zh-CN/TestingAndOptimization/PerformanceAndProfiling/StatCommands/
	// ���ɵ��ļ�λ��: \Saved\Profiling\UnrealStats

	FString Msg(TEXT("please input stat startfile"));
	UKismetSystemLibrary::PrintString(nullptr, Msg, true, false, FLinearColor{1,0,1,1});
}

// Called every frame
void AStatActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	static float TimePassed = 0;
	TimePassed += DeltaTime;
	
	// ��һЩ����
	this->FakeOP(7);
	
	// �����ڴ�
	this->FakeMemoryAlloc (this->BaseId++);

	// ÿ17������һ���ڴ�
	if (TimePassed >= 17.f) {
		TimePassed = 0;
		this->FakeMemoryClean();
	}
}

