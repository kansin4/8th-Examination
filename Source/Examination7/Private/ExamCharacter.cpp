#include "ExamCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "TestPlayerController.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerController.h"

AExamCharacter::AExamCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Capsule Component (Root)
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    RootComponent = CapsuleComp;
    CapsuleComp->InitCapsuleSize(42.f, 96.f);
    CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));

    // Skeletal Mesh Component
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    MeshComp->SetupAttachment(RootComponent);

    // SpringArm Component
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.f;
    SpringArmComp->bUsePawnControlRotation = true;

    // Camera Component
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp);
    CameraComp->bUsePawnControlRotation = false;

    // Movement Component
    MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
}

void AExamCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            InputSubsystem->AddMappingContext(InputMappingContext, 0);
        }
    }
}

void AExamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent)
    {
        EnhancedInputComponent->BindAction
        (
            MoveAction, ETriggerEvent::Triggered,
            this, &AExamCharacter::Move
        );
        EnhancedInputComponent->BindAction
        (
            LookAction, ETriggerEvent::Triggered,
            this, &AExamCharacter::Look
        );
    }
}

void AExamCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller || !MovementComp) return; // 컨트롤러 또는 MovementComp가 없으면 리턴

    FVector2D MovementVector = Value.Get<FVector2D>();

    FRotator ControlRotation = Controller->GetControlRotation();
    ControlRotation.Pitch = 0.0f;
    ControlRotation.Roll = 0.0f;

    FVector Forward = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::X); // 전진 방향
    FVector Right = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);   // 오른쪽 방향

    // X축과 Y축을 바꿔서 이동 방향을 조정
	// 기존 TestCharacter와는 X축과 Y축이 반대로 되어 있어서 조정함
    FVector MoveDirection = (Forward * MovementVector.X) + (Right * MovementVector.Y);

    MovementComp->AddInputVector(MoveDirection * MoveSpeed);
}

void AExamCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookVector = Value.Get<FVector2D>();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        // 감도 조절하여 부드러운 회전 적용
        PC->AddYawInput(LookVector.X * LookSpeed * GetWorld()->GetDeltaSeconds());
        PC->AddPitchInput(LookVector.Y * LookSpeed * GetWorld()->GetDeltaSeconds()); // 상하반전 적용
    }
}