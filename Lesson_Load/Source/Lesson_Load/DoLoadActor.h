#pragma once

#include "CoreMinimal.h"
#include "DoLoadActor.generated.h"


UCLASS()
class LESSON_LOAD_API ADoLoadActor : public AActor {
	GENERATED_BODY()

public:
	ADoLoadActor();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Load();

	UFUNCTION(BlueprintCallable)
	void UnLoad();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UTexture2D* Icon;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UTexture2D* Icon2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStaticMesh* Body;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AActor> Hero;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TSubclassOf<AActor> Hero2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UClass* Enemy;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowedClasses = "Texture2D"))
	FSoftObjectPath SoftIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (MetaClass = "Actor"))
	FSoftClassPath SoftHero;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> SoftIcon2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<AActor> SoftHero2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<UClass> SoftEnemy1;
};

