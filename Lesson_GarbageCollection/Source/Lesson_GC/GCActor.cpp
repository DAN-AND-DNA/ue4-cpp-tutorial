#include "GCActor.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


void FEasyGcObj::AddReferencedObjects(FReferenceCollector& Collector) {
	Collector.AddReferencedObject(this->Obj1);
}

UEasyObj::UEasyObj(){
}

AGCActor::AGCActor()
	: Passed(0)
	, bDoOnceGC(false) {
}

void AGCActor::BeginPlay() {
	Super::BeginPlay();

	this->Obj1 = NewObject<UEasyObj>(this, UEasyObj::StaticClass());
	this->Obj2 = NewObject<UEasyObj>();
	this->Obj3 = NewObject<UEasyObj>();

	check(IsValid(this->Obj1));
	check(IsValid(this->Obj2));
	check(IsValid(this->Obj3))

		this->Obj1 = nullptr;

	this->Obj2->ConditionalBeginDestroy();
	this->Obj2 = nullptr;

	this->Obj3->MarkPendingKill();
	this->Obj3 = nullptr;

	check(!IsValid(this->Obj1));
	check(!IsValid(this->Obj2));
	check(!IsValid(this->Obj3));

	UEasyObj* Obj4 = NewObject<UEasyObj>();
	Obj4->AddToRoot();
	
	Obj4->RemoveFromRoot();
	Obj4 = nullptr;

	this->StructEasy1 = FEasyStruct{};
	this->StructEasy2 = (FEasyStruct*)(FMemory::Malloc(sizeof(FEasyStruct)));
	check(this->StructEasy2 != nullptr);
	FMemory::Free(this->StructEasy2);
	this->StructEasy2 = nullptr;
	check(this->StructEasy2 == nullptr);

	auto StructEasy3 = (FEasyStruct*)(FMemory::Malloc(sizeof(FEasyStruct)));
	check(StructEasy3 != nullptr);
	FMemory::Free(StructEasy3);
	StructEasy3 = nullptr;
}

void AGCActor::Tick(float DeltaTime) {
	/*
	Passed += DeltaTime;

	
	if (Passed >= 1.f) {
		if (!bDoOnceGC) {
			UKismetSystemLibrary::PrintString(nullptr, FString::Printf(TEXT("Do GC")));

			GEngine->ForceGarbageCollection(true);
			bDoOnceGC = true;
		}
	}

	if (Passed >= 3.f) {
		Passed = 0.f;
		//UE_LOG(LogTemp, Log, TEXT("Obj2 is alive : %d"), IsValid(this->Obj2));
		UKismetSystemLibrary::PrintString(nullptr, FString::Printf(TEXT("Obj2 is alive : %d"), IsValid(this->Obj2)));
	}
	*/
}