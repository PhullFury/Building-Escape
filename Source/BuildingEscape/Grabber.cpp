// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	CheckPhysicsHandle();
	SetupInputComponent();
}

void UGrabber::Grab()
{//Gets Player's viewpiont
	FVector PlayerViewPointLocation;
	FRotator PlayerVeiwPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerVeiwPointRotation
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerVeiwPointRotation.Vector() * Reach;

	UE_LOG(LogTemp, Warning, TEXT("Grabber pressed!"));

	FHitResult HitResult = GetFirstPhysicsBodyInreach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	if (HitResult.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, LineTraceEnd);
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber released!"));

	if (HitResult.GetActor())
	{
		PhysicsHandle->ReleaseComponent();
	}
}

void UGrabber::CheckPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		//PhysicsHandle found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("The PhysicsHandle component is refrenced but not attached to: %s"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Gets Player's viewpiont
	FVector PlayerViewPointLocation;
	FRotator PlayerVeiwPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerVeiwPointRotation
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerVeiwPointRotation.Vector() * Reach;

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}	
}

FHitResult UGrabber::GetFirstPhysicsBodyInreach() const
{
	//Gets Player's viewpiont
	FVector PlayerViewPointLocation;
	FRotator PlayerVeiwPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerVeiwPointRotation
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerVeiwPointRotation.Vector() * Reach;

	//Ray Cast out to a certain distance (Reach)
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	//See what it hits
	AActor* ActorHit = Hit.GetActor();

	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor being hit is: %s"), *(ActorHit->GetName()));
	}

	return Hit;
}