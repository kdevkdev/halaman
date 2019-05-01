#include "CRenderer.h"



// set cube vertices and indices
CRenderer::CRenderer(int screen_width, int screen_height, int max_cubes) :
m_cube_vertices
{
    -0.5f, -0.5f, -0.5f, 0, 0, -1.0f,// back 1
     0.5f, -0.5f, -0.5f, 0, 0, -1.0f,
     0.5f,  0.5f, -0.5f, 0, 0, -1.0f,

    -0.5f, -0.5f, -0.5f, 0, 0, -1.0f,// back 2
     0.5f,  0.5f, -0.5f, 0, 0, -1.0f,
    -0.5f,  0.5f, -0.5f, 0, 0, -1.0f,

    -0.5f, -0.5f, -0.5f, -1.0f, 0, 0,// left 1
    -0.5f,  0.5f, -0.5f, -1.0f, 0, 0,
    -0.5f,  0.5f,  0.5f, -1.0f, 0, 0,

    -0.5f, -0.5f, -0.5f, -1.0f, 0, 0,// left 2
    -0.5f, -0.5f,  0.5f, -1.0f, 0, 0,
    -0.5f,  0.5f,  0.5f, -1.0f, 0, 0,

    -0.5f, -0.5f, -0.5f, 0, -1.0f, 0, // bottom 1
     0.5f, -0.5f, -0.5f, 0, -1.0f, 0,
     0.5f, -0.5f,  0.5f, 0, -1.0f, 0,

    -0.5f, -0.5f, -0.5f, 0, -1.0f, 0,// bottom 2
    -0.5f, -0.5f,  0.5f, 0, -1.0f, 0,
     0.5f, -0.5f,  0.5f, 0, -1.0f, 0,

     0.5f, -0.5f, -0.5f, 1.0f, 0, 0, // right 1
     0.5f,  0.5f, -0.5f, 1.0f, 0, 0,
     0.5f,  0.5f,  0.5f, 1.0f, 0, 0,

     0.5f, -0.5f, -0.5f, 1.0f, 0, 0,// right 2
     0.5f, -0.5f,  0.5f, 1.0f, 0, 0,
     0.5f,  0.5f,  0.5f, 1.0f, 0, 0,

     0.5f,  0.5f, -0.5f, 0, 1.0f, 0, // top 1
    -0.5f,  0.5f, -0.5f, 0, 1.0f, 0,
     0.5f,  0.5f,  0.5f, 0, 1.0f, 0,

    -0.5f,  0.5f, -0.5f, 0, 1.0f, 0, // top 2
     0.5f,  0.5f,  0.5f, 0, 1.0f, 0,
    -0.5f,  0.5f,  0.5f, 0, 1.0f, 0,

    -0.5f, -0.5f,  0.5f, 0, 0, 1.0f, // front 1
     0.5f, -0.5f,  0.5f, 0, 0, 1.0f,
    -0.5f,  0.5f,  0.5f, 0, 0, 1.0f,

     0.5f, -0.5f,  0.5f, 0, 0, 1.0f,// front 2
     0.5f,  0.5f,  0.5f, 0, 0, 1.0f,
    -0.5f,  0.5f,  0.5f, 0, 0, 1.0f
},
m_cube_indices
{
    0, 1, 2, // back
    0, 2, 3,
    0, 3, 7, // left
    0, 4, 7,
    0, 1, 5, // bottom
    0, 4, 5,
    1, 2, 6, // right
    1, 5, 6,
    2, 3, 6, // top
    3, 6, 7,
    4, 5, 7, // front
    5, 6, 7,
},
m_crosshair_vertices // in pixel coordinates
{
    -CROSSHAIR_LENGTH,    0.00f, 0.0f, 0.0f, 1.0f, 1.0f,
     CROSSHAIR_LENGTH,  0.00f, 0.0f, 0.0f, 1.0f, 1.0f,
     0.00f, -CROSSHAIR_LENGTH, 0.0f, 0.0f, 1.0f, 1.0f,
     0.00f,  CROSSHAIR_LENGTH, 0.0f, 0.0f, 1.0f, 1.0f
}
{
    m_window = nullptr;
    m_screen_width = screen_width;
    m_screen_height = screen_height;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create window
        m_window = SDL_CreateWindow( "halaman", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screen_width, m_screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if( m_window == nullptr )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            return;
        }

        m_mouse_focus = true;
		m_keyboard_focus = true;
		m_fullscreen = false;


        // create sdl renderer
        m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED );
        if( m_renderer == nullptr)
        {
            printf( "Sdl renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
            return;
        }
        SDL_SetRenderDrawColor(m_renderer,0xFF,0xFF,0xFF,0xFF);


        // do opengl stuff for sdl2
        if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0)
        {
            printf( "Failure setting gl context attributes! SDL_Error: %s\n", SDL_GetError() );
            return;
        }

        if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) < 0)
        {
            printf( "Failure setting gl major version! SDL_Error: %s\n", SDL_GetError() );
            return;
        }

        if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) < 0)
        {
            printf( "Failure setting gl minor version! SDL_Error: %s\n", SDL_GetError() );
            return;
        }

        if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0)
        {
            printf( "Failure setting gl double buffering! SDL_Error: %s\n", SDL_GetError() );
            return;
        }


        m_glcontext = SDL_GL_CreateContext(m_window);
        if(!m_glcontext)
        {
            printf( "Failure creating gl context! SDL_Error: %s\n", SDL_GetError() );
            return;
        }

        if(SDL_GL_SetSwapInterval(1)<0)
        {
            printf( "Failure setting swap interval! SDL_Error: %s\n", SDL_GetError() );
            return;
        }

        // initialize glew
        glewExperimental = GL_TRUE;
        glewInit();

        // blue clear color
        glClearColor(0.0, 0.0, 1.0, 1.0);


        //#############################################################################################################

        // load shaders
        m_cube_shader = new CShader("resources/vertex_instance_shader.vs", "resources/fragment_shader.fs");
        m_font_shader = new CShader("resources/font_vertex_shader.vs","resources/font_fragment_shader.fs");
        m_simple_shader = new CShader("resources/simple_vertex_shader.vs","resources/simple_fragment_shader.fs");

        // set mouse relative mode
        SDL_SetRelativeMouseMode(SDL_TRUE);
        m_mouserelative = true;

        // Enable blending for font rendering
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // initialize freetype
        if (FT_Init_FreeType(&m_ft))
        {
            printf("probem initializing freetype\n");
        }
        if (FT_New_Face(m_ft, "resources/mplus-1m-regular.ttf", 0, &m_face))
        {
            printf("probem loading font\n");
        }
        // disable opengl textuure aligment
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        FT_Set_Pixel_Sizes(m_face, 0, 48);

        for (GLubyte c = 0; c < 128; c++)
        {
            // Load character glyph
            if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
            {
                printf("failed to load glyph %d with freetype\n",c);
                continue;
            }
             // Generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                m_face->glyph->bitmap.width,
                m_face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                m_face->glyph->bitmap.buffer
            );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Now store character for later use
            scharacter character = {
                texture,
                ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
                ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
                m_face->glyph->advance.x
            };
            m_characters.insert(pair<GLchar, scharacter>(c, character));
        }

        // enable depth testing
        glEnable(GL_DEPTH_TEST);


      // text VAO and VBO
        glGenVertexArrays(1, &m_text_VAO);
        glGenBuffers(1, &m_text_VBO);
        glBindVertexArray(m_text_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_text_VBO);
        // set for rendereing quads (6 vertices with 4 floats)
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);



        // load stuff for cube rendering
        glGenBuffers(1, &m_VBO);
        //glGenBuffers(1, &m_EBO);

        glGenVertexArrays(1, &m_VAO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_cube_vertices), m_cube_vertices, GL_STREAM_DRAW);



        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_cube_indices), m_cube_indices, GL_STATIC_DRAW);

        // bind position to first slot, normal to second slot (3 floats each)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // instanced rendering (with different buffer)
        glBindVertexArray(m_VAO);
        glGenBuffers(1, &m_instance_VBO);
        m_max_cubes = max_cubes;
        glBindBuffer(GL_ARRAY_BUFFER, m_instance_VBO);
        // instanced cube position is 3th slot, cube color 4th slot (but different buffer)
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(3);


        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        // set up the buffers for the simple shader
        // load stuff for cube rendering
        glGenBuffers(1, &m_simple_VBO);
        //glGenBuffers(1, &m_simple_instance_VBO);

        glGenVertexArrays(1, &m_simple_VAO);
        glBindVertexArray(m_simple_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_simple_VBO);



        // bind position to first slot, color to 2nd  slot (3 floats each)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // bind instance position in other vbo buffer to 3rd slot, set attrib divisors
        /*glBindBuffer(GL_ARRAY_BUFFER, m_simple_instance_VBO);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1); */

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        // calculate projection matrix
        m_projection = perspective(radians(FOV_ANGLE), (float)m_screen_width / m_screen_height, NEAR_PLANE, FAR_PLANE);

        // ui projection matrix
        m_text_projection = ortho(0.0f, (float) m_screen_width, 0.0f, (float) m_screen_height);
        m_ui_projection = ortho(0.0f, (float) m_screen_width, 0.0f, (float) m_screen_height);


        // and the inverse model matrix
        m_model = mat4(1);
        m_inverse_model = inverse(m_model);

        // unload freetype
        FT_Done_Face(m_face);
        FT_Done_FreeType(m_ft);

    }
}
CRenderer::~CRenderer()
{
    delete m_cube_shader;
    delete m_font_shader;
    delete m_simple_shader;

     // destroy context
     SDL_GL_DeleteContext(m_glcontext);

     // destroy renderer
     SDL_DestroyRenderer(m_renderer);

     //Destroy
     SDL_DestroyWindow( m_window );

     //Quit SDL subsystems
     SDL_Quit();
}
int CRenderer::get_screen_height(void)
{
    return m_screen_height;
}
int CRenderer::get_screen_width(void)
{
    return m_screen_width;
}
void CRenderer::switch_fullscreen(void)
{
        // inspired by lazyfoos tutorial
		if(m_fullscreen)
		{
			SDL_SetWindowFullscreen( m_window, SDL_FALSE );
			m_fullscreen= false;
		}
		else
		{
			SDL_SetWindowFullscreen( m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			m_fullscreen = true;
			m_minimized = false;
		}
}
void CRenderer::switch_mouserelative()
{
    m_mouserelative = !m_mouserelative;
    if(m_mouserelative)
    {
        // disable mouse cursor, set relative mouse mode
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    else
    {
       SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}
bool CRenderer::get_mouserelative(void)
{
    return m_mouserelative;
}
void CRenderer::handle_window_events(SDL_Event& e )
{
    // inspired by lazyfoos tutorial
    //Caption update flag
    bool update_caption = false;

    switch( e.window.event )
    {
        //Get new dimensions and repaint on window size change
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            m_screen_width = e.window.data1;
            m_screen_height = e.window.data2;

            // adjust viewport
            glViewport(0, 0, m_screen_width, m_screen_height);

            // recalculate perspective
            m_projection = perspective(radians(FOV_ANGLE), (float)m_screen_width / m_screen_height, NEAR_PLANE, FAR_PLANE);

            // and adjust projections
            m_text_projection = ortho(0.0f, (float) m_screen_width, 0.0f, (float) m_screen_height);
            m_ui_projection = ortho(0.0f, (float) m_screen_width, 0.0f, (float) m_screen_height);

            display();
        break;

        //Repaint on exposure
        case SDL_WINDOWEVENT_EXPOSED:
           display();
        break;

        //Mouse entered window
        case SDL_WINDOWEVENT_ENTER:
            m_mouse_focus = true;
            update_caption = true;
        break;

        //Mouse left window
        case SDL_WINDOWEVENT_LEAVE:
            m_mouse_focus = false;
            update_caption = true;
        break;

        //Window has keyboard focus
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            m_keyboard_focus = true;
            update_caption = true;
        break;

        //Window lost keyboard focus
        case SDL_WINDOWEVENT_FOCUS_LOST:
            m_keyboard_focus= false;
            update_caption= true;
        break;

        //Window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
            m_minimized = true;
        break;

        //Window maxized
        case SDL_WINDOWEVENT_MAXIMIZED:
            m_minimized= false;
        break;

        //Window restored
        case SDL_WINDOWEVENT_RESTORED:
            m_minimized = false;
        break;
    }

    //Update window caption with new data
    if(update_caption)
    {
        std::stringstream caption;
        caption << "MouseFocus:" << ( ( m_mouse_focus ) ? "On" : "Off" ) << " KeyboardFocus:" << ( ( m_keyboard_focus) ? "On" : "Off") << m_screen_width << "x"  <<  m_screen_height;
        SDL_SetWindowTitle( m_window, caption.str().c_str() );
    }
}
void CRenderer::display(void)
{

    // swap buffers
    SDL_GL_SwapWindow(m_window);

    // clear current buffer to prepare for new draws
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void CRenderer::draw_crosshair(void)
{
    m_simple_shader->use();
    m_simple_shader->set_mat4("projection", m_ui_projection);
    m_simple_shader->set_mat4("model", translate(mat4(1), vec3(m_screen_width/2,m_screen_height/2,0)));


    glBindBuffer(GL_ARRAY_BUFFER, m_simple_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_crosshair_vertices), m_crosshair_vertices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindVertexArray(m_simple_VAO);
    glDrawArrays(GL_LINES,0,4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void CRenderer::draw_cube(int x, int y, mat4 view, vec3 cam_position)
{
    m_cube_shader->use();

    m_cube_shader->set_mat4("model", m_model);
    m_cube_shader->set_mat4("inverseModel", m_inverse_model);
    m_cube_shader->set_mat4("view", view);
    m_cube_shader->set_mat4("projection", m_projection);

    m_cube_shader->set_vec3("lightPos",-100.0f,-50.0f,200.0f);
    m_cube_shader->set_vec3("objectColor", 1.0f, 0.5f, 0.31f);
    m_cube_shader->set_vec3("lightColor", 1.0f, 1.0f, 1.0f);
    m_cube_shader->set_vec3("viewPos", cam_position);

    glBindVertexArray(m_VAO);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(0);
}
void CRenderer::draw_instanced_cubes(vector<float> cube_data, int cube_count, mat4 view, vec3 cam_position)
{
    m_cube_shader->use();

    m_cube_shader->set_mat4("model", m_model);
    m_cube_shader->set_mat4("inverseModel", m_inverse_model);
    m_cube_shader->set_mat4("view", view);
    m_cube_shader->set_mat4("projection", m_projection);

    m_cube_shader->set_vec3("lightPos",-100.0f,50.0f,200.0f);
    //m_cube_shader->set_vec3("lightPos",cam_position);

    m_cube_shader->set_vec3("lightColor", 1.0f, 1.0f, 1.0f);
    m_cube_shader->set_vec3("viewPos", cam_position);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_max_cubes*6, cube_data.data(), GL_STREAM_DRAW);


    glDrawArraysInstanced(GL_TRIANGLES,0, 36, cube_count);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void CRenderer::set_text_projection(float x, float y, float width, float height)
{
    // set projection mathrix (orthographic)
    m_text_projection = ortho(0.0f, width, 0.0f, height);

    // set viewport
    glViewport(x,y, width, height);
}
void CRenderer::reset_text_projection(void)
{
    m_text_projection = ortho(0.0f, (float) m_screen_width, 0.0f, (float) m_screen_height);

    glViewport(0,0, m_screen_width, m_screen_height);
}
void CRenderer::draw_text(string text, float x, float y, vec3 color, GLfloat scale)
{
    m_font_shader->use();
    // set shader inputs
    m_font_shader->set_mat4("projection", m_text_projection);
    m_font_shader->set_vec3("textColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_text_VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        scharacter ch = m_characters[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texture_id);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_text_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance>> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
vec3 CRenderer::pick_world_coordinates(vec3 camera_pos, vec3 camera_front)
{
        GLfloat zval;
        glReadPixels(m_screen_width/2, m_screen_height/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zval);


        // https://www.opengl.org/discussion_boards/showthread.php/145308-Depth-Buffer-How-do-I-get-the-pixel-s-Z-coord
        GLfloat clip_z = (zval - 0.5f) * 2.0f;
        GLfloat world_z = (2*FAR_PLANE*NEAR_PLANE)/(clip_z*(FAR_PLANE-NEAR_PLANE)-(FAR_PLANE+NEAR_PLANE));

        // travel along the camera vector to calculate the world point
        vec3 result =  camera_pos-normalize(camera_front)*world_z;
        return result + 0.5f;
}
