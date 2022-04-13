#pragma once

#include "CoreMinimal.h"

#include "GCActor.generated.h"


class FEasyGcObj : public FGCObject {

public:
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
	class UEasyObj* Obj1;
};


UCLASS()
class UEasyObj : public UObject {
	GENERATED_BODY()
public:
	UEasyObj();
};


USTRUCT()
struct FEasyStruct {
	GENERATED_BODY()
	FString Name;
};

UCLASS()
class LESSON_GC_API AGCActor : public AActor {
	GENERATED_BODY()
public:
	AGCActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	UEasyObj* Obj1; 

	UPROPERTY()
	UEasyObj* Obj2;

	UPROPERTY()
	UEasyObj* Obj3;

	UPROPERTY()
	FEasyStruct StructEasy1;

	FEasyStruct* StructEasy2;

	float Passed;
	bool bDoOnceGC;
};

