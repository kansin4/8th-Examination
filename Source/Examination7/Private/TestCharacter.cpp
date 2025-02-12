#include "TestCharacter.h"
#include "EnhancedInputComponent.h"
#include "TestPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "TestGameBase.h"
#include "Components/TextBlock.h"

ATestCharacter::ATestCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    // 카메라는 스프링 암의 회전을 따르므로 PawnControlRotation은 꺼둠
    CameraComp->bUsePawnControlRotation = false;

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(GetMesh());
    OverHeadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
    Health = MaxHealth;
}

float ATestCharacter::GetHealth() const
{
    return Health;
}
void ATestCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    UpdateOverHeadHp();
}
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();
    UpdateOverHeadHp();
}
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (ATestPlayerController* PlayerController = Cast<ATestPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->MoveAction,
                    ETriggerEvent::Triggered,
                    this,
                    &ATestCharacter::Move
                );
            }

            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction,
                    ETriggerEvent::Triggered,
                    this, &ATestCharacter::StartJump);

                EnhancedInput->BindAction(PlayerController->JumpAction,
                    ETriggerEvent::Completed,
                    this,
                    &ATestCharacter::StopJump);
            }

            if (PlayerController->LookAction)
            {
                // IA_Look 액션 마우스가 "움직일 때" Look() 호출
                EnhancedInput->BindAction(
                    PlayerController->LookAction,
                    ETriggerEvent::Triggered,
                    this,
                    &ATestCharacter::Look
                );
            }

            if (PlayerController->SprintAction)
            {
                // IA_Sprint 액션 키를 "누르고 있는 동안" StartSprint() 호출
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Triggered,
                    this,
                    &ATestCharacter::StartSprint
                );
                // IA_Sprint 액션 키에서 "손을 뗀 순간" StopSprint() 호출
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Completed,
                    this,
                    &ATestCharacter::StopSprint
                );
            }
        }
    }
}

float ATestCharacter::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - ActualDamage, 0.0f, MaxHealth); // Clamp함수 : 최소값과 최대값을 설정하여 그 사이의 값만 반환
    UpdateOverHeadHp();

    if (Health <= 0.0f)
    {
        OnDeath();
    }

    return ActualDamage;
}

void ATestCharacter::OnDeath()
{
	ATestGameBase* TestGameBase = GetWorld() ? GetWorld()->GetGameState<ATestGameBase>() : nullptr;
	if (TestGameBase)
	{
		TestGameBase->OnGameOver();
	}
}


void ATestCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return;


    const FVector2D MoveInput = value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
    }
}

void ATestCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ATestCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ATestCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ATestCharacter::StartSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

void ATestCharacter::StopSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}


void ATestCharacter::UpdateOverHeadHp()
{
    if (!OverHeadWidget) return;

    UUserWidget* OverHeadWidgetInstance = OverHeadWidget->GetUserWidgetObject();
    if (!OverHeadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverHeadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHp"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}

}
