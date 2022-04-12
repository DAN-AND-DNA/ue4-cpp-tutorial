#include "DeActor.h"



ADeActor::ADeActor()
	: client1(MakeShareable(new FClient()))
	, client2(MakeShareable(new FClient()))
	, client3(MakeShareable(new FClient())) {
}


void ADeActor::BeginPlay() {
	Super::BeginPlay();

	this->client4 = NewObject<UClient>(this, UClient::StaticClass());

	OnConnected.BindStatic(FClient::HandleOnConnected); 
	OnDisconnected.BindSP(this->client1, &FClient::HandleOnDisconnected);
	OnGameStart.BindUObject(client4, &UClient::HandleOnGameStart);	
	OnGameOver.BindThreadSafeSP(this->client3, &FClient::HandleOnGameOver);
	OnSendError.BindRaw(this->client2.Get(), &FClient::HandleOnSendError);
	OnReadError.BindLambda([](FError Error) {UE_LOG(LogTemp, Log, TEXT("HandleOnReadError")); });
	OnRead.BindWeakLambda(client4, [](FString msg) {UE_LOG(LogTemp, Log, TEXT("HandleOnRead")); });
	OnSend.BindUFunction(client4, STATIC_FUNCTION_FNAME(TEXT("UClient::HandleOnSend")));
	OnRoleCreated.AddUObject(client4, &UClient::HandleRoleCreated);

	this->OnUpdatedEventHandle = this->OnUpdated().AddUObject(this->client4, &UClient::OnUpdatedCallback);
	//OnOnline.BindUFunction(client4, STATIC_FUNCTION_FNAME(TEXT("UClient::HandleOnline")));
}

void ADeActor::BeginDestroy() {
	Super::BeginDestroy();

	OnConnected.Unbind();
	OnDisconnected.Unbind();
	OnGameStart.Unbind();
	OnGameOver.Unbind();
	OnSendError.Unbind();
	OnReadError.Unbind();
	OnRead.Unbind();
	OnSend.Unbind();

	OnRoleCreated.RemoveAll(this->client4);
	check(!OnRoleCreated.IsBound());
	check(!OnRoleCreated.IsBoundToObject(this->client4));

	OnRoleCreated.Clear();

	this->OnUpdated().Remove(this->OnUpdatedEventHandle);
	this->OnUpdated().Clear();

	OnOnline.Clear();
	OnOffline.Clear();
}


void ADeActor::ExecAll() {
	auto SendError = FError{ -1, TEXT("Send error") };
	auto ReadError = FError{ -1, TEXT("Read error") };

	OnConnected.ExecuteIfBound();
	OnDisconnected.ExecuteIfBound();
	OnGameStart.ExecuteIfBound();
	OnGameOver.ExecuteIfBound();
	OnSendError.ExecuteIfBound(MoveTemp(SendError));
	OnReadError.ExecuteIfBound(MoveTemp(ReadError));
	OnRead.ExecuteIfBound(TEXT("Send something"));
	OnSend.ExecuteIfBound();
	check(OnRoleCreated.IsBound());
	check(OnRoleCreated.IsBoundToObject(this->client4));
	OnRoleCreated.Broadcast("Hero");

	this->OnUpdated().Broadcast(TEXT(R"({"Name":"Dan","Level":99})"));
	OnOnline.ExecuteIfBound(TEXT("Dan"));
	OnOffline.Broadcast(TEXT("Lulu"));

}


void ADeActor::K2_RegisterUnderAttack(FOnUnderAttacked InUnderAttacked) {
	this->RegisterUnderAttack(FOnUnderAttacked_Delegate::CreateLambda([InUnderAttacked]() {
		InUnderAttacked.ExecuteIfBound();
	}));
}

void ADeActor::RegisterUnderAttack(FOnUnderAttacked_Delegate InUnderAttacked) {
	OnUnderAttacked = InUnderAttacked;
}