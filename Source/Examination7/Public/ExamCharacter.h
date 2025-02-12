// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "ExamCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UCapsuleComponent;
class UInputMappingContext;
class UInputAction;
class UFloatingPawnMovement;

UCLASS()
class EXAMINATION7_API AExamCharacter : public APawn
{
    GENERATED_BODY()

public:
    AExamCharacter();

    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UCapsuleComponent* CapsuleComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USkeletalMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UCameraComponent* CameraComp;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveSpeed = 600.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float LookSpeed = 50.0f;

    UPROPERTY(VisibleAnywhere)
    UFloatingPawnMovement* MovementComp;

    UFUNCTION()
    void Move(const FInputActionValue& Value);

    UFUNCTION()
    void Look(const FInputActionValue& Value);
};
