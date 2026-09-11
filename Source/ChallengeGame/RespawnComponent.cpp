// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnComponent.h"

// Sets default values
ARespawnComponent::ARespawnComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARespawnComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARespawnComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

