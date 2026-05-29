#include "ProyectilesVirusBasico.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Physics.h"
#include "Log.h"
#include "CelulaBasica.h"
#include <cmath>

ProyectilesVirusBasico::ProyectilesVirusBasico() : Entity(EntityType::PROJECTILE_VIRUS_BASICO)
{
	name = "ProyectilesVirusBasico";
}

ProyectilesVirusBasico::~ProyectilesVirusBasico()
{
}

bool ProyectilesVirusBasico::Awake()
{
	return true;
}

bool ProyectilesVirusBasico::Start()
{
	std::unordered_map<int, std::string> aliases = {
	{0, "attack1"},
	{3, "hitAttack1"},
	{8, "attack2"},
	{11, "hitGolpe2"},
	{16, "parasitar"}
	};

	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Virus_Basico_Proyectiles.tsx", aliases);
	anims.Func_SetAnimationLoop("hitGolpe2", false);
	anims.Func_SetAnimationLoop("hitAttack1", false);

	if (projectileType == 1)
	{
		anims.SetCurrent("attack1");
		damage = 1;             
		speed = 9.0f;
		texW = 48;
		texH = 48;
		maxLifeDurationMs = 900.0f;
	}
	else if (projectileType == 2)
	{
		anims.SetCurrent("attack2");
		damage = 3;
		speed = 5.5f;
		texW = 80;
		texH = 80;
	}
	else
	{
		anims.SetCurrent("parasitar");
		damage = 0;
		speed = 4.0f;
		texW = 64;
		texH = 64;
	}

	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_Virus_Basico_Proyectiles.png");

	pbody = Engine::GetInstance().physics->CreateCircle(
		(int)spawnPosition.getX(),
		(int)spawnPosition.getY(),
		texW / 2,
		bodyType::DYNAMIC
	);

	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	b2Body_SetGravityScale(pbody->body, 0.0f);

	float length = std::sqrt(direction.getX() * direction.getX() + direction.getY() * direction.getY());

	if (length > 0.0f)
	{
		direction.setX(direction.getX() / length);
		direction.setY(direction.getY() / length);
	}

	facingRight = direction.getX() >= 0.0f;

	lifeTimer.Start();

	return true;
}

bool ProyectilesVirusBasico::Update(float dt)
{
	anims.Update(dt);

	if (hasHit)
	{
		if (anims.Func_HasCurrentAnimationFinished())
		{
			Destroy();
		}
		Draw(dt);
		return true;
	}

	if (lifeTimer.ReadMSec() >= maxLifeDurationMs)
	{
		Destroy();
		return true;
	}

	ApplyPhysics();
	Draw(dt);

	return true;
}

void ProyectilesVirusBasico::ApplyPhysics()
{
	if (pbody == nullptr) return;

	b2Vec2 velocity;
	velocity.x = direction.getX() * speed;
	velocity.y = direction.getY() * speed;

	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void ProyectilesVirusBasico::Draw(float dt)
{
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int x, y;
	pbody->GetPosition(x, y);

	int frameW = animFrame.w;
	int frameH = animFrame.h;

	int drawX = x - frameW / 2;
	int drawY = y - frameH / 2;

	Engine::GetInstance().render->DrawTexture(
		texture,
		drawX,
		drawY,
		&animFrame,
		1.0f,
		0.0,
		frameW / 2,
		frameH / 2,
		facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE, 
		1.0f
	);
}

bool ProyectilesVirusBasico::CleanUp()
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

bool ProyectilesVirusBasico::Destroy()
{
	active = false;
	pendingToDelete = true;
	return true;
}

void ProyectilesVirusBasico::SetSpawnPosition(const Vector2D& pos)
{
	spawnPosition = pos;
}

void ProyectilesVirusBasico::SetDirection(const Vector2D& dir)
{
	direction = dir;
}

void ProyectilesVirusBasico::SetProjectileType(int type)
{
	projectileType = type;
}

void ProyectilesVirusBasico::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::UNKNOWN:
	{
		if (projectileType == 3)
		{
			CelulaBasica* cell = dynamic_cast<CelulaBasica*>(physB->listener);

			if (cell != nullptr)
			{
				cell->Parasitize();
				Destroy();
			}
		}
		break;
	}

	case ColliderType::CELL:
	{
		CelulaBasica* cell = dynamic_cast<CelulaBasica*>(physB->listener);

		if (cell != nullptr)
		{
			if (projectileType == 3)
			{
				cell->Parasitize();
			}
			else
			{
				cell->TakeDamage(damage);
			}

			Destroy();
		}

		break;
	}

	case ColliderType::PLAYER:
	{
		Player* player = dynamic_cast<Player*>(physB->listener);

		if (player != nullptr && !player->IsGodMode())
		{
			player->playerCurrentHp -= damage;

			if (player->playerCurrentHp < 0)
			{
				player->playerCurrentHp = 0;
			}
		}

		hasHit = true;

		if (projectileType == 1)
		{
			anims.SetCurrent("hitAttack1");
		}
		else if (projectileType == 2)
		{
			anims.SetCurrent("hitGolpe2");
		}

		Engine::GetInstance().physics->SetLinearVelocity(pbody, b2Vec2_zero);
		break;

	}

	case ColliderType::PLATFORM:
		Destroy();
		break;

	default:
		break;
	}
}