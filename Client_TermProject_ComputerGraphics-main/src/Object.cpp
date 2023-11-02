#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Transform.h"
#include "Timer.h"
#include "Camera.h"
#include "Bullet.h"

#define PAINT_DISAPEAR_TIME 5.0f

#define INIT_LOOK Vector3::Front()

Object::Object()
{
	InitValues();
}
GLvoid Object::InitValues()
{
	mPosition = Vector3::Zero();

	mLook = INIT_LOOK;
	ResetRotation();
	ResetModelRotation();

	mPivot = nullptr;
	mRotationPivot = nullptr;
}
GLvoid Object::Rotate(const glm::vec3& axis, const GLfloat& degree)
{
	mRotation *= glm::angleAxis(glm::radians(degree), glm::normalize(axis));
}
GLvoid Object::Rotate(const GLfloat& yaw, const GLfloat& pitch, const GLfloat& roll)
{
	if (yaw != 0.0f)
	{
		glm::vec3 axis = GetRight();
		mRotation *= glm::angleAxis(glm::radians(yaw), axis);
	}
	if (pitch != 0.0f)
	{
		glm::vec3 axis = GetUp();
		mRotation *= glm::angleAxis(glm::radians(pitch), axis);
	}
	if (roll != 0.0f)
	{
		glm::vec3 axis = GetLook();
		mRotation *= glm::angleAxis(glm::radians(roll), axis);
	}
}
GLvoid Object::SetRotation(const glm::vec3& axis, const GLfloat& degree)
{
	mRotation = glm::angleAxis(glm::radians(degree), glm::normalize(axis));
}
GLvoid Object::SetRotation(const glm::quat& rotation)
{
	mRotation = rotation;
}
GLvoid Object::RotatePivot(const glm::vec3& pivot, const glm::vec3& axis, const GLfloat& degree)
{
	RotateLocal(axis, degree);

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::rotate(transform, glm::radians(-degree), glm::normalize(axis));
	mPosition -= pivot;
	MultiplyVector(transform, mPosition);
	mPosition += pivot;
}
GLvoid Object::RotateLocal(const glm::vec3& axis, const GLfloat& degree)
{
	::Rotate(mLook, glm::radians(degree), glm::normalize(axis));
}
GLvoid Object::RotateLocal(const GLfloat& yaw, const GLfloat& pitch, const GLfloat& roll)
{
	if (pitch != 0.0f)
	{
		glm::vec3 axis = GetUp();
		::Rotate(mLook, glm::radians(pitch), axis);
	}
	if (yaw != 0.0f)
	{
		glm::vec3 axis = GetRight();
		::Rotate(mLook, glm::radians(yaw), axis);
	}
	if (roll != 0.0f)
	{
		mRoll += roll;
		glm::vec3 axis = GetLook();
		::Rotate(mLook, glm::radians(roll), axis);
	}
}
GLvoid Object::RotatePosition(const glm::vec3& pivot, const glm::vec3& axis, const GLfloat& degree)
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::rotate(transform, glm::radians(degree), glm::normalize(axis));
	mPosition -= pivot;
	MultiplyVector(transform, mPosition);
	mPosition += pivot;
}
GLvoid Object::SetLocalRotation(const glm::vec3& axis, const GLfloat& degree)
{
	mLook = INIT_LOOK;

	if (axis == Vector3::Up() || axis == Vector3::Right())
	{
		mLook = Vector3::RotateOrigin(mLook, glm::radians(-degree), axis);
	}
}
GLvoid Object::RotateModel(const glm::vec3& axis, const GLfloat& degree)
{
	mModelRotation = glm::rotate(mModelRotation, glm::radians(degree), axis);
}
GLvoid Object::ResetRotation()
{
	mRotation = glm::quat(1, 0, 0, 0);
	mLook = INIT_LOOK;
}
GLvoid Object::ResetModelRotation()
{
	mModelRotation = glm::quat(1, 0, 0, 0);
	/* 3ds max obj to opengl */
	mModelRotation = glm::rotate(mModelRotation, glm::radians(180.0f), Vector3::Up());
}

glm::quat Object::GetRotation() const
{
	return mRotation;
}
GLvoid Object::SetRotationPivot(const glm::vec3* pivot)
{
	mRotationPivot = pivot;
}




GLvoid Object::SetPivot(const glm::vec3* pivot)
{
	mPivot = pivot;
}


GLvoid Object::Look(const glm::vec3& point)
{
	if (mPivot == nullptr)
	{
		mLook = glm::normalize(point - mPosition);
	}
	else
	{
		mLook = glm::normalize(point - (mPosition + *mPivot));
	}
}
GLvoid Object::SetLook(const glm::vec3& look)
{
	mLook = look;
	if (look.y == 1.0f || look.y == -1.0f)
	{
		// vector cross 시 0이 되는 문제 해결 (nan)
		mLook = glm::normalize(mLook + 0.00001f);
	}
}
GLvoid Object::RotateLook(const glm::vec3& look)
{
	RotateLocal(look.y * 90, look.x * 90, 0);
}
GLvoid Object::ResetLook()
{
	mLook = INIT_LOOK;
}
glm::vec3 Object::GetLook() const
{
	return mLook;
}
glm::vec3 Object::GetRight() const
{
	glm::vec3 right = glm::cross(mLook, Vector3::Up());
	return right;
}
glm::vec3 Object::GetUp() const
{
	glm::vec3 up = glm::cross(GetRight(), mLook);
	return up;
}



