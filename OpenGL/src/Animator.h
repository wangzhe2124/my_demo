#pragma once
#include <glm/glm.hpp>
#include "Model.h"
struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
private:
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	void ReadMissingBones(const aiAnimation* animation, Model& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		dest.name = src->mName.data;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (unsigned int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
public:
	Animation() = default;

	Animation(std::string const& animationPath, Model* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_Duration = static_cast<float>(animation->mDuration);
		std::fstream out;

		m_TicksPerSecond = static_cast<int>(animation->mTicksPerSecond);


		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}

	~Animation()
	{
	}

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}


	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}
};

class Animations
{
public:
	static enum {idle, walk, jog, left_punch, right_punch, MAX};
	Animation* Animation_jog;
	Animation* Animation_left_punch;
	Animation* Animation_walk;
	Animation* Animation_right_punch;
	Animation* Animation_Idle;
	std::unordered_map<int, Animation*> animations_map;
	Animations(Model* model)
	{
		Animation_jog = new Animation("res/objects/Jogging.dae", model);
		animations_map[jog] = Animation_jog;
		Animation_left_punch = new Animation("res/objects/leftPunch.dae", model);
		animations_map[left_punch] = Animation_left_punch;
		Animation_walk = new Animation("res/objects/Walk.dae", model);
		animations_map[walk] = Animation_walk;
		Animation_right_punch = new Animation("res/objects/rightPunch.dae", model);
		animations_map[right_punch] = Animation_right_punch;
		Animation_Idle = new Animation("res/objects/Idle.dae", model);
		animations_map[idle] = Animation_Idle;
	}
	~Animations()
	{
		for (std::unordered_map<int, Animation*>::iterator iter = animations_map.begin(); iter != animations_map.end(); iter++)
			delete iter->second;
	}
};
class Animator
{
private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
	float Duration;
public:
	glm::mat4 RootTransform;
	Animator(Animation* animation = nullptr, bool is_root = false)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		if(m_CurrentAnimation)
			Duration = m_CurrentAnimation->GetDuration();
	}
	~Animator()
	{
	}
	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
			for (glm::mat4& m1 : m_FinalBoneMatrices)
			{
				m1[3][2] -= RootTransform[3][2];
			}
		}
	}
	float GetCurrentTime()
	{
		return m_CurrentTime;
	}
	float GetDuration()
	{
		return Duration;
	}
	void SetCurrentTime(float time)
	{
		m_CurrentTime = time;
	}
	float GetRatio()
	{
		return m_CurrentTime / m_CurrentAnimation->GetDuration();
	}
	void PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
		if(m_CurrentAnimation)
			Duration = m_CurrentAnimation->GetDuration();
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;
		//std::cout << nodeName << std::endl;
		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}


		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
			if (index == 0)
			{
				RootTransform = m_FinalBoneMatrices[index];
			}
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}
};

class animeModel : public Model
{
public:
	Animator* animator;
	animeModel(const std::string& path,const std::string& n, bool anime = false, float Life = 100.0f) : Model(path, n, Life), animator(nullptr)
	{
		if (anime)
			animator = new Animator();
	}
	animeModel(const std::string& n, float Life = 100.0f) : Model(n, Life), animator(nullptr)
	{

	}
	~animeModel()
	{
		if (animator)
		{
			delete animator;
			animator = nullptr;
		}
	}
	animeModel& operator=(const animeModel& m)
	{
		animator = new Animator();
		textures_loaded = m.textures_loaded;
		meshes = m.meshes;
		name = m.name;
		aabb = m.aabb;
		aabb_vertex = m.aabb_vertex;
		isMoved = m.isMoved;
		directory = m.directory;
		position = m.position;
		max_life = m.max_life;
		current_life = m.current_life;
		va = m.va;
		ib = m.ib;
		return *this;
	}
	animeModel(const animeModel& m) : Model(m)
	{
		animator = new Animator();
	}
};

class Models
{
private:

public:

	std::set<animeModel*> models_map;
	animeModel Nano = animeModel("res/objects/nanosuit_upgrade/nanosuit.obj", "Nano");
	animeModel Marry = animeModel("res/objects/Marry/Marry.obj", "Marry");
	animeModel Planet = animeModel("res/objects/planet/planet.obj", "Planet");
	animeModel Rock = animeModel("res/objects/rock/rock.obj", "Rock");
	animeModel Floor = animeModel("Floor");
	animeModel Sphere = animeModel("sphere");
	animeModel Terrain = animeModel("terrain");
	animeModel Main_character = animeModel("res/objects/arissa/arissa.dae", "Main_character", true);

	animeModel Robot_boxing = Main_character;
	animeModel Robot_pray = Main_character;
	animeModel Robot_death = Main_character;
	animeModel Robot_walk = Main_character;
	void Get_models()
	{
		models_map.insert(&Nano);
		models_map.insert(&Marry);
		models_map.insert(&Planet);
		models_map.insert(&Rock);

		models_map.insert(&Main_character);
		models_map.insert(&Robot_boxing);
		models_map.insert(&Robot_pray);
		models_map.insert(&Robot_death);
		models_map.insert(&Robot_walk);
		int i = 0;
		for (std::set<animeModel*>::iterator it = models_map.begin(); it != models_map.end(); it++)
		{
			animeModel* m = *it;
			m->index = i;
			i++;
		}
		/*Nano: 0
		Marry : 1
		Planet : 2
		Rock : 3
		Floor : 4
		sphere : 5
		terrain : 6
		Main_character : 7
		Main_character : 8
		Main_character : 9
		Main_character : 10
		Main_character : 11*/
	}
	void Get_anime_models()
	{

	}
	Models()
	{
		Get_models();
		Get_anime_models();
	}
	~Models()
	{

	}
};
