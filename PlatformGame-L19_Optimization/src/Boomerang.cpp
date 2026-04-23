#include "Boomerang.h"
#include "Player.h"
#include "Eosinofilo.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"
#include <cmath>

Boomerang::Boomerang() : Entity(EntityType::UNKNOWN)
{
	name = "Boomerang";
}

Boomerang::~Boomerang()
{
}

bool Boomerang::Awake()
{
	return true;
}

bool Boomerang::Start()
{
	std::unordered_map<int, std::string> aliases = {
		{0, "move"}
	};

	anims.LoadFromTSX("Assets/Textures/Characters/Boomerang.tsx", aliases);
	anims.SetCurrent("move");
	anims.Func_SetAnimationLoop("move", true);

	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Boomerang.png");

	texW = 256;
	texH = 256;

	lifeTimer.Start();

	pbody = Engine::GetInstance().physics->CreateCircle(
		(int)position.getX(),
		(int)position.getY(),
		20,
		bodyType::KINEMATIC
	);

	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	return true;
}

bool Boomerang::Update(float dt)
{
	anims.Update(dt);

	if (currentState == BoomerangState::OUTGOING)
	{
		if (!hasHitPlayer && lifeTimer.ReadMSec() >= outgoingDurationMs)
		{
			StartReturning();
		}
	}
	else if (currentState == BoomerangState::RETURNING)
	{
		if (owner != nullptr)
		{
			Vector2D ownerPosition = owner->GetPosition();
			Vector2D toOwner(
				ownerPosition.getX() - position.getX(),
				ownerPosition.getY() - position.getY()
			);

			float length = std::sqrt(toOwner.getX() * toOwner.getX() + toOwner.getY() * toOwner.getY());

			if (length > 0.0f)
			{
				direction.setX(toOwner.getX() / length);
				direction.setY(toOwner.getY() / length);
			}

			facingRight = direction.getX() >= 0.0f;

			if (length <= catchDistance)
			{
				owner->OnBoomerangReturned();
				Destroy();
				return true;
			}
		}
	}

	Move();
	ApplyPhysics();

	if (lifeTimer.ReadMSec() >= maxLifeDurationMs)
	{
		if (owner != nullptr && !hasHitPlayer)
		{
			owner->OnBoomerangLost();
		}

		Destroy();
		return true;
	}

	Draw(dt);
	return true;
}

void Boomerang::SetSpawnPosition(const Vector2D& pos)
{
	position = pos;
	spawnPosition = pos;

	if (pbody != nullptr)
	{
		pbody->SetPosition((int)pos.getX(), (int)pos.getY());
	}
}

void Boomerang::SetDirection(const Vector2D& dir)
{
	direction = dir;

	float length = std::sqrt(direction.getX() * direction.getX() + direction.getY() * direction.getY());

	if (length > 0.0f)
	{
		direction.setX(direction.getX() / length);
		direction.setY(direction.getY() / length);
	}

	facingRight = direction.getX() >= 0.0f;
}

void Boomerang::SetOwner(Eosinofilo* eosOwner)
{
	owner = eosOwner;
}

void Boomerang::StartReturning()
{
	currentState = BoomerangState::RETURNING;
}

void Boomerang::ApplyPhysics()
{
	if (pbody == nullptr)
	{
		return;
	}

	float elapsedMs = (float)lifeTimer.ReadMSec();

	float currentSpeed = speed;
	if (currentState == BoomerangState::RETURNING)
	{
		currentSpeed *= returnSpeedMultiplier;
	}

	b2Vec2 velocity;
	velocity.x = direction.getX() * currentSpeed;

	if (currentState == BoomerangState::OUTGOING)
	{
		velocity.y = std::sin(elapsedMs * curveFrequency) * 2.5f;
	}
	else
	{
		velocity.y = direction.getY() * currentSpeed;
	}

	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);

	int x, y;
	pbody->GetPosition(x, y);

	position.setX((float)x);

	if (currentState == BoomerangState::OUTGOING)
	{
		float curvedOffsetY = std::sin(elapsedMs * curveFrequency) * curveAmplitude;
		position.setY(spawnPosition.getY() + curvedOffsetY);
	}
	else
	{
		position.setY((float)y);
	}
}

void Boomerang::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
	{
		if (hasHitPlayer || currentState == BoomerangState::RETURNING)
		{
			break;
		}

		hasHitPlayer = true;

		Player* player = dynamic_cast<Player*>(physB->listener);
		if (player != nullptr && player->pbody != nullptr)
		{
			float impulseX = facingRight ? knockbackForceX : -knockbackForceX;
			float impulseY = knockbackForceY;

			Engine::GetInstance().physics->ApplyLinearImpulseToCenter(
				player->pbody,
				impulseX,
				impulseY,
				true
			);

			// SAME DAMAGE AS EOSINOFILO:
			// put here the exact same call you already use in Eosinofilo vs Player
		}

		if (owner != nullptr)
		{
			owner->OnBoomerangLost();
		}

		LOG("Boomerang hit PLAYER");
		Destroy();
		break;
	}

	case ColliderType::PLATFORM:
		// Do nothing: the boomerang should still be able to return
		break;

	default:
		break;
	}
}

void Boomerang::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
}

void Boomerang::Move()
{
	if (direction.getX() > 0.0f)
	{
		facingRight = true;
	}
	else if (direction.getX() < 0.0f)
	{
		facingRight = false;
	}
}

void Boomerang::Draw(float dt)
{
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int frameW = animFrame.w;
	int frameH = animFrame.h;

	int drawX = (int)position.getX() - (frameW / 2);
	int drawY = (int)position.getY() - (frameH / 2);

	if (facingRight)
	{
		Engine::GetInstance().render->DrawTexture(
			texture,
			drawX,
			drawY,
			&animFrame,
			1.0f,
			0.0,
			frameW / 2,
			frameH / 2,
			SDL_FLIP_NONE,
			1.0f
		);
	}
	else
	{
		Engine::GetInstance().render->DrawTexture(
			texture,
			drawX,
			drawY,
			&animFrame,
			1.0f,
			0.0,
			frameW / 2,
			frameH / 2,
			SDL_FLIP_HORIZONTAL,
			1.0f
		);
	}
}

bool Boomerang::CleanUp()
{
	if (texture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}

	if (pbody != nullptr)
	{
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	return true;
}

bool Boomerang::Destroy()
{
	active = false;
	pendingToDelete = true;
	return true;
}