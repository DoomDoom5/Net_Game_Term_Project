#pragma warning(disable: 6031)	// 반환값이 무시되었습니다. 'fscanf'
#pragma warning(disable: 6054)	// 데이터의 문자열이 0으로 종료되지 않을 수 있습니다.
#include "stdafx.h"
#include "model.h"
#include "object.h"

#include <myGL/stb_image.h>

Model::Model(const GLchar* path)
{
	LoadModel(path);
}
Model::Model(const Model& origin)
{
	CopyVector(mVertices, origin.mVertices);
	CopyVector(mNormals, origin.mNormals);
	CopyVector(mUVs, origin.mUVs);
	CopyVector(mIndices_vertex, origin.mIndices_vertex);
	CopyVector(mIndices_normal, origin.mIndices_normal);
	CopyVector(mIndices_uv, origin.mIndices_uv);
	mVerticesXZ = origin.mVerticesXZ;

	if (origin.mCuboid != nullptr)
	{
		mCuboid = new Cuboid(*origin.mCuboid);
	}

	mWidth = origin.mWidth;
	mHeight = origin.mHeight;
	mDepth = origin.mDepth;
}

/* Warning : 법선 벡터가 unify되어있지 않을 경우 제대로 그려지지 않을 수 있음 */
/* ZBrush 모델 읽지 못함 */
GLvoid Model::LoadModel(const GLchar* path)
{
	string objPath = "obj\\";
	objPath += path;

	FILE* objFile = fopen(objPath.c_str(), "r");
	if (objFile == nullptr)
	{
		printf("Can't load objFile : %s", path);
		assert(0);
		return;
	}
	printf("Load Model : %s\n", path);

	GLfloat left = FLOAT_MAX;
	GLfloat right = FLOAT_MIN;

	GLfloat bottom = FLOAT_MAX;
	GLfloat top = FLOAT_MIN;

	GLfloat front = FLOAT_MAX;
	GLfloat back = FLOAT_MIN;

	set<size_t> set_overlapChecker_vertex;
	set<pair<size_t, size_t>> overlapChecker_normal;
	set<pair<size_t, size_t>> overlapChecker_uv;

	// 다음 element reading 시, 이전 element의 overlapCount만큼 vertex index를 증가시켜야 함.
	size_t overlapCount = 0;
	size_t beforeOverlapCount = 0;

	GLchar data[128];
	while (feof(objFile) == GL_FALSE)
	{
		fscanf(objFile, "%s", data);

		if (strcmp(data, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(objFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.y *= -1;
			std::swap(vertex.y, vertex.z);

			if (vertex.y < bottom)
			{
				bottom = vertex.y;
			}
			else if (vertex.y > top)
			{
				top = vertex.y;
			}

			if (vertex.x < left)
			{
				left = vertex.x;
			}
			else if (vertex.x > right)
			{
				right = vertex.x;
			}

			if (vertex.z < front)
			{
				front = vertex.z;
			}
			else if (vertex.z > back)
			{
				back = vertex.z;
			}

			mVertices.emplace_back(vertex);

			vertex.x = round(vertex.x * 100) / 100.0f;
			vertex.z = round(vertex.z * 100) / 100.0f;
			mVerticesXZ.insert(glm::vec2(vertex.x, vertex.z));
		}
		else if (strcmp(data, "vt") == 0)
		{
			glm::vec3 uv;
			fscanf(objFile, "%f %f %f\n", &uv.x, &uv.y, &uv.z);
			mUVs.emplace_back(glm::vec2(uv.x, uv.y));
		}
		else if (strcmp(data, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(objFile, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normal.y *= -1;
			std::swap(normal.y, normal.z);
			mNormals.emplace_back(normal);
		}
		else if (strcmp(data, "f") == 0)
		{
			size_t indices_vertex[3], indices_uv[3], indices_normal[3];
			for (size_t i = 0; i < 3; ++i)
			{
				fscanf(objFile, "%d/%d/%d", &indices_vertex[i], &indices_uv[i], &indices_normal[i]);
				indices_vertex[i] += beforeOverlapCount;
			}

			// [ 하나의 vertex에 다중 noraml, texture 적용 ] : 한 vertex에는 normal, texture 정보를 1:1 대응해야 함 -> 정점을 새로 추가해야함
			// 
			// vertex index가 중복될 경우
			// if	: normal index가 동일	-> 같은 normal 사용으로 문제 X		( pair("vertex Index", "normal index") 중복 검사 )
			// else : 다른 normal 사용		-> 정점 추가 필요
			//								-> 따라서, vertices에 vertex index에 있는 값 emplace_back, vertex index 를 마지막 인덱스[mVertices.size()]로 변경

			for (size_t i = 0; i < 3; ++i)
			{
				const size_t index_vertex = indices_vertex[i];

				// vertex index 중복 검사
				if (set_overlapChecker_vertex.find(index_vertex) == set_overlapChecker_vertex.end())
				{
					continue;
				}

				// normal index 중복 검사
				const size_t index_normal = indices_normal[i];
				const size_t index_uv = indices_uv[i];

				if (overlapChecker_normal.find(make_pair(index_vertex, index_normal)) != overlapChecker_normal.end())
				{
					// 한 vertex에 동일 normal 사용
					if (overlapChecker_uv.find(make_pair(index_vertex, index_uv)) != overlapChecker_uv.end())
					{
						continue; // 한 vertex에 동일 uv 사용
					}
				}

				// 한 vertex에 다른 normal or uv 사용

				const glm::vec3 newVertex = mVertices[index_vertex - 1];	// 현재 index에 있는 vertex 추출
				mVertices.emplace_back(newVertex);							// 마지막 원소에 추가
				indices_vertex[i] = mVertices.size();						// index를 마지막 원소로 지정
				++overlapCount;
			}
			

			for (size_t i = 0; i < 3; ++i)
			{
				set_overlapChecker_vertex.insert(indices_vertex[i]);
				overlapChecker_normal.insert(make_pair(indices_vertex[i], indices_normal[i]));
				overlapChecker_uv.insert(make_pair(indices_vertex[i], indices_uv[i]));

				mIndices_vertex.emplace_back(indices_vertex[i] - 1);
				mIndices_uv.emplace_back(indices_uv[i] - 1);
				mIndices_normal.emplace_back(indices_normal[i] - 1);
			}
		}
		else if (strcmp(data, "o") == 0)
		{
			// 새로운 object element -> set_overlapChecker 초기화
			set_overlapChecker_vertex.clear();
			overlapChecker_normal.clear();
			overlapChecker_uv.clear();
			beforeOverlapCount += overlapCount;
			overlapCount = 0;
		}
	}
	mWidth = right - left;
	mHeight = top - bottom;
	mDepth = back - front;
}
GLvoid Model::ReverseNormal()
{
	for (glm::vec3& normal : mNormals)
	{
		normal.x *= -1;
		normal.y *= -1;
		normal.z *= -1;
	}
}




// model
static Model* planeModel = new Model();
static Model* circleModel = new Model();
static Model* cubeModel = new Model();
static Model* lowSphereModel = new Model();
static Model* geoSphereModel = new Model();


// [ texture models ] //

// player
static Model* playerHeadModel = new Model();
static Model* playerBodyModel = new Model();
static Model* playerArmsModel = new Model();
static Model* playerLegLModel = new Model();
static Model* playerLegRModel = new Model();
static Model* gunModel = new Model();

// monster
static Model* blooperModel = new Model();
static Model* eggModel = new Model();
static Model* koromonModel = new Model();

// building
static Model* coreModel = new Model();

// turret
static Model* turretBodyModel = new Model();
static Model* turretHeadModel = new Model();

// other
static Model* mapModel = new Model();
static Model* cubeMapModel = new Model();
static Model* paintModel = new Model();
static Model* coinModel = new Model();


static GLuint textures[NUM_TEXTURE];

unordered_map<Models, Model*> modelMap{
	{Models::Plane, planeModel},
	{Models::Circle, circleModel},
	{Models::Cube, cubeModel},
	{Models::LowSphere, lowSphereModel},
	{Models::GeoSphere, geoSphereModel},
};
unordered_map<Textures, Model*> textureModelMap{
	{Textures::Gun, gunModel },
	{Textures::Map, mapModel},
	{Textures::CubeMap, cubeMapModel },
	{Textures::Blooper, blooperModel},
	{Textures::Egg, eggModel},
	{Textures::Koromon, koromonModel},
	{Textures::Core, coreModel},
	{Textures::Turret_Body, turretBodyModel },
	{Textures::Turret_Head, turretHeadModel },
	{Textures::Coin, coinModel },
	{Textures::Player_Head, playerHeadModel},
	{Textures::Player_Body, playerBodyModel},
	{Textures::Player_Arms, playerArmsModel},
	{Textures::Player_Leg_L, playerLegLModel},
	{Textures::Player_Leg_R, playerLegRModel},
};
unordered_map<Textures, const GLchar*> textureMap{
	{Textures::Gun, "gun.png" },
	{Textures::Map, "map.png" },
	{Textures::CubeMap, "cubemap.png" },
	{Textures::Blooper, "blooper.png" },
	{Textures::Egg, "egg.png" },
	{Textures::Koromon, "koromon.png" },
	{Textures::Core, "core.png" },
	{Textures::Paint, "paint.png" },
	{Textures::Paint2, "paint2.png" },
	{Textures::Turret_Body, "turret_body.png" },
	{Textures::Turret_Head, "turret_head.png" },
	{Textures::Coin, "coin.png" },
	{Textures::Player_Head, "player.png" },
	{Textures::Player_Body, "player.png" },
	{Textures::Player_Arms, "player.png" },
	{Textures::Player_Leg_L, "player.png" },
	{Textures::Player_Leg_R, "player.png" },
	{Textures::UI_NUM_0, "ui_num_0.png" },
	{Textures::UI_NUM_1, "ui_num_1.png" },
	{Textures::UI_NUM_2, "ui_num_2.png" },
	{Textures::UI_NUM_3, "ui_num_3.png" },
	{Textures::UI_NUM_4, "ui_num_4.png" },
	{Textures::UI_NUM_5, "ui_num_5.png" },
	{Textures::UI_NUM_6, "ui_num_6.png" },
	{Textures::UI_NUM_7, "ui_num_7.png" },
	{Textures::UI_NUM_8, "ui_num_8.png" },
	{Textures::UI_NUM_9, "ui_num_9.png" },
	{Textures::UI_TEXT_HP, "ui_text_HP.png" },
	{Textures::UI_TEXT_SLASH, "ui_text_slash.png" },
	{Textures::UI_TEXT_X, "ui_text_X.png" },
	{Textures::UI_COLOR_HP, "ui_HP_color.png" },
	{Textures::UI_RIFLE_SYMBOL, "ui_weapon_Rifle.png" },
	{Textures::UI_SHOTGUN_SYMBOL, "ui_weapon_Shotgun.png" },
	{Textures::UI_LAUNCHER_SYMBOL, "ui_weapon_Launcher.png" },
	{Textures::UI_SNIPER_SYMBOL, "ui_weapon_Sniper.png" },
	{Textures::UI_TURRET_SYMBOL, "ui_Turret_symbol.png" },
	{Textures::UI_GAME_OVER, "ui_game_over.png" },

};

/* Should be arrange by obj file size (faster) */
enum class ObjList { Coin, Gun, Egg, Turret_Head, Blooper, Turret_Body, Koromon, GeoSphere, Circle, LowSphere, Cube, Core, Map, Plane, Player_Head, Player_Body, Player_Arms, Player_Leg_L, Player_Leg_R,_count };
constexpr GLuint NUM_OBJ = static_cast<GLuint>(ObjList::_count);

unordered_map<ObjList, pair<Model*, const GLchar*>> objMap{
	{ObjList::Gun, make_pair(gunModel, "gun.obj")},
	{ObjList::Map, make_pair(mapModel, "map.obj")},
	{ObjList::Blooper, make_pair(blooperModel, "blooper.obj")},
	{ObjList::Egg, make_pair(eggModel, "egg.obj")},
	{ObjList::Koromon, make_pair(koromonModel, "koromon.obj")},
	{ObjList::Coin, make_pair(coinModel, "coin.obj")},
	{ObjList::Core, make_pair(coreModel, "core.obj")},
	{ObjList::Player_Head, make_pair(playerHeadModel, "player_head.obj")},
	{ObjList::Player_Body, make_pair(playerBodyModel, "player_body.obj")},
	{ObjList::Player_Arms, make_pair(playerArmsModel, "player_arms.obj")},
	{ObjList::Player_Leg_L, make_pair(playerLegLModel, "player_leg_l.obj")},
	{ObjList::Player_Leg_R, make_pair(playerLegRModel, "player_leg_r.obj")},
	{ObjList::GeoSphere, make_pair(geoSphereModel, "geo_sphere.obj")},
	{ObjList::Circle, make_pair(circleModel, "circle.obj")},
	{ObjList::LowSphere, make_pair(lowSphereModel, "low_sphere.obj")},
	{ObjList::Cube, make_pair(cubeModel, "cube.obj")},
	{ObjList::Plane, make_pair(planeModel, "plane.obj")},
	{ObjList::Turret_Body, make_pair(turretBodyModel, "turret_body.obj")},
	{ObjList::Turret_Head, make_pair(turretHeadModel, "turret_head.obj")},
};



GLvoid ImportObj(mutex& m, unordered_set<GLuint>& emptyCore, const GLuint& id, const ObjList& obj)
{
	Model* model = objMap[obj].first;
	const GLchar* path = objMap[obj].second;
	model->LoadModel(path);

	m.lock();
	emptyCore.insert(id);
	m.unlock();
}

typedef struct ImageData {
	GLint width, height, numOfChannel;
}ImageData;
GLvoid ImportTextureData(mutex& m, unordered_set<GLuint>& emptyCore, const GLuint& id, const Textures& textureModel, ImageData& imageData, GLubyte*& data)
{
	string path = "textures\\";
	path += textureMap[textureModel];

	data = stbi_load(path.c_str(), &imageData.width, &imageData.height, &imageData.numOfChannel, 0);

	if (stbi_failure_reason())
	{
		cout << "[ stbi_failure ] : " << stbi_failure_reason() << endl;
		cout << "[ path ] : " << path << endl;
		assert(0);
	}

	if (!data)
	{
		cout << "Failed to load texture : " << path << endl;
		assert(0);
	}

	m.lock();
	emptyCore.insert(id);
	m.unlock();
}

// 12-02 benchmark
// [ obj loading ]
// core 1	: 6900ms
// core 2	: 4100ms
// core 16	: 4100ms
GLvoid InitModels()
{
	auto start = chrono::high_resolution_clock::now();

	mutex m;
	unordered_set<GLuint> emptyCore;
	vector<thread*> threads;
	threads.resize(NUM_CORE);

	for (size_t i = 0; i < threads.size(); ++i)
	{
		emptyCore.insert(i);
	}

	/* Import obj */
	for (GLuint i = 0; i < NUM_OBJ; ++i)
	{
		// find empty core id
		GLuint id = -1;
		while(GL_TRUE)
		{
			if (FindEmptyCoreID(m, emptyCore, id) == GL_TRUE)
			{
				break;
			}
		}

		if (threads[id] != nullptr)
		{
			threads[id]->join();
		}
		ObjList obj = static_cast<ObjList>(i);
		threads[id] = new thread(ref(ImportObj), ref(m), ref(emptyCore), id, obj);
	}

	/* Import texture */
	stbi_set_flip_vertically_on_load(GL_TRUE);

	glGenTextures(NUM_TEXTURE, textures);
	GLubyte* textureDataList[NUM_TEXTURE];
	ImageData imageDataList[NUM_TEXTURE];

	cout << "load textures..." << endl;
	for (GLsizei i = 0; i < NUM_TEXTURE; ++i)
	{
		// find empty core id
		GLuint id = -1;
		while (GL_TRUE)
		{
			if (FindEmptyCoreID(m, emptyCore, id) == GL_TRUE)
			{
				break;
			}
		}

		if (threads[id] != nullptr)
		{
			threads[id]->join();
		}
		Textures textureModel = static_cast<Textures>(i);
		threads[id] = new thread(ref(ImportTextureData), ref(m), ref(emptyCore), id, textureModel, ref(imageDataList[i]), ref(textureDataList[i]));
	}

	for (thread* t : threads)
	{
		if (t != nullptr)
		{
			t->join();
			t = nullptr;
		}
	}
	// end of thread

	for (GLsizei i = 0; i < NUM_TEXTURE; ++i)
	{
		GLubyte* data = textureDataList[i];
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageDataList[i].width, imageDataList[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}

	textureModelMap[Textures::CubeMap] = cubeModel;
	textureModelMap[Textures::CubeMap]->ReverseNormal();
	textureModelMap[Textures::Paint] = planeModel;
	textureModelMap[Textures::Paint2] = planeModel;

	for(GLint i=0;;++i)
	{
		GLint uiIndex = static_cast<GLint>(Textures::UI_NUM_0) + i;
		Textures texture = static_cast<Textures>(uiIndex);
		if (texture == Textures::_count)
		{
			break;
		}
		textureModelMap[texture] = planeModel;
	}

	auto duration = chrono::high_resolution_clock::now() - start;
	cout << "Model loading time : " << chrono::duration_cast<chrono::milliseconds>(duration).count() << "ms" << endl;

	system("cls");
}

const Model* GetModel(const Models& model)
{
	const Model* result = modelMap[model];
	assert(result != nullptr);

	return result;
}
const Model* GetTextureModel(const Textures& textureModel)
{
	const Model* result = textureModelMap[textureModel];
	assert(result != nullptr);

	return result;
}
GLuint GetTexture(const Textures& textureModel)
{
	return textures[static_cast<GLuint>(textureModel)];
}



glm::vec3 Model::GetVertex(const size_t& index) const
{
	assert(index <= mVertices.size());

	return mVertices[index];
}

Cuboid* Model::GetCuboid(const glm::vec3* position, const glm::vec3* scale) const
{
	return new Cuboid(position, scale, mWidth, mHeight, mDepth);
}


const GLchar* GetTexturePath(const Textures& textureModel)
{
	return textureMap[textureModel];
}