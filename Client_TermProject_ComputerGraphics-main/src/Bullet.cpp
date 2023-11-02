#include "stdafx.h"
#include "Bullet.h"
#include "Timer.h"
#include "Sound.h"

#define BULLET_RADIUS 10.0f

extern SoundManager* soundManager;
extern BulletManager* bulletManager;

GLvoid IBulletCollisionable::Destroy()
{
	extern BulletManager* bulletManager;
	mIsDestroyed = GL_TRUE;
	bulletManager->DelCollisionObject(this);
};


Bullet::Bullet(const BulletData& data, const glm::vec3& origin, const glm::vec3& position, const GLfloat& yaw, const GLfloat& pitch) : SharedObject()
{
	mType = data.type;
	mWeight = data.weight;
	mVelocity = data.velocity;
	mDamage= data.damage;
	SetScale(data.scale);
	SetColor(data.color);

	SharedObject::Init(GetIdentityModelObject(data.model));

	GLfloat resultYaw = yaw + rand() % (mSpreadAmount*2) - mSpreadAmount;
	GLfloat resultPitch = pitch + rand() % (mSpreadAmount*2) - mSpreadAmount;

	RotateLocal(0, resultPitch, 0);

	mPrevPos = origin;
	mPosition = position;
	mPosition.x += rand() % 2 - 1;
	mPosition.y += rand() % 2 - 1;

	mAngleY = sin(DEGREE_TO_RADIAN(resultYaw));
	mAngleZ = cos(DEGREE_TO_RADIAN(resultYaw));

}
Bullet::~Bullet()
{
	
}

GLvoid Bullet::Update()
{
	mPrevPos = GetTransformedPos();

	/* https://www.101computing.net/projectile-motion-formula/ */
	mT += timer::DeltaTime();
	MoveZ(mVelocity * mAngleZ);
	MoveY(mVelocity * mAngleY - (0.5f * GRAVITY * mT * mT * mWeight));
}
BulletAtt Bullet::GetAttribute() const
{
	BulletAtt result;
	result.prevPos = mPrevPos;
	result.crntPos = GetTransformedPos();
	result.radius = BULLET_RADIUS * GetScale().x;
	result.damage = mDamage;

	return result;
}
COLORREF Bullet::GetColor() const
{
	return ShaderObject::GetColor();
}
















BulletManager::BulletManager()
{
	mBulletList.reserve(1000);
}
BulletManager::~BulletManager()
{
	for (Bullet* bullet : mBulletList)
	{
		delete bullet;
	}
}

GLvoid BulletManager::Create(const BulletData& data, const glm::vec3& origin, const glm::vec3& position, const GLfloat& yaw, const GLfloat& pitch)
{
	Bullet* bullet = new Bullet(data, origin, position, yaw, pitch);
	if (data.type == BulletType::Particle_Explosion)
	{
		mParticles.emplace_back(bullet);
	}
	else
	{
		mBulletList.emplace_back(bullet);
	}
}
GLvoid BulletManager::CreateExplosion(const COLORREF& color, const glm::vec3& position, const GLfloat& radius, const GLint& amount)
{
	constexpr GLfloat particleVelocity = 150.0f;

	glm::vec3 origin = position;
	const GLint r = static_cast<GLint>(radius);

	for (GLint i = 0; i < amount; ++i)
	{
		GLfloat yaw = static_cast<GLfloat>(rand() % 180) - 90.0f;
		GLfloat pitch = static_cast<GLfloat>(rand() % 360);
		glm::vec3 pos = position;

		
		pos.x += rand() % (r * 2) - r;
		pos.y += rand() % (r * 2) - r;
		pos.z += rand() % (r * 2) - r;

		BulletData data;
		data.type = BulletType::Particle_Explosion;
		data.weight = 100.0f;
		data.damage = 0.0f;
		data.scale = 0.1f;
		data.velocity = particleVelocity;
		data.color = color;
		data.model = Models::LowSphere;

		Create(data, origin, pos, yaw, pitch);
	}
}

GLvoid BulletManager::Draw() const
{
	for (const Bullet* bullet : mBulletList)
	{
		bullet->Draw();
	}
	for (const Bullet* particle : mParticles)
	{
		particle->Draw();
	}
}


