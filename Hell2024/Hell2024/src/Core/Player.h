#pragma once
#include "../Common.h"
#include "AnimatedGameObject.h"
#include "../Physics/Physics.h"
#include "keycodes.h"

#define GLOCK_CLIP_SIZE 12
#define GLOCK_MAX_AMMO_SIZE 200
#define AKS74U_MAG_SIZE 30
#define AKS74U_MAX_AMMO_SIZE 9999
#define SHOTGUN_AMMO_SIZE 8
#define SHOTGUN_MAX_AMMO_SIZE 9999

struct Ammo {
	int clip = 0;
	int total = 0;
};

struct Inventory {
    Ammo glockAmmo;
    Ammo aks74uAmmo;
    Ammo shotgunAmmo;
};

enum InputType {
    KEYBOARD_AND_MOUSE,
    CONTROLLER
};

enum CrosshairType {
    NONE,
    REGULAR,
    INTERACT
};

struct PlayerControls {
    unsigned int WALK_FORWARD = HELL_KEY_W;
    unsigned int WALK_BACKWARD = HELL_KEY_S;
    unsigned int WALK_LEFT = HELL_KEY_A;
    unsigned int WALK_RIGHT = HELL_KEY_D;
    unsigned int INTERACT = HELL_KEY_E;
    unsigned int RELOAD = HELL_KEY_R;
    unsigned int FIRE = HELL_MOUSE_LEFT;
    unsigned int ADS = HELL_MOUSE_RIGHT;
    unsigned int JUMP = HELL_KEY_SPACE;
    unsigned int CROUCH = HELL_KEY_WIN_CONTROL; // different mapping to the standard glfw HELL_KEY_LEFT_CONTROL
    unsigned int NEXT_WEAPON = HELL_KEY_Q;
    unsigned int MELEE = HELL_KEY_J;
    unsigned int ESCAPE = HELL_KEY_WIN_ESCAPE;
    unsigned int DEBUG_FULLSCREEN = HELL_KEY_F;
    unsigned int DEBUG_ONE = HELL_KEY_1;
    unsigned int DEBUG_TWO = HELL_KEY_2;
    unsigned int DEBUG_THREE = HELL_KEY_3;
    unsigned int DEBUG_FOUR = HELL_KEY_4;
    
};

class Player {

public:
	float _radius = 0.1f;
	bool _ignoreControl = false;
    int _killCount = 0;
    
    int _mouseIndex = -1;
    int _keyboardIndex = -1;
    InputType _inputType = KEYBOARD_AND_MOUSE;
    PlayerControls _controls;

	PhysXRayResult _cameraRayResult;

	//RayCastResult _cameraRayData;
	AnimatedGameObject _characterModel;
	PxController* _characterController = NULL;

	PxShape* _itemPickupOverlapShape = NULL;
	//PxRigidStatic* _itemPickupOverlapDebugBody = NULL;
    float _yVelocity = 0;
    Transform _weaponSwayTransform;

	Inventory _inventory;

	Player();

	int GetCurrentWeaponClipAmmo();
	int GetCurrentWeaponTotalAmmo(); 
    void SetPosition(glm::vec3 position);
    void RespawnAtCurrentPosition();

    bool _glockSlideNeedsToBeOut = false;
    bool _needsShotgunFirstShellAdded = false;
    bool _needsShotgunSecondShellAdded = false; 

    int _health = 100;
    float _damageColorTimer = 1.0f;
    float _outsideDamageTimer = 0;
    float _outsideDamageAudioTimer = 0;

    void DrawWeapons();
    void GiveDamageColor();


	//void Init(glm::vec3 position);
	void Update(float deltaTime);
    void UpdateRagdoll();

	void SetRotation(glm::vec3 rotation);
	void SetWeapon(Weapon weapon);
	void Respawn();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetInverseViewMatrix();
	glm::vec3 GetViewPos();
	glm::vec3 GetViewRotation();
	glm::vec3 GetFeetPosition();
	glm::vec3 GetCameraRight();
	glm::vec3 GetCameraForward();
	glm::vec3 GetCameraUp();
	bool IsMoving();
	int GetCurrentWeaponIndex();
	void UpdateFirstPersonWeaponLogicAndAnimations(float deltaTime);
	AnimatedGameObject& GetFirstPersonWeapon();
	void SpawnMuzzleFlash();
    void SpawnGlockCasing();
    void SpawnAKS74UCasing();
    void SpawnShotgunShell();
	float GetMuzzleFlashTime();
	float GetMuzzleFlashRotation();
	float GetRadius();
	void CreateCharacterController(glm::vec3 position);
	//void WipeYVelocityToZeroIfHeadHitCeiling();
	PxShape* GetCharacterControllerShape();
	PxRigidDynamic* GetCharacterControllerActor();
	void CreateItemPickupOverlapShape();
	PxShape* GetItemPickupOverlapShape();

