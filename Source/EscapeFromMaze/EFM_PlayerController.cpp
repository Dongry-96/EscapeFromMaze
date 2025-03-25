#include "EFM_PlayerController.h"
#include "EFM_PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetSystemLibrary.h"

AEFM_PlayerController::AEFM_PlayerController()
{
	NameInputWidgetClass = nullptr;
	NameInputWidgetInstance = nullptr;
	HUDWidgetClass = nullptr;
	HUDWidgetInstance = nullptr;
	InputGuideText = nullptr;
	InputTextBox = nullptr;
	HUDText = nullptr;
	PlayerNameText = nullptr;
}

void AEFM_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && NameInputWidgetClass)
	{
		NameInputWidgetInstance = CreateWidget<UUserWidget>(this, NameInputWidgetClass);
		if (NameInputWidgetInstance)
		{
			NameInputWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());

			InputGuideText = Cast<UTextBlock>(NameInputWidgetInstance->GetWidgetFromName(TEXT("InputGuideText")));
			InputTextBox = Cast<UEditableTextBox>(NameInputWidgetInstance->GetWidgetFromName(TEXT("InputTextBox")));

			if (InputTextBox)
			{
				InputTextBox->OnTextCommitted.AddDynamic(this, &AEFM_PlayerController::OnInputCommitted);
			}
		}
	}
}

void AEFM_PlayerController::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsLocalController()) return;

	if (CommitMethod == ETextCommit::OnEnter && !Text.IsEmpty())
	{
		Server_SetPlayerNickname(Text.ToString());

		if (NameInputWidgetInstance)
		{
			NameInputWidgetInstance->RemoveFromParent();
			NameInputWidgetInstance = nullptr;
		}

		if (HUDWidgetClass)
		{
			HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

			if (HUDWidgetInstance)
			{
				HUDText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("HUDText")));
				PlayerNameText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("PlayerNameText")));

				if (HUDText)
				{
					HUDText->SetText(FText::FromString(TEXT("미로를 탈출하려면 트로피를 찾으세요!")));
				}

				if (PlayerNameText)
				{
					PlayerNameText->SetText(Text);
				}

				FTimerHandle HiddenTextTimer;
				FTimerDelegate HiddenTextDelegate;
				HiddenTextDelegate.BindUObject(this, &AEFM_PlayerController::SetVisibleText, HUDText, false);
				GetWorld()->GetTimerManager().SetTimer(HiddenTextTimer, HiddenTextDelegate, 4.0f, false);
			}
		}
	}
}

void AEFM_PlayerController::Server_SetPlayerNickname_Implementation(const FString& NewName)
{
	if (AEFM_PlayerState* EFM_PlayerState = GetPlayerState<AEFM_PlayerState>())
	{
		EFM_PlayerState->SetPlayerNickname(NewName);
	}
}

void AEFM_PlayerController::Client_ReachedGoal_Implementation(const FString& PlayerName)
{
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	if (HUDWidgetInstance && HUDText)
	{
		HUDText->SetText(FText::FromString(FString::Printf(TEXT("%s 님이 미로에서 탈출하였습니다!\n잠시후 서버가 종료됩니다!"), *PlayerName)));
		HUDText->SetVisibility(ESlateVisibility::Visible);
	}
}

void AEFM_PlayerController::Client_ExitGame_Implementation()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, true);
}


void AEFM_PlayerController::SetVisibleText(UTextBlock* TextBlock, bool bVisible)
{
	if (!TextBlock || !IsLocalController()) return;

	bVisible ? TextBlock->SetVisibility(ESlateVisibility::Visible) :
		TextBlock->SetVisibility(ESlateVisibility::Hidden);
}
