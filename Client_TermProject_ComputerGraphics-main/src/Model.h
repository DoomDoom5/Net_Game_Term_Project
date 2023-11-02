#pragma once
#include "stdafx.h"

class Cuboid;
enum class Models { Plane = 0, Circle, Cube, LowSphere, GeoSphere, _count };
enum class Textures { 
	Gun,
	Map,
	CubeMap,
	Blooper,
	Egg,
	Koromon,
	Core,
	Turret_Body,
	Turret_Head,
	Paint,
	Paint2,
	Coin,
	Player_Head,
	Player_Body,
	Player_Arms,
	Player_Leg_L,
	Player_Leg_R,
	/* must be ui */
	UI_NUM_0,
	UI_NUM_1,
	UI_NUM_2,
	UI_NUM_3,
	UI_NUM_4,
	UI_NUM_5,
	UI_NUM_6,
	UI_NUM_7,
	UI_NUM_8,
	UI_NUM_9,
	UI_TEXT_HP,
	UI_TEXT_SLASH,
	UI_TEXT_X,
	UI_COLOR_HP,
	UI_RIFLE_SYMBOL,
	UI_SHOTGUN_SYMBOL,
	UI_LAUNCHER_SYMBOL,
	UI_SNIPER_SYMBOL,
	UI_TURRET_SYMBOL,
	UI_GAME_OVER,
	_count
};
constexpr GLuint NUM_MODEL = static_cast<GLuint>(Models::_count);
constexpr GLuint NUM_TEXTURE = static_cast<GLuint>(Textures::_count);
constexpr GLuint NUM_PAINT = 2;


class Model {
private:
	vector<glm::vec3> mVertices;
	vector<glm::vec3> mNormals;
	vector<glm::vec2> mUVs;
	vector<size_t> mIndices_vertex;
	vector<size_t> mIndices_normal;
	vector<size_t> mIndices_uv;
	
	set<glm::vec2, CompareSet> mVerticesXZ;	// for XZ-collision

	Cuboid* mCuboid = nullptr;				// for 3D-box collision
	GLfloat mWidth = 0;
	GLfloat mHeight = 0;
	GLfloat mDepth = 0;

public:
	Model() {};
	Model(const GLchar* path);
	Model(const Model& origin);
	GLvoid LoadModel(const GLchar* path);

	inline set<glm::vec2, CompareSet> GetBoundings_XZ() const { return mVerticesXZ; }

	inline constexpr const vector<glm::vec3>& GetVertices() const { return mVertices; }
	inline constexpr const vector<glm::vec3>& GetNormals() const { return mNormals; }
	inline constexpr const vector<glm::vec2>& GetUVs() const { return mUVs; }
	inline constexpr const vector<size_t>& GetIndices_Vertex() const { return mIndices_vertex; }
	inline constexpr const vector<size_t>& GetIndices_Normal() const { return mIndices_normal; }
	inline constexpr const vector<size_t>& GetIndices_UV() const { return mIndices_uv; }
	inline size_t GetVertexCount() const { return mVertices.size(); }
	inline size_t GetIndexCount() const { return mIndices_vertex.size(); }
	glm::vec3 GetVertex(const size_t& index) const;

	Cuboid* GetCuboid(const glm::vec3* position, const glm::vec3* scale) const;
	inline GLfloat GetWidth() const { return mWidth; }
	inline GLfloat GetHeight() const {return mHeight; }
	inline GLfloat GetDepth() const { return mDepth; }

	GLvoid ReverseNormal();
};

GLvoid InitModels();
const GLchar* GetTexturePath(const Textures& textureModel);
const Model* GetModel(const Models& model);
const Model* GetTextureModel(const Textures& textureModel);
GLuint GetTexture(const Textures& textureModel);
