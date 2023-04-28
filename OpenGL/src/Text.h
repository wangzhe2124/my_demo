#pragma once
#include "ft2build.h"
#include FT_FREETYPE_H 
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <iostream>
struct Character {
    GLuint     TextureID;  // 字形纹理的ID
    glm::ivec2 Size;       // 字形大小
    glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
    GLuint     Advance;    // 原点距下一个字形原点的距离
};

static std::map<GLchar, Character> Characters;
static FT_Library ft;
static FT_Face face;
class TextFBO
{
private:
    
public:
    
    TextFBO()
    {
        Initialize_freetype();
        BindTexture();
    }
    static void Init()
    {
        Initialize_freetype();
        BindTexture();
    }
    static void BindTexture()
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制
        unsigned int texture;
        for (GLubyte c = 0; c < 128; c++)
        {
            unsigned int i = unsigned int(c);
            // 加载字符的字形 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // 生成纹理          
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // 设置纹理选项
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // 储存字符供之后使用
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            Characters.insert(std::pair<GLchar, Character>(c, character));
        }
        glDeleteTextures(1, &texture);      
    }
    static void Initialize_freetype()
    {
        if (FT_Init_FreeType(&ft))
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

        if (FT_New_Face(ft, "res/fonts/Avatarock.ttf", 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        FT_Set_Pixel_Sizes(face, 0, 48);//字体大小

        if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }
    ~TextFBO()
    {     
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
};