glm::vec3 Object::GetPviotedPosition() const
{
	glm::vec3 result = mPosition;
	if (mPivot) result += *mPivot;
	return result;
}
GLvoid Object::Move(const glm::vec3& vector, const GLboolean& applyTime)
{
	MoveX(vector.x, applyTime);
	MoveY(vector.y, applyTime);
	MoveZ(vector.z, applyTime);
}
GLvoid Object::MoveX(const GLfloat& amount, const GLboolean& applyTime)
{
	glm::vec3 right = GetRight();
	GLfloat dt = 1.0f;
	if (applyTime)
	{
		dt = timer::DeltaTime();
	}

	mPosition += right * amount * dt;
}
GLvoid Object::MoveY(const GLfloat& amount, const GLboolean& applyTime)
{
	glm::vec3 up = GetUp();
	GLfloat dt = 1.0f;
	if (applyTime)
	{
		dt = timer::DeltaTime();
	}

	mPosition += up * amount * dt;
}
GLvoid Object::MoveZ(const GLfloat& amount, const GLboolean& applyTime)
{
	glm::vec3 look = GetLook();
	GLfloat dt = 1.0f;
	if (applyTime)
	{
		dt = timer::DeltaTime();
	}

	mPosition += look * amount * dt;
}
GLvoid Object::MoveGlobal(const glm::vec3& vector, const GLboolean& applyTime)
{
	GLfloat dt = 1.0f;
	if (applyTime)
	{
		dt = timer::DeltaTime();
	}
	mPosition += vector * dt;
}


















ShaderObject::ShaderObject()
{
	InitValues();
	SetColor(DEFAULT_OBJECT_COLOR);
}
ShaderObject::~ShaderObject() {}

GLvoid ShaderObject::PrepareDraw() const
{
	ModelTransform();
	shd::SetShader(mShader, "objectColor", glm::vec3(mColor));
}

bool ShaderObject::CompareBlendObject(const ShaderObject* lhs, const ShaderObject* rhs)
{
	extern const Camera* crntCamera;
	glm::vec3 cameraPos = crntCamera->GetPosition();
	GLfloat distanceLHS = glm::length(cameraPos - lhs->GetTransformedPos());
	GLfloat distanceRHS = glm::length(cameraPos - rhs->GetTransformedPos());

	return distanceLHS > distanceRHS;
}

GLvoid ShaderObject::InitValues()
{
	Object::InitValues();
	mScale = { 1.0f,1.0f,1.0f };
	mScaleOrigin = { 1.0f,1.0f,1.0f };
	mColor = DEFAULT_OBJECT_COLOR;
}
GLvoid ShaderObject::SetScale(const GLfloat& scale)
{
	mScale.x = scale;
	mScale.y = scale;
	mScale.z = scale;
}
GLvoid ShaderObject::ScaleOrigin(const GLfloat& scale)
{
	Scale(scale);
	mPosition *= scale;
}

GLvoid ShaderObject::Scale(const GLfloat& scale)
{
	mScale *= scale;
}
GLvoid ShaderObject::SetScale(const glm::vec3& scale)
{
	mScale = scale;
}
GLvoid ShaderObject::ScaleX(const GLfloat& amount)
{
	mScale.x *= amount;
}
GLvoid ShaderObject::ScaleY(const GLfloat& amount)
{
	mScale.y *= amount;
}
GLvoid ShaderObject::ScaleZ(const GLfloat& amount)
{
	mScale.z *= amount;
}
GLvoid ShaderObject::SetScaleX(const GLfloat& scale)
{
	mScale.x = scale;
}
GLvoid ShaderObject::SetScaleY(const GLfloat& scale)
{
	mScale.y = scale;
}
GLvoid ShaderObject::SetScaleZ(const GLfloat& scale)
{
	mScale.z = scale;
}
GLfloat ShaderObject::GetRadius() const
{
	GLfloat width = GetWidth();
	GLfloat depth = GetDepth();

	return (width > depth) ? (width / 2) : (depth / 2);
}
GLrect ShaderObject::GetRect() const
{
	GLfloat halfWidth = GetWidth() / 2;
	GLfloat halfDepth = GetDepth() / 2;

	glm::vec3 pos = GetTransformedPos();

	GLrect result;
	result.left = pos.x - halfWidth;
	result.right = pos.x + halfWidth;
	result.top = pos.z - halfDepth;
	result.bottom = pos.z + halfDepth;

	return result;
}

