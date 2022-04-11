#pragma once

#include "CoreMinimal.h"
#include "MemoryActor.generated.h"




UCLASS()
class LESSON_MEMORY_API AMemoryActor: public AActor {
	GENERATED_BODY()
public:

	AMemoryActor();
private:

	virtual void Tick(float DeltaTime) override;
	static void Init();
private:
	float Passed;
	class A* Ptra;
};