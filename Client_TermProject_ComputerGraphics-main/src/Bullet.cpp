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
	//for (const PaintPlane* paint : mPaints)
	//{
	//	paint->Draw();
	//}
}

#define MAXBULLET 100

struct BulletInfo {
	char bulletNum[sizeof(int)];
	char bulletPos[sizeof(glm::vec3) * MAXBULLET];
	char bulletType[sizeof(BulletType) * MAXBULLET];
	char bulletColor[sizeof(unsigned long) * MAXBULLET];
	char bulletScale[sizeof(uint32_t) * MAXBULLET];

	char particleNum[sizeof(int)];
	char particlePos[sizeof(glm::vec3) * MAXBULLET];
	char particleRadius[sizeof(uint32_t) * MAXBULLET];
	char particleColor[sizeof(unsigned long) * MAXBULLET];

	char paintNum[sizeof(int)];
	char paintTexture[sizeof(Textures) * MAXBULLET];
	char painthitPoint[sizeof(glm::vec3) * MAXBULLET];
	char paintColor[sizeof(unsigned long) * MAXBULLET];
	char paintNormal[sizeof(glm::vec3) * MAXBULLET];
	char paintScale[sizeof(glm::vec3) * MAXBULLET];
};

GLvoid BulletManager::Update(SOCKET& sock)
{
	BulletInfo bulletInfo;
	char buf[sizeof(BulletInfo)];
	char nBulletBuf[sizeof(int)];

	int retval = recv(sock, buf, sizeof(BulletInfo), 0);
	if (retval == SOCKET_ERROR) {
		printf("SOCKET_ERROR\n");
		return;
	}
	memcpy(&bulletInfo, &buf, sizeof(BulletInfo));
	int nbullets;
	int nParticles = 0;
	int nPaints = 0;

	memcpy(&nbullets, &bulletInfo.bulletNum, sizeof(int));
	memcpy(&nParticles, &bulletInfo.particleNum, sizeof(int));
	memcpy(&nPaints, &bulletInfo.paintNum, sizeof(int));
#ifdef DEBUG
	cout << "Bullet : " << endl;
	cout << "Bullet 偎熱: " << nbullets << endl;
	cout << "Particle 偎熱: " << nParticles << endl;
	cout << "Paint 偎熱: " << nPaints << endl;
#endif
		// 等檜攪 嫡晦
	uint32_t nScale[MAXBULLET];
	unsigned long cColor[MAXBULLET];
	BulletType types[MAXBULLET];
	glm::vec3 vPos[MAXBULLET];

	memcpy(&types, &bulletInfo.bulletType, sizeof(BulletType) * nbullets);
	memcpy(&cColor, &bulletInfo.bulletColor, sizeof(unsigned long) * nbullets);
	memcpy(&vPos, &bulletInfo.bulletPos, sizeof(glm::vec3) * nbullets);
	memcpy(&nScale, &bulletInfo.bulletScale, sizeof(uint32_t) * nbullets);
	GLfloat scale[MAXBULLET];
	COLORREF nColor[MAXBULLET];

	for (int i = 0; i < nbullets; ++i) {
		nColor[i] = static_cast<COLORREF>(cColor[i]);
		scale[i] = *reinterpret_cast<float*>(&nScale[i]);
	}

	// --- Particle
	uint32_t nPartRadius[MAXBULLET];
	GLfloat fPartRadius[MAXBULLET];
	unsigned long cPartColor[MAXBULLET];
	glm::vec3 vPartPos[MAXBULLET];
	memcpy(&nPartRadius, &bulletInfo.particleRadius, sizeof(uint32_t) * nParticles);
	memcpy(&cPartColor, &bulletInfo.particleColor, sizeof(unsigned long) * nParticles);
	memcpy(&vPartPos, &bulletInfo.particlePos, sizeof(glm::vec3) * nParticles);
	COLORREF nPartColor[MAXBULLET];

	for (int i = 0; i < nParticles; ++i) {
		fPartRadius[i] = *reinterpret_cast<GLfloat*>(&nPartRadius[i]);
		nPartColor[i] = static_cast<COLORREF>(cPartColor[i]);
	}

	// --- paints
	Textures texture[MAXBULLET];
	unsigned long nPaintColor[MAXBULLET];
	COLORREF paintcolor[MAXBULLET];
	glm::vec3 vHitPoint[MAXBULLET];
	glm::vec3 vNormal[MAXBULLET];
	glm::vec3 vScale[MAXBULLET];
	memcpy(&texture, &bulletInfo.paintTexture, sizeof(Textures) * nPaints);
	memcpy(&nPaintColor, &bulletInfo.paintColor, sizeof(unsigned long) * nPaints);
	memcpy(&vHitPoint, &bulletInfo.painthitPoint, sizeof(glm::vec3) * nPaints);
	memcpy(&vNormal, &bulletInfo.paintNormal, sizeof(glm::vec3) * nPaints);
	memcpy(&vScale, &bulletInfo.paintScale, sizeof(glm::vec3) * nPaints);

	for (int i = 0; i < nPaints; ++i) {
		paintcolor[i] = static_cast<COLORREF>(nPaintColor[i]);
	}

	mCrntInkSoundDelay += timer::DeltaTime();

	glm::vec3 origin = { 0, 9, 0 };

	mBulletList.clear();
	mParticles.clear();
	mPaints.clear();

	for (int i = 0; i < nbullets; ++i) {
		BulletData data;
		if (types[i] == BulletType::Rocket)
			data = { BulletType::Rocket, nColor[i], 0, scale[i],0,0, Models::GeoSphere };
		else data = { types[i], nColor[i], 0, scale[i],0,0, Models::LowSphere };
		Create(data, origin, vPos[i], 0, 0);
	}
	for (int i = 0; i < nParticles; ++i) {
		BulletData data = { BulletType::Particle_Explosion, 
			nPartColor[i], 0, 0.1f,0,0, Models::GeoSphere };
		Create(data, origin, vPartPos[i], 0, 0);
	}
	for (int i = 0; i < nPaints; ++i) {
		cout << "Texture: " << static_cast<GLuint>(texture[i]) << endl;
		const IdentityObject* object = GetIdentityTextureObject(texture[i]);
		PaintPlane* plane = new PaintPlane(object, paintcolor[i], vHitPoint[i], vNormal[i]);
		plane->SetScale(vScale[i]);
		mPaints.emplace_back(plane);
	}
}

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