glm::mat4 ShaderObject::GetTransform() const
{
	glm::mat4 transform = glm::mat4(1.0f);

	if (mPivot != nullptr)
	{
		transform = glm::translate(transform, { (*mPivot).x, (*mPivot).y, (*mPivot).z });
	}

	if (mRotationPivot == nullptr)
	{
		transform *= glm::mat4_cast(mRotation);
		transform = glm::translate(transform, { mPosition.x, mPosition.y, mPosition.z });	// T
	}
	else
	{
		transform = glm::translate(transform, { mPosition.x, mPosition.y, mPosition.z });
		if (mPivot == nullptr)
		{
			transform = glm::translate(transform, *mRotationPivot);
			transform *= glm::mat4_cast(mRotation);
			transform = glm::translate(transform, -*mRotationPivot);
		}
		else
		{
			transform = glm::translate(transform, *mRotationPivot + (*mPivot));
			transform *= glm::mat4_cast(mRotation);
			transform = glm::translate(transform, -*mRotationPivot - (*mPivot));
		}
	}

	/* roll */
	transform = glm::rotate(transform, glm::radians(mRoll), GetLook());

	/* use quaternion from look vector */
	/* mLook(direction) can't be parallel to up */
	glm::quat lookAt = glm::quatLookAt(mLook, GetUp());
	transform *= glm::mat4_cast(lookAt);		// R

	transform *= glm::mat4_cast(mModelRotation);

	transform = glm::scale(transform, mScale);	// S

	return transform;
}
GLvoid ShaderObject::ModelTransform() const
{
	glm::mat4 transform = GetTransform();
	shd::SetShader(mShader, "modelTransform", transform);
}
glm::vec3 ShaderObject::GetCenterPos() const
{
	glm::vec3 explosionPos = GetTransformedPos();
	explosionPos.y += GetHeight() / 2;
	return explosionPos;
}









IdentityObject::IdentityObject() : ShaderObject()
{
	InitValues();
	InitBuffers();
}
IdentityObject::~IdentityObject()
{
	DeleteBuffers();
}
GLvoid IdentityObject::InitValues()
{
	ShaderObject::InitValues();
	mVAO = 0;
	mVBO[0] = 0;
	mVBO[1] = 0;
	mVBO[2] = 0;
	mEBO = 0;
	mTexture = 0;

	SetShader(Shader::Light);
}
GLvoid IdentityObject::InitBuffers()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(3, &mVBO[0]);
	glGenBuffers(1, &mEBO);
}
GLvoid IdentityObject::DeleteBuffers()
{
	glDeleteBuffers(3, &mVBO[0]);
	glDeleteBuffers(1, &mEBO);
	glDeleteVertexArrays(1, &mVAO);
}

GLvoid IdentityObject::BindBuffers()
{
	vector<GLfloat> vertices;
	vector<GLfloat> normals;
	vector<GLfloat> uvs;
	vector<size_t> indices;

	PullVertices(vertices);
	Pull_Indices_Vertex(indices);

	switch (mShader)
	{
	case Shader::Back:
	case Shader::Texture:
		PullUVs(uvs);
		PullNormals(normals);
		break;
	case Shader::Light:
		PullNormals(normals);
		break;
	}

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	if (normals.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}

	if (mShader == Shader::Texture || mShader == Shader::Back)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uvs.size(), uvs.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}
	else
	{
		glDeleteBuffers(1, &mVBO[2]);
	}

	if (indices.size() > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(size_t) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);
	}
	else
	{
		glDeleteBuffers(1, &mEBO);
	}

	glBindVertexArray(0);
}
GLvoid IdentityObject::SetTexture(const Textures& textureModel)
{
	mTexture = ::GetTexture(textureModel);
}