GLboolean ProcessCollision(Bullet* bullet, IBulletCollisionable* object, vector<PaintPlane*>& paints, GLfloat& crntInkSoundDelay)
{
	constexpr GLfloat inkSoundDelay = 0.5f;
	constexpr GLfloat NO_NORMAL = 9;

	glm::vec3 hitPoint;
	glm::vec3 normal = { NO_NORMAL, NO_NORMAL, NO_NORMAL };

	if (object->CheckCollisionBullet(bullet->GetAttribute(), hitPoint, normal) == GL_TRUE)
	{
		/* create paint */
		if (normal.x != NO_NORMAL)
		{
			GLuint randPaint = rand() % NUM_PAINT;
			Textures texture = static_cast<Textures>(static_cast<GLuint>(Textures::Paint) + randPaint);
			const IdentityObject* object = GetIdentityTextureObject(texture);

			if (bullet->GetType() != BulletType::Rocket)
			{
				if (crntInkSoundDelay >= inkSoundDelay)
				{
					crntInkSoundDelay = 0;
					soundManager->PlayEffectSound(EffectSound::Drawing_ink, hitPoint, 0.2f);
				}
			}
			else
			{
				soundManager->PlayEffectSound(EffectSound::Drawing_Bigink, hitPoint, 0.2f);
			}
			PaintPlane* plane = new PaintPlane(object, bullet->GetColor(), hitPoint, normal);
			plane->SetScale(BULLET_RADIUS * bullet->GetScale());
			paints.emplace_back(plane);
		}

		if (bullet->GetType() == BulletType::Rocket)
		{
			bulletManager->CreateExplosion(RED, bullet->GetCenterPos(), bullet->GetRadius());
		}

		bullet->Destroy();
		return GL_TRUE;
	}

	return GL_FALSE;
}
GLvoid BulletManager::Update()
{
	mCrntInkSoundDelay += timer::DeltaTime();

	for (auto iter = mBulletList.begin(); iter != mBulletList.end();)
	{
		Bullet* bullet = (*iter);

		for (auto iter2 = mCollisionObjectList.begin(); iter2 != mCollisionObjectList.end(); ++iter2)
		{
			IBulletCollisionable* object = *iter2;

			size_t beforeSize = mCollisionObjectList.size();
			if (ProcessCollision(bullet, object, mPaints, mCrntInkSoundDelay) == GL_TRUE)
			{
				break;
			}
			size_t afterSize = mCollisionObjectList.size();
			if (beforeSize < afterSize)
			{
				--iter2;
			}
		}

		if (bullet->IsDestroyed())
		{
			iter = mBulletList.erase(iter);
		}
		else
		{
			bullet->Update();
			++iter;
		}
	}

	for (auto iter = mParticles.begin(); iter != mParticles.end();)
	{
		Bullet* bullet = (*iter);

		for (IBulletCollisionable* object : mParticleCollisions)
		{
			if (ProcessCollision(bullet, object, mPaints, mCrntInkSoundDelay) == GL_TRUE)
			{
				break;
			}
		}

		if (bullet->IsDestroyed())
		{
			iter = mParticles.erase(iter);
		}
		else
		{
			bullet->Update();
			++iter;
		}
	}

	for (auto iter = mPaints.begin(); iter != mPaints.end();)
	{
		PaintPlane* paint = *iter;
		if (paint->Update() == GL_FALSE)
		{
			delete paint;
			iter = mPaints.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}
//GLvoid ProcessCollision(Bullet* bullet, IBulletCollisionable* object, vector<PaintPlane*>& paints)
//{
//	constexpr GLfloat NO_NORMAL = 9;
//
//	glm::vec3 hitPoint;
//	glm::vec3 normal = { NO_NORMAL, NO_NORMAL, NO_NORMAL };
//
//	if (object->CheckCollisionBullet(bullet->GetAttribute(), hitPoint, normal) == GL_TRUE)
//	{
//		/* create paint */
//		if (normal.x != NO_NORMAL)
//		{
//			GLuint randPaint = rand() % NUM_PAINT;
//			Textures texture = static_cast<Textures>(static_cast<GLuint>(Textures::Paint) + randPaint);
//			const IdentityObject* object = GetIdentityTextureObject(texture);
//			if (bullet->GetType() != BulletType::Rocket)
//			{
//				soundManager->PlayEffectSound(EffectSound::Drawing_ink, 0.05f, GL_TRUE);
//			}
//			else
//			{
//				soundManager->PlayEffectSound(EffectSound::Drawing_Bigink, 0.05f, GL_TRUE);
//			}
//			PaintPlane* plane = new PaintPlane(object, bullet->GetColor(), hitPoint, normal);
//			plane->SetScale(BULLET_RADIUS * bullet->GetScale());
//			paints.emplace_back(plane);
//		}
//
//		if (bullet->GetType() == BulletType::Rocket)
//		{
//			bulletManager->CreateExplosion(RED, bullet->GetCenterPos(), bullet->GetRadius());
//		}
//
//		bullet->Destroy();
//	}
//}
//GLvoid Thread_BulletCollision(Bullet* bullet, const vector<IBulletCollisionable*>& obectList, const GLuint& begin, const GLuint& end, vector<PaintPlane*>& paints)
//{
//	if (bullet == nullptr)
//	{
//		return;
//	}
//
//
//	for (auto iter = obectList.begin() + begin; iter != obectList.begin() + end; ++iter)
//	{
//		IBulletCollisionable* object = *iter;
//
//		ProcessCollision(bullet, object, paints);
//		if (bullet->IsDestroyed())
//		{
//			return;
//		}
//	}
//}
//GLvoid Single_BulletCollision(Bullet* bullet, const vector<IBulletCollisionable*>& obectList, vector<Bullet*>& bulletList, vector<Bullet*>::iterator& iter, vector<PaintPlane*>& paints)
//{
//	for (IBulletCollisionable* object : obectList)
//	{
//		ProcessCollision(bullet, object, paints);
//		if (bullet->IsDestroyed())
//		{
//			break;
//		}
//	}
//
//	if (bullet->IsDestroyed())
//	{
//		delete bullet;
//		iter = bulletList.erase(iter);
//	}
//	else
//	{
//		bullet->Update();
//		++iter;
//	}
//}
//
//GLvoid BulletManager::Update()
//{
//	constexpr GLfloat NO_NORMAL = 9;
//
//	if (mCollisionObjectList.size() < NUM_CORE)
//	{
//		for (auto iter = mBulletList.begin(); iter != mBulletList.end();)
//		{
//			Bullet* bullet = (*iter);
//
//			Single_BulletCollision(bullet, mCollisionObjectList, mBulletList, iter, mPaints);
//		}
//	}
//	else
//	{
//		vector<thread*> threads;
//		threads.resize(NUM_CORE);
//		vector<PaintPlane*>* paints = new vector<PaintPlane*>[NUM_CORE];
//
//		timer::StartRecord();
//		for (auto iter = mBulletList.begin(); iter != mBulletList.end();)
//		{
//			Bullet* bullet = (*iter);
//			GLuint begin = 0;
//			GLuint end = 0;
//
//			GLuint size = mCollisionObjectList.size();
//
//			if (size < NUM_CORE)
//			{
//				Single_BulletCollision(bullet, mCollisionObjectList, mBulletList, iter, mPaints);
//				continue;
//			}
//
//			GLuint split = size / NUM_CORE;
//			GLuint lastSplit = size - (NUM_CORE * split) + 1;
//
//			for (GLuint i = 0; i < NUM_CORE - 1; ++i)
//			{
//				begin = i * split;
//				end = begin + (split - 1);
//
//				threads[i] = new thread(Thread_BulletCollision, bullet, ref(mCollisionObjectList), begin, end, ref(paints[i]));
//			}
//
//			begin = (NUM_CORE - 1) * split;
//			end = begin + (lastSplit - 1);
//			threads[NUM_CORE - 1] = new thread(Thread_BulletCollision, bullet, ref(mCollisionObjectList), begin, end, ref(paints[NUM_CORE - 1]));
//
//			for (GLuint i = 0; i < NUM_CORE; ++i)
//			{
//				threads[i]->join();
//				for (auto paint : paints[i])
//				{
//					mPaints.emplace_back(paint);
//				}
//				paints[i].clear();
//			}
//			ClearStack();
//
//			if (bullet->IsDestroyed())
//			{
//				delete bullet;
//				iter = mBulletList.erase(iter);
//			}
//			else
//			{
//				bullet->Update();
//				++iter;
//			}
//		}
//		delete[] paints;
//
//		SetConsoleCursor(0, 0);
//		timer::PrintDuration();
//	}
//
//	
//
//	for (auto iter = mPaints.begin(); iter != mPaints.end();)
//	{
//		PaintPlane* paint = *iter;
//		if (paint->Update() == GL_FALSE)
//		{
//			delete paint;
//			iter = mPaints.erase(iter);
//		}
//		else
//		{
//			++iter;
//		}
//	}
//}

GLvoid BulletManager::AddCollisionObject(IBulletCollisionable* object)
{
	mCollisionObjectList.emplace_back(object);
	object->SetID(mID++);
}
GLvoid BulletManager::AddParticleCollision(IBulletCollisionable* object)
{
	mParticleCollisions.emplace_back(object);
}
GLvoid BulletManager::DelCollisionObject(IBulletCollisionable* object)
{
	mCollisionObjectList.erase(remove_if(mCollisionObjectList.begin(), mCollisionObjectList.end() - 1, [&object](IBulletCollisionable* item) {return object->GetID() == item->GetID(); }));
}
GLvoid BulletManager::DelParticleCollision(IBulletCollisionable* object)
{
	mParticleCollisions.erase(remove_if(mParticleCollisions.begin(), mParticleCollisions.end() - 1, [&object](IBulletCollisionable* item) {return object->GetID() == item->GetID(); }));
}