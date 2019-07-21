// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile0.h"

// Sets default values
AProjectile0::AProjectile0()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectile0::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile0::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

