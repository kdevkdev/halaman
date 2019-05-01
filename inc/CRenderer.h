#ifndef CRENDERER_H
#define CRENDERER_H

#include "defhelper.h"

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <ft2build.h>
#include FT_FREETYPE_H

#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "CShader.h"

using namespace std;
using namespace glm;


struct scharacter {
    GLuint texture_id;  // ID handle of the glyph texture
    ivec2 size;       // Size of glyph
    ivec2 bearing;    // Offset from baseline to left/top of glyph
    long int advance;    // Offset to advance to next glyph
};

class CRenderer
{
    public:
        CRenderer(int screen_width, int screen_height, int max_cubes);
        virtual ~CRenderer();


        void draw_cube(int x, int y, mat4 view, vec3 cam_position);
        void draw_instanced_cubes(vector<float> cube_data, int cube_count,  mat4 view, vec3 cam_position);
        void draw_crosshair(void);

        void display(void);


        void draw_text(string text, float x, float y, vec3 color, GLfloat scale);

        vec3 pick_world_coordinates(fvec3 camera_pos, vec3 cameara_front);

        // introduce possibility to change the  projection matrix to enable easy viewport rendering.
        void set_text_projection(float x, float y, float width, float height);

        // restores fullscreen viewport
        void reset_text_projection(void);


        // handle window events
        void handle_window_events( SDL_Event& e );

        // go or leave fullscreen
        void switch_fullscreen(void);
        void switch_mouserelative(void);
        bool get_mouserelative();

        int get_screen_height(void);
        int get_screen_width(void);

    private:

        // SDL window & renderer, context
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_GLContext m_glcontext;;


        //Window focus
		bool m_mouse_focus;
		bool m_keyboard_focus;
		bool m_fullscreen;
		bool m_minimized;
		bool m_mouserelative;


        CShader * m_cube_shader;
        CShader * m_font_shader;
        CShader * m_simple_shader;

        GLuint m_text_VAO;
        GLuint m_text_VBO;
        GLuint m_VAO;
        GLuint m_VBO;
        GLuint m_EBO;
        GLuint m_instance_VBO;
        GLuint m_simple_VAO;
        GLuint m_simple_VBO;
        GLuint m_simple_instance_VBO;


        int m_screen_width;
        int m_screen_height;

        int m_max_cubes;

        mat4 m_ui_projection;
        mat4 m_text_projection;

        FT_Library m_ft;
        FT_Face m_face;

        map<GLchar, scharacter> m_characters;


        // 3 coordinates+normals for 6 surfaces with 2 triangles each
        GLfloat m_cube_vertices[12*18];

        // 6 surfaces with each two triangles a 3 vertices
        GLuint m_cube_indices[36];

        // crosshair (3 coordinates + 3 floats for color)
        GLfloat m_crosshair_vertices[4*6];


        mat4 m_model;
        mat4 m_projection;
        mat4 m_inverse_model;


};

#endif // CRENDERER_H
