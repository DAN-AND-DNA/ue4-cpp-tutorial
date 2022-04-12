#pragma once

#include "CoreMinimal.h"
#include "DeActor.generated.h"

struct FError {
	int32 Code;
	FString Message;
};

DECLARE_DELEGATE(FOnConnected_Delegate)
DECLARE_DELEGATE(FOnGameStart_Delegate)
DECLARE_DELEGATE(FOnGameOver_Delegate)
DECLARE_DELEGATE(FOnDisconnected_Delegate)
DECLARE_DELEGATE_OneParam(FOnSendError_Delegate, FError /*error*/)
DECLARE_DELEGATE_OneParam(FOnReadError_Delegate, FError  /*error*/)
DECLARE_DELEGATE_OneParam(FOnRead_Delegate, FString /*msg*/)
DECLARE_DELEGATE(FOnSend_Delegate)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoleCreated, const FString& /*name*/);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnOnline, const FString&, Name);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOffline, const FString&, Name);
DECLARE_DYNAMIC_DELEGATE(FOnUnderAttacked);
DECLARE_DELEGATE(FOnUnderAttacked_Delegate)

class FClient {
public:
	static void HandleOnConnected() { UE_LOG(LogTemp, Log, TEXT("HandleOnConnected")); }
	void HandleOnDisconnected() { UE_LOG(LogTemp, Log, TEXT("HandleOnDisconnected")); }
	void HandleOnSendError(FError error) { UE_LOG(LogTemp, Log, TEXT("HandleOnSendError")); }
	void HandleOnGameOver() { UE_LOG(LogTemp, Log, TEXT("HandleOnGameOver")); }
};

UCLASS()
class UClient: public UObject {
	GENERATED_BODY()
public:
	UFUNCTION()
	void HandleOnGameStart() { UE_LOG(LogTemp, Log, TEXT("HandleOnGameStart")); };

	UFUNCTION()
	void HandleOnSend() { 
		UE_LOG(LogTemp, Log, TEXT("HandleOnSend")); 
	};

	
	void OnUpdatedCallback(const FString& Msg) { UE_LOG(LogTemp, Log, TEXT("OnUpdatedCallback: %s"), *Msg); }

	void HandleRoleCreated (const FString& Name) { UE_LOG(LogTemp, Log, TEXT("HandleRoleCreated: %s"), *Name); }
	
	UFUNCTION()
	void HandleOnline(const FString& Name) { UE_LOG(LogTemp, Log, TEXT("HandleOnline: %s"), *Name); }
};

UCLASS()
class LESSON_DELEGATE_API ADeActor : public AActor {
	GENERATED_BODY()
public:
	ADeActor();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintCallable)
	void ExecAll();

	DECLARE_EVENT_OneParam(ADeActor, FUpdatedEvent, const FString&)
	FUpdatedEvent& OnUpdated() { return OnUpdatedEvent; }

	UFUNCTION(BlueprintCallable)
	void RegisterOnline(FOnOnline InOnline) { OnOnline = InOnline; }

	UFUNCTION(BlueprintCallable, meta = (Displayname="RegisterUnderAttacked"))
	void K2_RegisterUnderAttack(FOnUnderAttacked InUnderAttacked);

	void RegisterUnderAttack(FOnUnderAttacked_Delegate InUnderAttacked);

	UPROPERTY(BlueprintAssignable)
	FOnOffline OnOffline;

private:
	TSharedRef<class FClient> client1;
	TSharedPtr<class FClient> client2;
	TSharedRef<class FClient, ESPMode::ThreadSafe> client3;

	UPROPERTY()
	UClient* client4;

	FOnConnected_Delegate		OnConnected;
	FOnGameStart_Delegate		OnGameStart;
	FOnGameOver_Delegate		OnGameOver;
	FOnDisconnected_Delegate	OnDisconnected;
	FOnSendError_Delegate		OnSendError;
	FOnReadError_Delegate		OnReadError;
	FOnRead_Delegate			OnRead;
	FOnSend_Delegate			OnSend;
	FOnRoleCreated				OnRoleCreated;

	FUpdatedEvent				OnUpdatedEvent;
	FDelegateHandle				OnUpdatedEventHandle;

	FOnOnline					OnOnline;
	FOnUnderAttacked_Delegate	OnUnderAttacked;
};