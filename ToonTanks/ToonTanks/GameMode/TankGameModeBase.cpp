// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameModeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "ToonTanks/PlayerController/PlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"

void ATankGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawnTurret::StaticClass(), TurretActors);
    TargetTurrets = TurretActors.Num();

    PlayerControllerRef = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));

    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));

    HandleGameStart();
}

void ATankGameModeBase::HandleGameStart()
{
    GameStart();

    if(PlayerControllerRef)
    {
        PlayerControllerRef->SetPlayerEnabledState(false);

        FTimerHandle PlayerEnableHandle;
        FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerRef, &APlayerControllerBase::SetPlayerEnabledState, true);
        GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, 3.0f, false);
    }
}

void ATankGameModeBase::HandleGameOver(bool PlayerWon)
{
    GameOver(PlayerWon);
}

void ATankGameModeBase::ActorDied(AActor* DeadActor)
{
    if(DeadActor == PlayerTank)
    {
        PlayerTank->HandleDestruction();
        HandleGameOver(false);

        if(PlayerControllerRef)
        {
            PlayerControllerRef->SetPlayerEnabledState(false);
        }
    }
    else if(APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))
    {
        DestroyedTurret->HandleDestruction();

        if(--TargetTurrets == 0)
        {
            HandleGameOver(true);
        }
    }
}

