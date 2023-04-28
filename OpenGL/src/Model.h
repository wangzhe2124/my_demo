#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "vender/stb_image/stb_image.h"
#include "opengl_Mesh.h"
#include "ModelSpace.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Bone.h"
#include <vector>
#include <map>
#include <thread>

using namespace std;
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
enum {min_x, max_x, min_y, max_y, min_z, max_z};
class Model
{
public:
    // model data 
    vector<MeshTexture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    std::string name;
    int index;
    vector<float> aabb = vector<float>(6);
    vector<glm::vec3> aabb_vertex;
    bool inFrustum;
    bool isMoved;
    GLuint query;
    GLuint queryPassed;
    string directory;
    ModelSpace position;
    float max_life;
    float current_life;
    VertexArray* va;
    IndexBuffer* ib;
    // constructor, expects a filepath to a 3D model.
    Model(const std::string& path, const std::string& n,  float Life = 100.0f) : va(nullptr), ib(nullptr), max_life(Life), current_life(Life), isMoved(false), name(n)
    {
        Initialize_aabb();
        loadModel(path);
        /*AABB();*/
    }
    void Get_index(const unsigned int* data, unsigned int count) 
    {
        ib = new IndexBuffer(data, count);
    }
    Model(const std::string& n, float Life = 100.0f) : ib(nullptr), max_life(Life), current_life(Life), isMoved(false), name(n)
    {
        Initialize_aabb();
        /*AABB();*/
        va = new VertexArray();
    }
    Model& operator=(const Model& m)
    {
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
    Model(const Model& m)
    {
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
    }
    // draws the model, and thus all its meshes
    virtual ~Model()
    {
        delete va;
        delete ib;
    }
    template<typename T>
    void Draw(T& shader)
    {
        shader.Bind();
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    template<typename T>
    void DrawShadow(T& shader)
    {
        shader.Bind();
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].DrawShadow(shader);
    }
    void Initialize_aabb()
    {
        aabb[min_x] = aabb[min_y] = aabb[min_z] = FLT_MAX;
        aabb[max_x] = aabb[max_y] = aabb[max_z] = -FLT_MAX;
    }
    void intializeAABB()
    {
        aabb_vertex = {
        glm::vec3(aabb[min_x], aabb[min_y],aabb[min_z]),
        glm::vec3(aabb[max_x], aabb[min_y],aabb[min_z]),

        glm::vec3(aabb[max_x], aabb[min_y],aabb[max_z]),
        glm::vec3(aabb[min_x], aabb[min_y],aabb[max_z]),

        glm::vec3(aabb[min_x], aabb[max_y],aabb[min_z]),
        glm::vec3(aabb[max_x], aabb[max_y],aabb[min_z]),

        glm::vec3(aabb[max_x], aabb[max_y],aabb[max_z]),
        glm::vec3(aabb[min_x], aabb[max_y],aabb[max_z]),
        };
    }
    void updateAABB() //update
    {
        Initialize_aabb();
        for (int i = 0; i < aabb_vertex.size(); i++)
        {
            glm::vec4 temp = glm::vec4(position.GetModelSpace() * glm::vec4(aabb_vertex[i], 1.0f));  
            if (temp.x > aabb[max_x])
                aabb[max_x] = temp.x;
            else if (temp.x < aabb[min_x])
                aabb[min_x] = temp.x;

            if (temp.y > aabb[max_y])
                aabb[max_y] = temp.y;
            else if (temp.y < aabb[min_y])
                aabb[min_y] = temp.y;

            if (temp.z > aabb[max_z])
                aabb[max_z] = temp.z;
            else if (temp.z < aabb[min_z])
                aabb[min_z] = temp.z;
        }

    }
    void updateAABB(glm::mat4& rootTrans) // initialize
    {
        Initialize_aabb();
        for (int i = 0; i < aabb_vertex.size(); i++)
        {
            aabb_vertex[i] = glm::vec3(rootTrans * glm::vec4(aabb_vertex[i], 1.0f));
            glm::vec4 temp = glm::vec4(position.GetModelSpace() *  glm::vec4(aabb_vertex[i], 1.0f));  
            if (temp.x > aabb[max_x])
                aabb[max_x] = temp.x;
            else if (temp.x < aabb[min_x])
                aabb[min_x] = temp.x;

            if (temp.y > aabb[max_y])
                aabb[max_y] = temp.y;
            else if (temp.y < aabb[min_y])
                aabb[min_y] = temp.y;

            if (temp.z > aabb[max_z])
                aabb[max_z] = temp.z;
            else if (temp.z < aabb[min_z])
                aabb[min_z] = temp.z;
        }

    }
    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    std::map<string, BoneInfo> m_BoneInfoMap; //
    int m_BoneCounter = 0;  
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }
    void SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<MeshTexture> textures;

        // walk through each of the mesh's vertices
        
