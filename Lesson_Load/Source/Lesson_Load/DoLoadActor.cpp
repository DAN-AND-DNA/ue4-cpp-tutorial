#include "DoLoadActor.h"


ADoLoadActor::ADoLoadActor()
	: Icon2(nullptr)
	, Hero2(nullptr) {

	check(!IsValid(Icon2));
	auto EasyTextureFinder = ConstructorHelpers::FObjectFinder<UTexture2D>(TEXT("Texture2D'/Game/Texture/T_UI_ShowMove.T_UI_ShowMove'"));
	if (EasyTextureFinder.Succeeded()) {
		this->Icon2 = EasyTextureFinder.Object;
	}
	check(IsValid(Icon2));

	check(!IsValid(Hero2));
	static ConstructorHelpers::FClassFinder<AActor>  EasyClassFinder = ConstructorHelpers::FClassFinder<AActor>(TEXT( "/Game/Blueprint/BP_Hero.BP_Hero_C"));

	if (EasyClassFinder.Succeeded()) {
		this->Hero2 = EasyClassFinder.Class;
	}
	
	check(IsValid(Hero2));
}

void ADoLoadActor::BeginPlay() {
	Super::BeginPlay();
}

void ADoLoadActor::Load() {
	auto RawIcon = this->SoftIcon.TryLoad();
	if (RawIcon != nullptr) {
		UTexture2D* NewIcon = Cast<UTexture2D>(RawIcon);
		check(IsValid(NewIcon));
	}

	UClass* RawHero = this->SoftHero.TryLoadClass<AActor>();
	if (RawHero != nullptr) {
		check(IsValid(RawHero));
	}

	if (!this->SoftIcon2.IsPending()) {
		check(IsValid(this->SoftIcon2.Get()));
	}

	auto NewIcon2 = SoftIcon2.LoadSynchronous();
	check(IsValid(NewIcon2));

	if (!this->SoftHero2.IsPending()) {
		check(IsValid(this->SoftHero2.Get()));
	}
	
	auto NewHero2 = SoftHero2.LoadSynchronous();
	check(IsValid(NewHero2));
}


void ADoLoadActor::UnLoad() {
	if (IsValid(Icon)) {
		Icon = nullptr;
	}

	if (IsValid(Body)) {
		Body = nullptr;
	}

	if (IsValid(Hero)) {
		Hero = nullptr;
	}


	if (IsValid(Enemy)){
		Enemy = nullptr;
	}

	if (SoftIcon.IsValid()) {
		SoftIcon.Reset();
	}

	if (SoftHero.IsValid()) {
		SoftHero.Reset();
	}

	if (SoftIcon2.IsValid()) {
		SoftIcon2.Reset();
	}

	if (SoftHero2.IsValid()) {
		SoftHero2.Reset();
	}

	if (SoftEnemy1.IsValid()) {
		SoftEnemy1.Reset();
	}
}