GLvoid IdentityObject::Draw() const
{
	ShaderObject::PrepareDraw();
	const size_t indexCount = GetIndexCount();

	if (mShader == Shader::Texture || mShader == Shader::Back)
	{
		glBindTexture(GL_TEXTURE_2D, mTexture);
	}
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
GLfloat IdentityObject::GetWidth() const
{
	assert(0);
	return 0;
}
GLfloat IdentityObject::GetHeight() const
{
	assert(0);
	return 0;
}
GLfloat IdentityObject::GetDepth() const
{
	assert(0);
	return 0;
}


SharedObject::SharedObject(const IdentityObject* object) : ShaderObject()
{
	Init(object);
}
GLvoid SharedObject::Init(const IdentityObject* object)
{
	SetObject(object);
	SetShader(object->GetShader());
	mTexture = object->GetTexture();
}
GLvoid SharedObject::Draw() const
{
	ShaderObject::PrepareDraw();
	const size_t indexCount = mObject->GetIndexCount();

	glBindVertexArray(mObject->GetVAO());
	if (indexCount > 0)
	{
		if (GetShader() == Shader::Texture)
		{
			glBindTexture(GL_TEXTURE_2D, mTexture);
		}
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		const size_t vertexCount = mObject->GetVertexCount();
		switch (vertexCount)
		{
		case 1:
			glDrawArrays(GL_POINT, 0, 1);
			break;
		case 2:
			glDrawArrays(GL_LINES, 0, 2);
			break;
		default:
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			break;
		}
	}
	glBindVertexArray(0);
}
GLvoid SharedObject::SetTexture(const Textures& textureModel)
{
	mTexture = ::GetTexture(textureModel);
}










///// private /////
ModelObject::ModelObject() : IdentityObject()
{

}
ModelObject::ModelObject(const Model* model, const Shader& shader) : IdentityObject()
{
	LoadModel(model, shader);
}
GLvoid ModelObject::LoadModel(const Model* model, const Shader& shader)
{
	if (mModel != nullptr)
	{
		DeleteBuffers();
	}

	IdentityObject::InitValues();
	InitBuffers();
	mModel = model;

	SetShader(shader);
	
	BindBuffers();
}
GLvoid ModelObject::PullNormals(vector<GLfloat>& normals) const
{
	const vector<glm::vec3> objectNormals = mModel->GetNormals();
	const vector<size_t> indices_normal = mModel->GetIndices_Normal();
	const vector<size_t> indices_vertex = mModel->GetIndices_Vertex();

	// vertex마다 normal을 지정
	normals.resize(mModel->GetVertexCount() * 3, 0);
	size_t count = 0;
	for (const size_t& vertexIndex : indices_vertex)
	{
		glm::vec3 normal = objectNormals[indices_normal[count]];
		normals[(vertexIndex * 3)] = normal.x;
		normals[(vertexIndex * 3) + 1] = normal.y;
		normals[(vertexIndex * 3) + 2] = normal.z;
		++count;
	}
}
GLvoid ModelObject::PullVertices(vector<GLfloat>& vertices) const
{
	const vector<glm::vec3> objectVertices = mModel->GetVertices();
	for (size_t i = 0; i < objectVertices.size(); ++i)
	{
		vertices.emplace_back(objectVertices[i].x);
		vertices.emplace_back(objectVertices[i].y);
		vertices.emplace_back(objectVertices[i].z);
	}
}
GLvoid ModelObject::Pull_Indices_Vertex(vector<size_t>& indices_vertex) const
{
	const vector<size_t> indices = mModel->GetIndices_Vertex();
	indices_vertex.resize(indices.size());
	std::copy(indices.begin(), indices.end(), indices_vertex.begin());
}
GLvoid ModelObject::Pull_Indices_Normal(vector<size_t>& indices_normal) const
{
	const vector<size_t> indices = mModel->GetIndices_Normal();
	indices_normal.resize(indices.size());
	std::copy(indices.begin(), indices.end(), indices_normal.begin());
}
GLvoid ModelObject::PullUVs(vector<GLfloat>& uvs) const
{
	const vector<glm::vec2> objectUVs = mModel->GetUVs();
	const vector<size_t> indices_uv = mModel->GetIndices_UV();
	const vector<size_t> indices_vertex = mModel->GetIndices_Vertex();

	// vertex마다 uv를 지정
	uvs.resize(mModel->GetVertexCount() * 2, 0);
	size_t count = 0;
	for (const size_t& vertexIndex : indices_vertex)
	{
		glm::vec2 uv = objectUVs[indices_uv[count]];
		uvs[(vertexIndex * 2)] = uv.x;
		uvs[(vertexIndex * 2) + 1] = uv.y;
		++count;
	}
}
GLvoid ModelObject::Pull_Indices_UV(vector<size_t>& indices_uv) const
{
	const vector<size_t> indices = mModel->GetIndices_UV();
	indices_uv.resize(indices.size());
	std::copy(indices.begin(), indices.end(), indices_uv.begin());
}


///// public /////
size_t ModelObject::GetIndexCount() const
{
	return mModel->GetIndexCount();
}
size_t ModelObject::GetVertexCount() const
{
	return mModel->GetVertexCount();
}
GLfloat ModelObject::GetSide(const Dir& dir) const
{
	GLfloat width = ModelObject::GetWidth();
	GLfloat height = ModelObject::GetHeight();
	GLfloat depth = ModelObject::GetDepth();

	switch (dir)
	{
	case Dir::Left:
	{
		GLfloat left = mPosition.x - width / 2;
		if (mPivot)
		{
			left += mPivot->x;
		}
		return left;
	}
	case Dir::Right:
	{
		GLfloat right = mPosition.x + width / 2;
		if (mPivot)
		{
			right -= mPivot->x;
		}
		return right;
	}
	case Dir::Down:
	{
		GLfloat bottom = mPosition.y - height / 2;
		if (mPivot)
		{
			bottom += mPivot->y;
		}
		return bottom;
	}
	case Dir::Up:
	{
		GLfloat top = mPosition.y + height / 2;
		if (mPivot)
		{
			top -= mPivot->y;
		}
		return top;
	}
	case Dir::Front:
	{
		GLfloat front = mPosition.z - depth / 2;
		if (mPivot)
		{
			front -= mPivot->z;
		}
		return front;
	}
	case Dir::Back:
	{
		GLfloat back = mPosition.z + depth / 2;
		if (mPivot)
		{
			back += mPivot->z;
		}
		return back;
	}
	default:
		assert(0);
		return -1;
	}
}
GLfloat ModelObject::GetWidth() const
{
	GLfloat width = mModel->GetWidth();
	return width * mScale.x;
}
GLfloat ModelObject::GetHeight() const
{
	GLfloat height = mModel->GetHeight();
	return height * mScale.y;
}
GLfloat ModelObject::GetDepth() const
{
	GLfloat depth = mModel->GetDepth();
	return depth * mScale.z;
}
GLrect ModelObject::GetXZRect() const
{
	Cuboid cuboid(&mPosition, &mScale, GetWidth(), GetHeight(), GetDepth());
	return cuboid.GetXZRect();
}



///// private /////
GLvoid CustomObject::PullColors(vector<GLfloat>& colors) const
{
	for (size_t i = 0; i < mVertices.size(); ++i)
	{
		colors.emplace_back(mColor.r);
		colors.emplace_back(mColor.g);
		colors.emplace_back(mColor.b);
	}
}
GLvoid CustomObject::PullVertices(vector<GLfloat>& vertices) const
{
	for (size_t i = 0; i < mVertices.size(); ++i)
	{
		vertices.emplace_back(mVertices[i].x);
		vertices.emplace_back(mVertices[i].y);
		vertices.emplace_back(mVertices[i].z);
	}
}
GLvoid CustomObject::Pull_Indices_Vertex(vector<size_t>& indices_vertex) const
{
	for (const size_t& index : mIndices)
	{
		indices_vertex.emplace_back(index);
	}
}
size_t CustomObject::GetIndexCount() const
{
	return mIndices.size();
}
size_t CustomObject::GetVertexCount() const
{
	return mVertices.size();
}


///// public /////
CustomObject::CustomObject() : IdentityObject()
{
	SetShader(Shader::Color);
}
CustomObject::CustomObject(vector<glm::vec3>& vertices) : CustomObject()
{
	mVertices = vertices;
	BindBuffers();
}
CustomObject::CustomObject(vector<glm::vec3>& vertices, vector<size_t>& indices) : CustomObject()
{
	mVertices = vertices;
	mIndices = indices;
	BindBuffers();
}
set<glm::vec2, CompareSet> ModelObject::GetBoundings_XZ() const
{
	return mModel->GetBoundings_XZ();
}












////////// [ BASIC OBJECTS ] //////////

// Line
LineObject::LineObject() : CustomObject()
{
	glm::vec3 v1 = { 0, 0, 1.0f };
	glm::vec3 v2 = { 0, 0, -1.0f };
	mVertices.emplace_back(v1);
	mVertices.emplace_back(v2);
	BindBuffers();
}
LineObject::LineObject(const glm::vec3& v1, const glm::vec3& v2) : CustomObject()
{
	mVertices.emplace_back(v1);
	mVertices.emplace_back(v2);
	BindBuffers();
}
GLvoid LineObject::Draw() const
{
	ShaderObject::PrepareDraw();

	glBindVertexArray(mVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}
GLvoid LineObject::SetVertex(const GLboolean& index, const glm::vec3& pos)
{
	mVertices[index] = pos;
}

// Triangle
TriangleObject::TriangleObject() : CustomObject()
{
	const vector<glm::vec3> defaultTriangle = {
		{  0, 0, -1},
		{ -1, 0,  1},
		{  1, 0,  1},
	};
	mVertices = defaultTriangle;

	BindBuffers();
}
GLvoid TriangleObject::Draw() const
{
	ShaderObject::PrepareDraw();

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

// Plane
PlaneObject::PlaneObject() : CustomObject()
{
	const vector<glm::vec3> defaultPlane = {
		{-1, 0, -1},
		{-1, 0,  1},
		{ 1, 0, -1},
		{ 1, 0,  1},
	};
	mVertices = defaultPlane;

	BindBuffers();
}
GLvoid PlaneObject::Draw() const
{
	ShaderObject::PrepareDraw();

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
glm::vec3 PlaneObject::GetNormal() const
{
	glm::vec3 vertices[3] = { mVertices[0],mVertices[1] ,mVertices[2] };

	glm::mat4 transform = GetTransform();
	for (glm::vec3& vertex : vertices)
	{
		MultiplyVector(transform, vertex);
	}

	glm::vec3 v1 = vertices[0];
	glm::vec3 v2 = vertices[1];
	glm::vec3 v3 = vertices[2];

	return glm::normalize(glm::cross(v3 - v2, v1 - v2));
}
GLfloat PlaneObject::CheckCollision(const glm::vec3& center, const GLfloat& radius) const
{
	glm::vec3 vertices[3] = { mVertices[0],mVertices[1] ,mVertices[2] };

	glm::mat4 transform = GetTransform();
	for (glm::vec3& vertex : vertices)
	{
		MultiplyVector(transform, vertex);
	}

	glm::vec3 v1 = vertices[0];
	glm::vec3 v2 = vertices[1];
	glm::vec3 v3 = vertices[2];
	glm::vec3 normal = glm::normalize(glm::cross(v3 - v2, v1 - v2));

	GLfloat distance = fabs(glm::dot((center - v1), normal) / glm::length(normal));

	GLfloat interRadius = sqrtf((radius * radius) - (distance * distance));
	glm::vec3 point = center + distance * normal;
	if ((distance < radius) && (interRadius > 0))
	{
		return radius - distance;
	}

	return 0.0f;
}


// Cuboid
Cuboid::Cuboid(const IdentityObject* object)
{
	mPosition = object->GetRefPos();
	mScale = object->GetRefScale();
	mHalfWidth = object->GetWidth() / 2;
	mHeight = object->GetHeight();
	mHalfDepth = object->GetDepth() / 2;
}
Cuboid::Cuboid(const SharedObject* object, const GLfloat& width, const GLfloat& height, const GLfloat& depth)
{
	mPosition = object->GetRefPos();
	mScale = object->GetRefScale();
	mHalfWidth = width;
	mHeight = height;
	mHalfDepth = depth;
}
Cuboid::Cuboid(const glm::vec3* position, const glm::vec3* scale, const GLfloat& width, const GLfloat& height, const GLfloat& depth)
{
	mPosition = position;
	mScale = scale;
	mHalfWidth = width / 2;
	mHeight = height;
	mHalfDepth = depth / 2;
}

GLfloat Cuboid::GetSide(const Dir& dir) const
{
	GLfloat w = mHalfWidth * mScale->x;
	GLfloat h = mHeight * mScale->y;
	GLfloat d = mHalfDepth * mScale->z;

	if (mScale->x == 0)
	{
		w = 0;
	}
	if (mScale->y == 0)
	{
		h = 0;
	}
	if (mScale->z == 0)
	{
		d = 0;
	}

	switch (dir)
	{
	case Dir::Left:
		return mPosition->x - w;
	case Dir::Right:
		return mPosition->x + w;
	case Dir::Down:
		return mPosition->y;
	case Dir::Up:
		return mPosition->y + h;
	case Dir::Front:
		return mPosition->z - d;
	case Dir::Back:
		return mPosition->z + d;
	default:
		assert(0);
		return 0;
	}
}


GLrect Cuboid::GetXZRect() const
{
	GLrect rect;
	rect.left = GetSide(Dir::Left);
	rect.right = GetSide(Dir::Right);
	rect.top = GetSide(Dir::Back);
	rect.bottom = GetSide(Dir::Front);

	return rect;
}
GLboolean Cuboid::CheckCollide(const glm::vec3& point, const GLfloat& radius) const
{
	const GLfloat w = mHalfWidth / mScale->x;
	const GLfloat h = mHeight / mScale->y;
	const GLfloat d = mHalfDepth / mScale->z;

	const GLfloat left = mPosition->x - w + radius;
	const GLfloat right = mPosition->x + w - radius;
	const GLfloat top = mPosition->y + h - radius;
	const GLfloat bottom = mPosition->y - h + radius;
	const GLfloat front = mPosition->z - d + radius;
	const GLfloat back = mPosition->z + d - radius;

	if( (point.x < right && point.x > left) &&
		(point.y < top && point.y > bottom) &&
		(point.z < back && point.z > front))
	{
		return GL_TRUE;
	}

	return GL_FALSE;
}
GLboolean Cuboid::CheckCollide(const GLrect& rect) const
{
	const GLfloat w = mHalfWidth / mScale->x;
	const GLfloat h = mHeight / mScale->y;
	const GLfloat d = mHalfDepth / mScale->z;

	const GLfloat left = mPosition->x - w;
	const GLfloat right = mPosition->x + w;
	const GLfloat front = mPosition->z - d;
	const GLfloat back = mPosition->z + d;

	if (((rect.left > left && rect.left < right) || (rect.right > left && rect.right < right)) &&
		((rect.top > front && rect.top < back) || (rect.bottom > front && rect.bottom < back)))
	{
		return GL_TRUE;
	}

	return GL_FALSE;
}
GLvoid Cuboid::Draw() const
{
	const GLfloat w = mHalfWidth * mScale->x;
	const GLfloat h = (mHeight/2) * mScale->y;
	const GLfloat d = mHalfDepth * mScale->z;

	const LineObject* identityLine = GetIdentityLine();
	SharedObject line(identityLine);

	// z
	line.SetPosition(*mPosition);
	line.SetScaleZ(d);
	line.SetColor(BLUE);
	for (size_t i = 0; i < 2; ++i)
	{
		if (i == 0)
		{
			line.SetPosY(GetSide(Dir::Down));
		}
		else
		{
			line.SetPosY(GetSide(Dir::Up));
		}

		line.SetPosX(GetSide(Dir::Left));
		line.Draw();
		line.SetPosX(GetSide(Dir::Right));
		line.Draw();
	}

	// x
	line.SetPosition(*mPosition);
	line.SetScaleZ(w);
	line.SetColor(RED);
	line.RotateLocal(0, 90, 0);
	for (size_t i = 0; i < 2; ++i)
	{
		if (i == 0)
		{
			line.SetPosY(GetSide(Dir::Down));
		}
		else
		{
			line.SetPosY(GetSide(Dir::Up));
		}

		line.SetPosZ(GetSide(Dir::Front));
		line.Draw();
		line.SetPosZ(GetSide(Dir::Back));
		line.Draw();
	}

	// y
	line.SetPosition(*mPosition);
	line.SetScaleZ(h);
	line.MoveY(h);
	line.SetColor(GREEN);
	line.RotateLocal(Vector3::Right(), -90);
	for (size_t i = 0; i < 2; ++i)
	{
		if (i == 0)
		{
			line.SetPosX(GetSide(Dir::Left));
		}
		else
		{
			line.SetPosX(GetSide(Dir::Right));
		}

		line.SetPosZ(GetSide(Dir::Front));
		line.Draw();
		line.SetPosZ(GetSide(Dir::Back));
		line.Draw();
	}
}


Circle::Circle(const glm::vec3* position, const GLfloat& radius, const GLboolean& isShadow)
{
	mRadius = radius;
	mIsShadow = isShadow;
	mCircle = new SharedObject(GetIdentityModelObject(Models::Circle));
	mCircle->SetScale(radius);
	mCircle->SetPivot(position);
}
GLvoid Circle::Draw() const
{
	if (mIsShadow)
	{
		mCircle->SetPosY(0.1f - mCircle->GetPivot()->y);
	}
	mCircle->Draw();
}
GLvoid Circle::SetColor(const COLORREF& color)
{
	mCircle->SetColor(color);
}
glm::vec2 Circle::GetCenter() const
{
	glm::vec3 pos = mCircle->GetPviotedPosition();
	return { pos.x, pos.z };
}
GLfloat Circle::GetRadius() const
{
	return mRadius;
}










GLvoid ICollisionable_2D::SetCollision(const glm::vec3* position, const GLfloat& radius)
{
	mPosition = position;
	mRadius = radius;
	mCollisionType = CollisionType::Circle;
}
GLvoid ICollisionable_2D::SetCollision(const glm::vec3* position, const GLfloat& width, const GLfloat& depth)
{
	mPosition = position;
	mWidth = width;
	mDepth = depth;
	mCollisionType = CollisionType::Rect;
}

GLfloat ICollisionable_2D::GetRadius() const
{
	assert(mCollisionType == CollisionType::Circle);
	return mRadius;
}
GLfloat ICollisionable_2D::GetWidth() const
{
	assert(mCollisionType == CollisionType::Rect);
	return mWidth;
}
GLfloat ICollisionable_2D::GetDepth() const
{
	assert(mCollisionType == CollisionType::Rect);
	return mDepth;
}


CollisionManager::CollisionManager()
{
	mMonsterList.reserve(100);
	mObstacleList.reserve(100);
}
CollisionManager::~CollisionManager()
{

}

GLvoid CollisionManager::CheckCollisions()
{
	for (ICollisionable_2D* monster : mMonsterList)
	{
		//monster
	}
}
GLvoid CollisionManager::AddObject(const Tag& tag, ICollisionable_2D* object)
{
}
GLvoid CollisionManager::DeleteObject(ICollisionable_2D* object)
{
}

























// Bug : does not work correctly when a vertices size is large
GLvoid DrawDebugWireXZ(const set<glm::vec2, CompareSet>& vertices, GLfloat yPos, const COLORREF& color, const glm::vec3* pivot)
{
	if (pivot != nullptr)
	{
		yPos += pivot->y;
	}

	LineObject* line = nullptr;

	for (glm::vec2 v1 : vertices)
	{
		if (pivot != nullptr)
		{
			v1.x += pivot->x;
			v1.y += pivot->z;
		}
		for (glm::vec2 v2 : vertices)
		{
			if (v1 == v2)
			{
				continue;
			}
			if (pivot != nullptr)
			{
				v2.x += pivot->x;
				v2.y += pivot->z;
			}

			line = new LineObject({ v1.x , yPos, v1.y }, { v2.x , yPos, v2.y });
			line->SetColor(color);
			line->BindBuffers();
			line->Draw();
			delete line;
		}
	}
}






PaintPlane::PaintPlane(const IdentityObject* object, const COLORREF& color, const glm::vec3& pos, const glm::vec3& normal) : SharedObject(object)
{
	SetPosition(pos);
	SetColor(color);
	
	SetLook(normal);

	GLfloat randZ = ((rand() % 1000)*0.0001f) + 0.2f;	// 0.2 ~ 0.0999 + 0.2
	MoveZ(randZ, GL_FALSE);

	GLfloat randRotation = rand() % 720 * 0.5f;
	RotateLocal(0, 0, randRotation);
}
GLboolean PaintPlane::Update()
{
	dt += timer::DeltaTime();
	if (dt >= PAINT_DISAPEAR_TIME)
	{
		return GL_FALSE;
	}

	return GL_TRUE;
}






















// identity simple objects
static const LineObject* lineObject = nullptr;

static ModelObject* modelObjects[NUM_MODEL];
static ModelObject* textureModelObjects[NUM_TEXTURE];

GLvoid InitObject()
{
	lineObject = new LineObject();

	for (GLsizei i = 0; i < NUM_MODEL; ++i)
	{
		const Model* model = GetModel(static_cast<Models>(i));
		modelObjects[i] = new ModelObject(model, Shader::Light);
	}

	for (GLsizei i = 0; i < NUM_TEXTURE; ++i)
	{
		Textures textureModel = static_cast<Textures>(i);
		const Model* model = GetTextureModel(textureModel);
		textureModelObjects[i] = new ModelObject(model, Shader::Texture);
		textureModelObjects[i]->SetTexture(textureModel);
	}
}
const LineObject* GetIdentityLine()
{
	return lineObject;
}
const ModelObject* GetIdentityModelObject(const Models& model)
{
	return modelObjects[static_cast<GLint>(model)];
}
const ModelObject* GetIdentityTextureObject(const Textures& textureModel)
{
	return textureModelObjects[static_cast<GLint>(textureModel)];
}





static vector<ShaderObject*> customObjects;
static vector<ShaderObject*> lightObjects;
static vector<ShaderObject*> textureObjects;
static vector<ShaderObject*> blendObjects;
static vector<ShaderObject*> minimapObjects;


GLvoid AddObject(const Shader& shader, ShaderObject* object)
{
	switch (shader)
	{
	case Shader::Color:
		customObjects.emplace_back(object);
		break;
	case Shader::Light:
		lightObjects.emplace_back(object);
		break;
	case Shader::Texture:
		textureObjects.emplace_back(object);
		break;
	default:
		assert(0);
		break;
	}
}
GLvoid AddBlendObject(ShaderObject* object)
{
	blendObjects.emplace_back(object);
}
GLvoid DeleteBlendObject(ShaderObject* object)
{
	for(auto it = blendObjects.begin(); it != blendObjects.end(); ++it)
	{
		ShaderObject* o = *it;
		if (object == o)
		{
			blendObjects.erase(it);
			return;
		}
	}
}
GLvoid DeleteObject(const Shader& shader, ShaderObject* object)
{

}
GLvoid DeleteObjects()
{
	for (ShaderObject* object : customObjects)
	{
		delete object;
	}
	for (ShaderObject* object : lightObjects)
	{
		delete object;
	}
	for (ShaderObject* object : textureObjects)
	{
		delete object;
	}
	for (ShaderObject* object : minimapObjects)
	{
		delete object;
	}
	customObjects.clear();
	lightObjects.clear();
	textureObjects.clear();
	minimapObjects.clear();
}


GLvoid DrawObjects(const Shader& shader)
{
	switch (shader)
	{
	case Shader::Color:
		for (const ShaderObject* object : customObjects)
		{
			object->Draw();
		}
		break;
	case Shader::Light:
		for (const ShaderObject* object : lightObjects)
		{
			object->Draw();
		}
		break;
	case Shader::Texture:
		for (const ShaderObject* object : textureObjects)
		{
			object->Draw();
		}
		break;
	default:
		break;
	}

}


typedef pair<ShaderObject*, GLfloat> blendObject;

GLvoid Sort(vector<blendObject>& objects)
{
	std::sort(objects.begin(), objects.end(), [](const blendObject& lhs, const blendObject& rhs) { return lhs.second < rhs.second; });
}

/* paints는 삽입/제거가 빈번하게 일어나기 때문에 */
/* 속도 향상을 위해 paints와 blendObjects를 분리 후 Draw 시에 병합 */
GLvoid DrawBlendObjects()
{
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	constexpr GLfloat maxPaintDistance = 300.0f;

	/* Camera 위치 */
	extern const Camera* crntCamera;
	glm::vec3 cameraPos = crntCamera->GetPosition();

	/* Paints 불러오기 */
	extern BulletManager* bulletManager;
	const vector<PaintPlane*>& paints = bulletManager->GetPaints();

	/* size 계산 */
	const size_t blendObjectSize = blendObjects.size();
	const size_t totalSize = blendObjectSize + paints.size();

	/* 멀리 있는 Paints는 정렬하지 않음 */
	vector<blendObject> nearObjects;
	vector<ShaderObject*> farObjects;
	for (size_t i = 0; i < blendObjectSize; ++i)
	{
		ShaderObject* object = blendObjects[i];
		GLfloat distance = glm::length(cameraPos - object->GetTransformedPos());
		nearObjects.emplace_back(make_pair(object, distance));
	}

	for (size_t i = blendObjectSize; i < totalSize; ++i)
	{
		ShaderObject* object = paints[i - blendObjectSize];
		GLfloat distance = glm::length(cameraPos - object->GetPosition());
		if (distance < maxPaintDistance)
		{
			nearObjects.emplace_back(make_pair(object, distance));
		}
		else
		{
			farObjects.emplace_back(object);
		}
	}

	Sort(nearObjects);

	for (size_t i = 0; i < nearObjects.size(); ++i)
	{
		nearObjects[i].first->Draw();
	}
	for (size_t i = 0; i < farObjects.size(); ++i)
	{
		farObjects[i]->Draw();
	}

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}