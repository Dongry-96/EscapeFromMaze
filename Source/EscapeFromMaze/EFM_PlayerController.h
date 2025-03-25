#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EFM_PlayerController.generated.h"

UCLASS()
class ESCAPEFROMMAZE_API AEFM_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEFM_PlayerController();

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerNickname(const FString& NewName);
	void Server_SetPlayerNickname_Implementation(const FString& NewName);

	UFUNCTION(Client, Reliable)
	void Client_ReachedGoal(const FString& PlayerName);
	void Client_ReachedGoal_Implementation(const FString& PlayerName);

	UFUNCTION(Client, Reliable)
	void Client_ExitGame();
	void Client_ExitGame_Implementation();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	void SetVisibleText(class UTextBlock* TextBlock, bool bVisible);
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> NameInputWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

private:
	UUserWidget* NameInputWidgetInstance;
	UUserWidget* HUDWidgetInstance;

	class UTextBlock* InputGuideText;
	class UEditableTextBox* InputTextBox;
	class UTextBlock* HUDText;
	class UTextBlock* PlayerNameText;

	FString PlayerNickname;
};
