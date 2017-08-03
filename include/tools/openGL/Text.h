#pragma once
#include <map>
#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H
// glad
#include <glad\glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <openGL/Shader.h>

class Text
{
public:
	// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
		GLuint Advance;    // Horizontal offset to advance to next glyph
	};
	typedef struct
	{
		std::string data;
		glm::ivec2 coord;
		glm::vec3 color;
		float scale;
	}TEXT_CONTEXT;
	Text(Shader* shader,float width, float height);
	~Text();
	void addContext(std::string name, TEXT_CONTEXT& context);//if name exsit,override
	TEXT_CONTEXT* Context(std::string name);
	void RenderTotalText();
private:
	void init();
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	std::map<GLubyte, Character> m_mapCharacters;
	Shader* m_hShader;
	GLuint m_VAO;
	GLuint m_VBO;
	std::map<std::string, TEXT_CONTEXT*>m_mapContext;
};