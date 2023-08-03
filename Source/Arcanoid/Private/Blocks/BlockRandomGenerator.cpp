// Fill out your copyright notice in the Description page of Project Settings.


#include "Blocks/BlockRandomGenerator.h"

#include "Arcanoid/Public/utils.h"
#include "Blocks/BlockActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABlockRandomGenerator::ABlockRandomGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
}

void ABlockRandomGenerator::BeginPlay()
{
	Super::BeginPlay();

	Server_GenerateBlockGrid();
}

void ABlockRandomGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlockRandomGenerator, Rows);
	DOREPLIFETIME(ABlockRandomGenerator, Columns);
	DOREPLIFETIME_CONDITION(ABlockRandomGenerator, bIsExtra, COND_SimulatedOnly);
}

void ABlockRandomGenerator::Server_ClearGenerate_Implementation()
{
	if(!GetWorld()) return;
	
	TArray<ABlockActor*> FindAllBlocks;
	utils::FindAllActors(GetWorld(), FindAllBlocks);
	for (auto FindAllBlock : FindAllBlocks)
	{
		FindAllBlock->Destroy();
	}
}

bool ABlockRandomGenerator::Server_ClearGenerate_Validate()
{
	return true;
}

void ABlockRandomGenerator::Client_SetExtras_Implementation(AActor* ExtrasActor)
{
	if(!ExtrasActor && !ExtraScoreMaterial) return;
	
	const auto Actor = Cast<ABlockActor>(ExtrasActor);
	Actor->SetScoreValue(3.0f);
	Actor->Multicast_SetExtraMaterial(ExtraScoreMaterial);
}

void ABlockRandomGenerator::Server_GenerateBlockGrid_Implementation()
{
	if(!GetWorld() || !BlockClass) return;
	
	Columns = FMath::RandRange(3, 12);

	UE_LOG(LogTemp, Display, TEXT("Columns number = %i"), Columns);

	if (BlockClass)
	{
		for (int Y = 0; Y != Rows; ++Y)
		{
			for (int X = 0; X != Columns; ++X)
			{
				const auto ActorLocationY = GetActorLocation().Y;
				float BlockPositionOffsetY = 160.0f;

				if (ActorLocationY > 0.0f)
				{
					BlockPositionOffsetY *= -1.0f;
				}
				
				const auto LocationY = ActorLocationY + (Y * BlockPositionOffsetY);

				const auto ActorLocationX = GetActorLocation().X;
				float BlockPositionOffsetX = 70.0f;
				
				if (ActorLocationX < 0.0f)
				{
					BlockPositionOffsetX *= -1.0f;
				}
				
				const auto LocationX = ActorLocationX + (X * BlockPositionOffsetX);
				
				FVector SpawnLocation = FVector(LocationX, LocationY, GetActorLocation().Z);
				FRotator SpawnRotation = RootComponent->GetRelativeRotation();
				
				bool bRandGenerator = FMath::RandBool();
				if (bRandGenerator) 
				{
					BlockActor = GetWorld()->SpawnActor<ABlockActor>(BlockClass, SpawnLocation, SpawnRotation);
					
					bIsExtra = FMath::RandBool();
					if (bIsExtra)
					{
						Client_SetExtras(BlockActor);

						bIsStopBallExtra = FMath::RandBool();
						if(bIsStopBallExtra)
						{
							BlockActor->Multicast_SetExtraMaterial(ExtraStopBallMaterial);
							BlockActor->SetIsStopBall(bIsStopBallExtra);
							BlockActor->SetScoreValue(2.0f);
						}
					}
					
				}
			}
		}
	}
}

bool ABlockRandomGenerator::Server_GenerateBlockGrid_Validate()
{
	return true;
}


