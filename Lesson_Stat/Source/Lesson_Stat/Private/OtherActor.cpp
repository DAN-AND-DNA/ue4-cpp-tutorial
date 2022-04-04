// Fill out your copyright notice in the Description page of Project Settings.


#include "OtherActor.h"
#include "Lesson_Stat/Lesson_Stat.h"

DECLARE_CYCLE_STAT(TEXT("Do Echo"), STAT_Echo, STATGROUP_DanProf);

// Sets default values
AOtherActor::AOtherActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOtherActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOtherActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	this->Echo();
}

void AOtherActor::Echo() {
	{
		SCOPE_CYCLE_COUNTER(STAT_Echo);

		for (int32 i = 0; i < 100000; i++) {
			auto Ptr = TSharedPtr<AOtherActor::B>(new AOtherActor::B());
		}
	}
}