	void ShowPickUpText(std::string text);
    void PickUpAKS74U();
    void PickUpAKS74UAmmo();
    void PickUpShotgunAmmo();
    void PickUpGlockAmmo();
	void CastMouseRay();
	void DropAKS7UMag();
    void CheckForKnifeHit();

    void SetGlockAnimatedModelSettings();

	//ShadowMap _shadowMap;
	float _muzzleFlashCounter = 0;

	bool MuzzleFlashIsRequired();
	glm::mat4 GetWeaponSwayMatrix();
    WeaponAction& GetWeaponAction();

    glm::vec3 GetGlockBarrelPosition();

	bool _isGrounded = true;

    void PickUpShotgun();

    glm::mat4 GetProjectionMatrix();

    bool CanEnterADS();
    bool InADS();

	std::string _pickUpText = "";
	float _pickUpTextTimer = 0;
    float _zoom = 1.0f;

    bool PressingWalkForward();
    bool PressingWalkBackward();
    bool PressingWalkLeft();
    bool PressingWalkRight();
    bool PressingCrouch();
    bool PressedWalkForward();
    bool PressedWalkBackward();
    bool PressedWalkLeft();
    bool PressedWalkRight();
    bool PressedInteract();
    bool PressedReload();
    bool PressedFire();
    bool PressingFire();
    bool PresingJump();
    bool PressedCrouch();
    bool PressedNextWeapon();
    bool PressingADS();
    bool PressedADS();
    bool PressedEscape();
    //bool PressedWindowsEnter();

    // Dev keys
    bool PressedFullscreen();
    bool PressedOne();
    bool PressedTwo();
    bool PressedThree();
    bool PressedFour();

    glm::vec3 GetCameraRotation();
    void GiveAKS74UScope();
    bool _hasAKS74UScope = false;

    void HideKnifeMesh();
    void HideGlockMesh();
    void HideShotgunMesh();
    void HideAKS74UMesh();
    void Kill();
    void PickUpGlock();
    PxU32 _interactFlags;
    PxU32 _bulletFlags;
    std::string _playerName;
    bool _isDead = false;
    glm::vec3 _movementVector = glm::vec3(0);
    float _timeSinceDeath = 0;
    bool _isOutside = false;
    bool _hasGlockSilencer = false;

    float _currentSpeed = 0.0f;

    void ForceSetViewMatrix(glm::mat4 viewMatrix);
    std::vector<RenderItem2D> GetHudRenderItems(ivec2 viewportSize);
    std::vector<RenderItem2D> GetHudRenderItemsHiRes(ivec2 viewportSize);
    CrosshairType GetCrosshairType();

    bool IsDead();
    bool IsAlive();
    bool RespawnAllowed();

private:

    glm::vec3 _displacement;

	void Interact();
	void SpawnBullet(float variance, Weapon type);
	bool CanFire();
	bool CanReload();
	void CheckForItemPickOverlaps();

	glm::mat4 _weaponSwayMatrix = glm::mat4(1);
	bool _needsToDropAKMag = false;

    float _footstepAudioTimer = 0;
    float _footstepAudioLoopLength = 0.5;

	glm::vec3 _position = glm::vec3(0);
	glm::vec3 _rotation = glm::vec3(-0.1f, -HELL_PI * 0.5f, 0);
	float _viewHeightStanding = 1.65f;
	float _viewHeightCrouching = 1.15f;
	float _crouchDownSpeed = 17.5f;
	float _currentViewHeight = _viewHeightStanding;
	float _walkingSpeed = 4.85f;
	float _crouchingSpeed = 2.325f;
	glm::mat4 _viewMatrix = glm::mat4(1);
	glm::mat4 _inverseViewMatrix = glm::mat4(1);
	glm::vec3 _viewPos = glm::vec3(0);
	glm::vec3 _forward = glm::vec3(0);
	glm::vec3 _up = glm::vec3(0);
	glm::vec3 _right = glm::vec3(0);

	bool _isMoving = false;
	float _muzzleFlashTimer = -1;
	float _muzzleFlashRotation = 0;
	int _currentWeaponIndex = 0;
	AnimatedGameObject _firstPersonWeapon;
	WeaponAction _weaponAction = DRAW_BEGIN;
	std::vector<bool> _weaponInventory;
	bool _needsRespawning = true;
	glm::vec2 _weaponSwayFactor = glm::vec2(0);
	glm::vec3 _weaponSwayTargetPos = glm::vec3(0);
	bool _needsAmmoReloaded = false;
    
};