        auto f1 = [&]() {
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                SetVertexBoneDataToDefault(vertex);
                glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
                // positions
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;
                if (vector.x > aabb[max_x])
                    aabb[max_x] = vector.x;
                else if (vector.x < aabb[min_x])
                    aabb[min_x] = vector.x;

                if (vector.y > aabb[max_y])
                    aabb[max_y] = vector.y;
                else if (vector.y < aabb[min_y])
                    aabb[min_y] = vector.y;

                if (vector.z > aabb[max_z])
                    aabb[max_z] = vector.z;
                else if (vector.z < aabb[min_z])
                    aabb[min_z] = vector.z;

                if (mesh->HasNormals())
                {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.Normal = vector;
                }
                // texture coordinates
                if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
                {
                    glm::vec2 vec;
                    // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                    // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = vec;
                    // tangent
                    vector.x = mesh->mTangents[i].x;
                    vector.y = mesh->mTangents[i].y;
                    vector.z = mesh->mTangents[i].z;
                    vertex.Tangent = vector;
                    // bitangent
                    vector.x = mesh->mBitangents[i].x;
                    vector.y = mesh->mBitangents[i].y;
                    vector.z = mesh->mBitangents[i].z;
                    vertex.Bitangent = vector;
                }
                else
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);

                vertices.push_back(vertex);
            }
        };
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        auto f2 = [&]() {
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                // retrieve all indices of the face and store them in the indices vector
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }
        };
        // process materials
        auto f3 = [&]() {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN

            // 1. diffuse maps
            vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<MeshTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        };
        f1();
        f2();
        f3();

        ExtractBoneWeightForVertices(vertices, mesh, scene);
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }
    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {

        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = m_BoneCounter;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
                    mesh->mBones[boneIndex]->mOffsetMatrix);
                m_BoneInfoMap[boneName] = newBoneInfo;
                
                boneID = m_BoneCounter;

                m_BoneCounter++;
            }
            else
            {
                boneID = m_BoneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<MeshTexture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                MeshTexture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};

static unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format{};
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        GLenum outformat{};
        if (gamma)
            outformat = GL_RGBA;
        else
            outformat = GL_SRGB_ALPHA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, outformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

struct sphere_data
{
    std::vector<float> vertex;
    std::vector<unsigned int> index;
};
static sphere_data SphereData()
{
    std::vector<glm::vec3> positions;   
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uv;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    unsigned int indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }
    sphere_data Data = { data, indices };
    return Data;
}

struct TerrainData
{
    std::vector<float> vertex;
    std::vector<unsigned int> index;
    int numStrips;
    int numTrisPerStrip;
    unsigned int rez;
};
//static TerrainData Get_TerrainData_cpu() //without uv
//{
//    TerrainData terrain_data;
//    //stbi_set_flip_vertically_on_load(true);
//    int width, height, nrChannels;
//    unsigned char* data = stbi_load("res/textures/iceland_heightmap.png", &width, &height, &nrChannels, 0);
//    if (data)
//    {
//        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
//    }
//    else
//    {
//        std::cout << "Failed to load texture" << std::endl;
//    }
//
//
//    // set up vertex data (and buffer(s)) and configure vertex attributes
//    // ------------------------------------------------------------------
//    float yScale = 64.0f / 256.0f, yShift = 16.0f;
//    int rez = 1;
//    unsigned int bytePerPixel = nrChannels;
//    for (int i = 0; i < height; i++)
//    {
//        for (int j = 0; j < width; j++)
//        {
//            unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
//            unsigned char y = pixelOffset[0];
//
//            // vertex
//            terrain_data.vertex.push_back(-height / 2.0f + height * i / (float)height);   // vx
//            terrain_data.vertex.push_back((int)y * yScale - yShift);   // vy
//            terrain_data.vertex.push_back(-width / 2.0f + width * j / (float)width);   // vz
//        }
//    }
//    std::cout << "Loaded " << terrain_data.vertex.size() / 3 << " vertices" << std::endl;
//    stbi_image_free(data);
//
//    for (int i = 0; i < height - 1; i += rez)
//    {
//        for (int j = 0; j < width; j += rez)
//        {
//            for (int k = 0; k < 2; k++)
//            {
//                terrain_data.index.push_back(j + width * (i + k * rez));
//            }
//        }
//    }
//    std::cout << "Loaded " << terrain_data.index.size() << " indices" << std::endl;
//
//    terrain_data.numStrips = (height - 1) / rez;
//    terrain_data.numTrisPerStrip = (width / rez) * 2;
//    std::cout << "Created lattice of " << terrain_data.numStrips << " strips with " << terrain_data.numTrisPerStrip << " triangles each" << std::endl;
//    std::cout << "Created " << terrain_data.numStrips * terrain_data.numTrisPerStrip << " triangles total" << std::endl;
//    return terrain_data;
//}
static TerrainData Get_TerrainData_gpu(int width, int height)
{
    TerrainData terrain_data;
    std::vector<float>& vertices = terrain_data.vertex;
    unsigned rez = 40;
    terrain_data.rez = rez;
    for (unsigned i = 0; i <= rez - 1; i++)
    {
        for (unsigned j = 0; j <= rez - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v
        }
    }
    //std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
    //std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;
    return terrain_data;
}

