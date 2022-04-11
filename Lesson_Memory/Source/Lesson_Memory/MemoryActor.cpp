#include "MemoryActor.h"
#include "Lesson_Memory/Lesson_Memory.h"



class A {
public:
	A() 
		: Age(17) 
		, Name(TEXT("ddddddddddddddddddddddddddddddddddddddddddddd")) {
	}
	int32 Age;
	FString Name;
};

AMemoryActor::AMemoryActor() {
	
	auto Ptr = new(A);
	delete(Ptr);
	Ptr = nullptr;

	check(Ptr == nullptr);
}

void AMemoryActor::Tick(float DeltaTime) {

	{
		LLM_SCOPE(ELLMTag((int)(EMyLLMTag::DanMemProf)));
		Ptra = new(A);
		//delete Ptra;
	}
	
	
}

void AMemoryActor::Init() {
}