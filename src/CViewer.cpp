#include "CViewer.h"
#include "CWorld.h"

CViewer::CViewer(CSim * sim)
{
    m_simulation = sim;
    m_outline = false;
    m_grid = false;
    m_light = true;
    m_rendering_enabled = true;
    m_disp_text = true;
    m_crosshair = true;


    int max_cubes = m_simulation->get_world()->get_width()*m_simulation->get_world()->get_height()*m_simulation->get_world()->get_depth();
    m_renderer = new CRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, max_cubes);

    // camera position
    m_camera_position = vec3(0.0f, 0.0f, 100.0f);
    m_camera_front =    vec3(0.0f, 0.0f, -1.0f);



    // allocate maximum possible number of cubes, and fill with 0 (3 position and 3 color values per cube)
    m_cube_data.resize(max_cubes*6);
    fill(m_cube_data.begin(), m_cube_data.end(), 0.0f);
    m_cube_count = 0;

}

CViewer::~CViewer()
{
    delete m_renderer;
}
int CViewer::render_loop()
{
    //Main loop flag
    bool quit = false;
    int start_tick = 0;
    int end_tick = 0;
    int frame_span = 0;
    float framerate = 0;
    float simround_rate = 0;
    int prev_simround = 0;
    float iteration_rate = 0;
    int prev_iteration= 0;
    bool paused = true;
    int curround  = 0;
    int curit = 0;
    //Event handler
    SDL_Event e;
    int mouse_offset_x = 0;
    int mouse_offset_y = 0;
    float yaw = -90.0f;
    float pitch = 0;
    glm::vec3 cursor_worldcoords;
    CPlant * focused_plant = nullptr;
    CPlant * single_plant = nullptr;
    float rg = 0;
    float gg = 0;
    float bg = 0;
    float rs = 0;
    float gs = 0;
    float bs = 0;
    CTextScroll textcodes(m_renderer, 24, 0.5f);
    bool disp_codes = false;
    bool focused_only = false;
    bool draw_green = true;
    bool draw_seed = true;

    while( !quit )
    {
        // calculate time between frames
        frame_span = start_tick;
        start_tick = SDL_GetTicks();
        frame_span = start_tick - frame_span;

        // calculate framerate, with smothing
        framerate= (framerate * 30 + (1000/(float)frame_span))/31;

        if(!paused)
        {
            // calculate per second
            // only poll once to make it a little  thread safer ;)
            curround = m_simulation->get_rounds();
            simround_rate = ((simround_rate*150 + (curround - prev_simround))/151);

            // only poll once to make it a little  thread safer ;)
            curit = m_simulation->get_cum_iterations();
            iteration_rate = ((iteration_rate*150 + (curit- prev_iteration))/151);
        }

        prev_simround = curround;
        prev_iteration = curit;

        // Event handling
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            else if(e.type == SDL_KEYDOWN)
            {
                switch( e.key.keysym.sym )
                {
                    case SDLK_u:
                        iteration_rate = 0;
                        simround_rate = 0;
                        paused = !paused;
                        m_simulation->flip_stop();
                        single_plant = nullptr;
                        focused_only = false;
                    break;
                    case SDLK_SPACE:
                        m_simulation->single_round();
                    break;
                    case SDLK_o:
                        m_outline = !m_outline;
                    break;
                    case SDLK_g:
                        m_grid = !m_grid;
                    break;
                    case SDLK_l:
                        m_light = !m_light;
                    break;
                    case SDLK_t:
                        m_disp_text = !m_disp_text;
                    break;
                    case SDLK_r:
                        m_rendering_enabled = !m_rendering_enabled;
                    break;
                    case SDLK_c:
                        m_crosshair = !m_crosshair;
                    break;
                    case SDLK_e:
                        if(focused_plant)
                        {
                            disp_codes = !disp_codes;
                            single_plant = focused_plant;

                            if(disp_codes)
                            {
                                // copy codes to the text box
                                for(int i = 0; i < focused_plant->get_num_codes(); i++)
                                {
                                    textcodes.add_line(translate_code(focused_plant->get_code(i)));
                                }
                            }
                            else
                            {
                                textcodes.clear();
                            }
                        }
                        else
                        {
                            disp_codes = false;
                            textcodes.clear();
                            single_plant = nullptr;
                        }
                    break;
                    case SDLK_f:
                        focused_only = !focused_only;
                    break;
                    case SDLK_b:
                        draw_green = !draw_green;
                    break;
                    case SDLK_v:
                        draw_seed = !draw_seed;
                    break;

                    case SDLK_n:
                        m_renderer->switch_fullscreen();
                    break;
                    case SDLK_m:
                        m_renderer->switch_mouserelative();
                    break;
                }

            }
            else if(e.type == SDL_MOUSEMOTION && m_renderer->get_mouserelative())
            {
                // only perform if in relative mouse mode

                mouse_offset_x = e.motion.xrel; //mouse_offset_x = last_mouse_x - e.motion.x;
                mouse_offset_y = e.motion.yrel; //mouse_offset_y = last_mouse_y - e.motion.y;

                // update yaw and pitch based on mouse movement
                yaw   += (mouse_offset_x*TURN_SENSIVITY)/framerate;
                pitch += (mouse_offset_y*TURN_SENSIVITY)/framerate;

                // prevent turning over 90 degree angle for pitch
                if(pitch > 89)
                {
                    pitch = 89;
                }
                else if(pitch < -89)
                {
                    pitch = -89;
                }

                vec3 front;
                front.x = cos(radians(yaw)) * cos(radians(pitch));
                front.y = sin(radians(pitch));
                front.z = sin(radians(yaw)) * cos(radians(pitch));
                m_camera_front = normalize(front);
            }
            else if(e.type == SDL_MOUSEWHEEL)
            {
                // zoom in out?
            }
            else if(e.type == SDL_WINDOWEVENT)
            {
                // forward window events to the renderer
                m_renderer->handle_window_events(e);
            }
        }

        // read keyboard and move camera
        const Uint8* currentKeyStates = SDL_GetKeyboardState( nullptr );
        if( currentKeyStates[ SDL_SCANCODE_W ] )
        {
            m_camera_position += (MOVE_SPEED/framerate)* m_camera_front;
        }
        if( currentKeyStates[ SDL_SCANCODE_A] )
        {
            m_camera_position -= (MOVE_SPEED/framerate)* normalize(cross(m_camera_front,vec3(0.0f, 1.0f,  0.0f)));
        }
        if( currentKeyStates[ SDL_SCANCODE_S ] )
        {
            m_camera_position -= (MOVE_SPEED/framerate)* m_camera_front;
        }
        if( currentKeyStates[ SDL_SCANCODE_D ] )
        {
            m_camera_position += (MOVE_SPEED/framerate)* normalize(cross(m_camera_front,vec3(0.0f, 1.0f,  0.0f)));
        }
        if ( currentKeyStates[ SDL_SCANCODE_LSHIFT])
        {
            m_camera_position -= (MOVE_SPEED/framerate)* normalize(cross(m_camera_front,cross(m_camera_front,vec3(0.0f, 1.0f,  0.0f))));
        }
        if ( currentKeyStates[ SDL_SCANCODE_LCTRL])
        {
            m_camera_position += (MOVE_SPEED/framerate)* normalize(cross(m_camera_front,cross(m_camera_front,vec3(0.0f, 1.0f,  0.0f))));
        }
        if( currentKeyStates[ SDL_SCANCODE_LALT] && currentKeyStates[ SDL_SCANCODE_F4])
        {
            quit = true;
        }
        if( currentKeyStates[ SDL_SCANCODE_DOWN])
        {
            textcodes.scroll(+SCROLL_SPEED/framerate);
        }
        if( currentKeyStates[ SDL_SCANCODE_UP])
        {
            textcodes.scroll(-SCROLL_SPEED/framerate);
        }

        // update camera view, (uses the ukp vector to normalize)
        m_view = lookAt(m_camera_position, m_camera_position + m_camera_front, vec3(0.0f, 1.0f,  0.0f));


        if(m_rendering_enabled)
        {
            // loop through blocks
            int matpoint = 0;
            float lightf = 0;
            m_cube_count = 0;
            CPlant * cplant = nullptr;

            if(m_light)
            {
                for(int k = 0; k <WORLD_DEPTH; k++)
                {
                    for(int j = 0; j < WORLD_HEIGHT; j++)
                    {
                        for(int i = 0; i < WORLD_WIDTH; i++)
                        {
                            matpoint = m_simulation->get_world()->get_material_point(i,j,k);
                            lightf = (m_simulation->get_world()->get_light_density(i,j,k)/ DEF_LIGHT_DENSITY)/1.25f+0.2;
                            cplant = m_simulation->get_world()->get_plant_point(i,j,k);

                            if(focused_only && cplant != single_plant && paused)
                            {
                                continue;
                            }

                            if(cplant == focused_plant)
                            {
                                rg = GREEN_HIGHLIGHT_COLOR_R;gg = GREEN_HIGHLIGHT_COLOR_G;bg = GREEN_HIGHLIGHT_COLOR_B;
                                rs = SEED_HIGHLIGHT_COLOR_R;gs = SEED_HIGHLIGHT_COLOR_G;bs = SEED_HIGHLIGHT_COLOR_B;
                            }
                            else
                            {
                                rg = GREEN_COLOR_R;gg = GREEN_COLOR_G;bg = GREEN_COLOR_B;
                                rs = SEED_COLOR_R;gs = SEED_COLOR_G;bs = SEED_COLOR_B;
                            }

                            if(draw_green && matpoint == MAT_PLANT_GREEN)
                            {

                                    // store position and color values consequently in an array so that its easely sendable to the shader
                                    m_cube_data[m_cube_count*6]   = (float) i;
                                    m_cube_data[m_cube_count*6+1] = (float) j;
                                    m_cube_data[m_cube_count*6+2] = (float) k;
                                    m_cube_data[m_cube_count*6+3] = rg * lightf;
                                    m_cube_data[m_cube_count*6+4] = gg * lightf;
                                    m_cube_data[m_cube_count*6+5] = bg * lightf;

                                    m_cube_count++;
                            }
                            else if(draw_seed && matpoint == MAT_PLANT_SEED)
                            {


                                    m_cube_data[m_cube_count*6]   = (float) i;
                                    m_cube_data[m_cube_count*6+1] = (float) j;
                                    m_cube_data[m_cube_count*6+2] = (float) k;
                                    m_cube_data[m_cube_count*6+3] = rs * lightf;
                                    m_cube_data[m_cube_count*6+4] = gs * lightf;
                                    m_cube_data[m_cube_count*6+5] = bs * lightf;

                                    m_cube_count++;
                            }
                        }
                    }
                }
            }
            else
            {
               for(int k = 0; k <WORLD_DEPTH; k++)
                {
                    for(int j = 0; j < WORLD_HEIGHT; j++)
                    {
                        for(int i = 0; i < WORLD_WIDTH; i++)
                        {
                            matpoint = m_simulation->get_world()->get_material_point(i,j,k);

                            cplant = m_simulation->get_world()->get_plant_point(i,j,k);


                            if(focused_only && cplant != single_plant && paused)
                            {
                                continue;
                            }

                            if(cplant == focused_plant)
                            {
                                rg = GREEN_HIGHLIGHT_COLOR_R;gg = GREEN_HIGHLIGHT_COLOR_G;bg = GREEN_HIGHLIGHT_COLOR_B;
                                rs = SEED_HIGHLIGHT_COLOR_R;gs = SEED_HIGHLIGHT_COLOR_G;bs = SEED_HIGHLIGHT_COLOR_B;
                            }
                            else
                            {
                                rg = GREEN_COLOR_R;gg = GREEN_COLOR_G;bg = GREEN_COLOR_B;
                                rs = SEED_COLOR_R;gs = SEED_COLOR_G;bs = SEED_COLOR_B;
                            }

                            if(draw_green && matpoint == MAT_PLANT_GREEN)
                            {

                                    // store position and color values consequently in an array so that its easely sendable to the shader
                                    m_cube_data[m_cube_count*6]   = (float) i;
                                    m_cube_data[m_cube_count*6+1] = (float) j;
                                    m_cube_data[m_cube_count*6+2] = (float) k;
                                    m_cube_data[m_cube_count*6+3] = rg;
                                    m_cube_data[m_cube_count*6+4] = gg;
                                    m_cube_data[m_cube_count*6+5] = bg;

                                    m_cube_count++;
                            }
                            else if(draw_seed && matpoint == MAT_PLANT_SEED)
                            {

                                    m_cube_data[m_cube_count*6]   = (float) i;
                                    m_cube_data[m_cube_count*6+1] = (float) j;
                                    m_cube_data[m_cube_count*6+2] = (float) k;
                                    m_cube_data[m_cube_count*6+3] = rs;
                                    m_cube_data[m_cube_count*6+4] = gs;
                                    m_cube_data[m_cube_count*6+5] = bs;

                                    m_cube_count++;
                            }
                        }
                    }
                }
            }
            m_renderer->draw_instanced_cubes(m_cube_data, m_cube_count, m_view, m_camera_position);
        }


        if(m_disp_text)
        {
            char tstring[32];
            vec3 text_color = vec3(1.0f,1.0f,0.0f);
            vec3 num_color = vec3(1.0f,0,0);
            float tscale = 0.34f;
            float xoffset1 = 10.0f;
            float xoffset2 = 200.0f;

            sprintf(tstring, "FPS:");
            m_renderer->draw_text(tstring,xoffset1,10.0f,text_color,tscale );
            sprintf(tstring, "%.4g",framerate);
            m_renderer->draw_text(tstring,xoffset2,10.0f,num_color,tscale );

            sprintf(tstring, "#TInstr.:");
            m_renderer->draw_text(tstring,xoffset1,30.0f, text_color,tscale );
            sprintf(tstring, "%llu" ,(long long unsigned int) m_simulation->get_cum_iterations());
            m_renderer->draw_text(tstring,xoffset2,30.0f, num_color,tscale );


            sprintf(tstring, "#TCubes:");
            m_renderer->draw_text(tstring,xoffset1,50.0f,text_color,tscale );
            sprintf(tstring, "%d", m_cube_count);
            m_renderer->draw_text(tstring,xoffset2,50.0f, num_color,tscale );

            sprintf(tstring, "Seconds:");
            m_renderer->draw_text(tstring,xoffset1,70.0f,text_color,tscale );
            sprintf(tstring, "%.2f", start_tick/1000.0f);
            m_renderer->draw_text(tstring,xoffset2,70.0f,num_color,tscale );


            sprintf(tstring, "Round:");
            m_renderer->draw_text(tstring,xoffset1,90.0f,text_color,tscale );
            sprintf(tstring, "%llu" ,(long long unsigned int) m_simulation->get_rounds());
            m_renderer->draw_text(tstring,xoffset2,90.0f,num_color,tscale );

            sprintf(tstring, "Instr. /sec:");
            m_renderer->draw_text(tstring,xoffset1,110.0f,text_color,tscale );
            sprintf(tstring, "%.1f",iteration_rate/frame_span*1000);
            m_renderer->draw_text(tstring,xoffset2,110.0f,num_color,tscale );

            sprintf(tstring, "Rounds/sec:");
            m_renderer->draw_text(tstring,xoffset1,130.0f,text_color,tscale );
            sprintf(tstring, "%.1f",simround_rate/frame_span*1000);
            m_renderer->draw_text(tstring,xoffset2,130.0f,num_color,tscale );

            sprintf(tstring, "#TMutations:");
            m_renderer->draw_text(tstring,xoffset1,150.0f,text_color,tscale );
            sprintf(tstring, "%d",m_simulation->get_num_change_muts());
            m_renderer->draw_text(tstring,xoffset2,150.0f,num_color,tscale );

            sprintf(tstring, "#TIns. mutations:");
            m_renderer->draw_text(tstring,xoffset1,170.0f,text_color,tscale );
            sprintf(tstring, "%d",m_simulation->get_num_insert_muts());
            m_renderer->draw_text(tstring,xoffset2,170.0f,num_color,tscale );


            sprintf(tstring, "#TDel. mutations:");
            m_renderer->draw_text(tstring,xoffset1,190.0f,text_color,tscale );
            sprintf(tstring, "%d",m_simulation->get_num_delete_muts());
            m_renderer->draw_text(tstring,xoffset2,190.0f,num_color,tscale );

            sprintf(tstring, "#Plants:");
            m_renderer->draw_text(tstring,xoffset1,210.0f,text_color,tscale );
            sprintf(tstring, "%d",m_simulation->get_num_plants());
            m_renderer->draw_text(tstring,xoffset2,210.0f,num_color,tscale );

            sprintf(tstring, "#TDead plants:");
            m_renderer->draw_text(tstring,xoffset1,230.0f,text_color,tscale );
            sprintf(tstring, "%lu",m_simulation->get_num_dead_plants());
            m_renderer->draw_text(tstring,xoffset2,230.0f,num_color,tscale );

            sprintf(tstring, "#Seeds:");
            m_renderer->draw_text(tstring,xoffset1,250.0f,text_color,tscale );
            sprintf(tstring, "%d",m_simulation->get_num_seeds());
            m_renderer->draw_text(tstring,xoffset2,250.0f,num_color,tscale );

            sprintf(tstring, "Average mass:");
            m_renderer->draw_text(tstring,xoffset1,270.0f,text_color,tscale );
            sprintf(tstring, "%.4g",(float)m_simulation->get_biomass()/ m_simulation->get_num_plants());
            m_renderer->draw_text(tstring,xoffset2,270.0f,num_color,tscale );


            sprintf(tstring, "Av. instr. length:");
            m_renderer->draw_text(tstring,xoffset1,290.0f,text_color,tscale );
            sprintf(tstring, "%.4g",(float)m_simulation->get_codemass()/ (m_simulation->get_num_plants()+m_simulation->get_num_seeds()));
            m_renderer->draw_text(tstring,xoffset2,290.0f,num_color,tscale );





            if(disp_codes)
            {
                textcodes.draw(m_renderer->get_screen_width()*0.68,m_renderer->get_screen_height()*0.5, m_renderer->get_screen_width()*0.3, m_renderer->get_screen_height()*0.5, text_color);
            }

        }

        // crosshair
        focused_plant = nullptr;
        float margin = 0.03f;
        if(m_crosshair)
        {
            cursor_worldcoords = m_renderer->pick_world_coordinates(m_camera_position, m_camera_front);
            m_renderer->draw_crosshair();

            // check if valid points and then retrieve a possible plant
            if(cursor_worldcoords.x >= 0 && cursor_worldcoords.x < WORLD_WIDTH && cursor_worldcoords.y >= 0 && cursor_worldcoords.y < WORLD_HEIGHT && cursor_worldcoords.z >= 0 && cursor_worldcoords.z < WORLD_DEPTH )
            {
                focused_plant = m_simulation->get_world()->get_plant_point(floor(cursor_worldcoords.x), floor(cursor_worldcoords.y), floor(cursor_worldcoords.z));

            }
            if(!focused_plant)
            {
                // unfortunately pick woorldcordinates is not very precise. therefore, we take a small margin and check within it for a neighbour if no plant is found yet,


                float z = cursor_worldcoords.z;
                float x = cursor_worldcoords.x;
                float y = cursor_worldcoords.y;

                if(x - floor(x) < margin && x-margin > 0)
                {
                    x -= margin;
                }
                else if(ceil(x) - x  < margin && x+margin < (WORLD_HEIGHT-1))
                {
                    x += margin;
                }
                if(y - floor(y) < margin && y-margin > 0)
                {
                    y -= margin;
                }
                else if(ceil(y) - y  < margin && y+margin < (WORLD_HEIGHT-1))
                {
                    y += margin;
                }
                if(z - floor(z) < margin && z-margin > 0)
                {
                    z -= margin;
                }
                else if(ceil(z) - z  < margin && z+margin < (WORLD_DEPTH-1))
                {
                    z += margin;

                }
                if(x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT && z >= 0 && z < WORLD_DEPTH )
                {
                    focused_plant = m_simulation->get_world()->get_plant_point(floor(x), floor(y), floor(z));


                }

                // todo: check instead next squarea in view direction

                /*focused_plant = (!focused_plant ?  m_simulation->get_world()->get_plant_point(ceil(cursor_worldcoords.x-margin), ceil(cursor_worldcoords.y-margin), ceil(cursor_worldcoords.z-margin)) : focused_plant );
                focused_plant = (!focused_plant ?  m_simulation->get_world()->get_plant_point(ceil(cursor_worldcoords.x-margin), ceil(cursor_worldcoords.y-margin), floor(cursor_worldcoords.z+margin)) : focused_plant );
                focused_plant = (!focused_plant ?  m_simulation->get_world()->get_plant_point(ceil(cursor_worldcoords.x-margin), floor(cursor_worldcoords.y+margin), ceil(cursor_worldcoords.z-margin)) : focused_plant );
                focused_plant = (!focused_plant ?  m_simulation->get_world()->get_plant_point(ceil(cursor_worldcoords.x-margin), floor(cursor_worldcoords.y+margin), floor(cursor_worldcoords.z+margin)) : focused_plant );
                focused_plant = (!focused_plant ?  m_simulation->get_world()->get_plant_point(floor(cursor_worldcoords.x+margin), ceil(cursor_worldcoords.y-margin), ceil(cursor_worldcoords.z-margin)) : focused_plant );
                focused_plant = (!focused_plant ?  m_simulation->get_world()->get_plant_point(floor(cursor_worldcoords.x+margin), ceil(cursor_worldcoords.y-margin), floor(cursor_worldcoords.z+margin)) : focused_plant );
                focused_plant = (!focused_plant ?  m_simulation->get_world()->get_plant_point(floor(cursor_worldcoords.x+margin), floor(cursor_worldcoords.y+margin), ceil(cursor_worldcoords.z-margin)) : focused_plant );
                focused_plant = (!focused_plant ?  m_simulation->get_world()->get_plant_point(floor(cursor_worldcoords.x+margin), floor(cursor_worldcoords.y+margin), floor(cursor_worldcoords.z+margin)) : focused_plant );*/

            }
        }

        // display
        m_renderer->display();

        end_tick = SDL_GetTicks();

        // try to reach the target framerate - framerate limiter - but does not work perfectly
        int aheadticks = ((float)1000)/TARGET_FRAMERATE- (end_tick - start_tick);

        if(aheadticks > 0)
        {
            SDL_Delay(aheadticks);
        }
    }
    m_simulation->quit();

    return 0;
}
string CViewer::translate_code(CodeBlock * code)
{
    uint32_t data = code->data;
    float bvar1 = (float)(((((data & BC_BITMASK_BVAR1) >> BC_BVAR1_SHIFT) << 16) /BC_BVAR_MAX)>> 16);
    float bvar2 = (float)(((((data & BC_BITMASK_BVAR2) >> BC_BVAR2_SHIFT) << 16) /BC_BVAR_MAX)>> 16);

    string kind;
    if(data & BC_NOP)
    {
            kind = "NOP   ";
    }
    else
    {
            kind = "BUILD ";
    }

    string dir;
    switch(data & BC_BITMASK_DIR)
    {
        case BC_BUILD_DIR_BACK:
            dir = "back  ";
        break;
        case BC_BUILD_DIR_FRONT:
            dir = "front ";
        break;
        case BC_BUILD_DIR_LEFT:
            dir = "left  ";
        break;
        case BC_BUILD_DIR_RIGHT:
            dir = "right ";
        break;
        case BC_BUILD_DIR_DOWN:
            dir = "down  ";
        break;
        case BC_BUILD_DIR_UP:
            dir = "up    ";
        break;
        default:
            dir = "      ";
        break;
    }
    string placement_mode;
    switch(data & BC_BITMASK_PM)
    {
        case BC_PM_PROP:
            placement_mode = "rel ";
        break;
        case BC_PM_RAND:
            placement_mode = "rnd ";
        break;
        default:
            placement_mode =  "   ";
        break;
    }
    string buildmaterial;
    switch(data & BC_BITMASK_MAT)
    {
        case BC_BUILD_GREEN:
            buildmaterial = "green ";
        break;
        case BC_BUILD_SEED:
            buildmaterial = "seed  ";
        break;
        default:
            buildmaterial = "      ";
        break;
    }
    return kind + dir + placement_mode + buildmaterial + "bv1: " + std::to_string(bvar1) + " bv2: " + std::to_string(bvar2);
}
