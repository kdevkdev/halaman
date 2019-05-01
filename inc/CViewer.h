#ifndef CVIEWER_H
#define CVIEWER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <inttypes.h>
#include <stdio.h>

#include <cmath>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CSim.h"
#include "CRenderer.h"
#include "CPlant.h"
#include "CTextScroll.h"


class CViewer
{
    public:
        CViewer(CSim * sim);
        virtual ~CViewer();

        // render
        int render_loop();



    protected:

    private:

        CSim * m_simulation;
        CRenderer * m_renderer;

        // viewing matrix
        mat4 m_view;

        // camera vectors
        vec3 m_camera_position;
        vec3 m_camera_front;

        // cube positions and colors
        vector<float> m_cube_data;

        int m_cube_count;

        // display outlines?
        bool m_outline;
        // display grid?
        bool m_grid;
        // display light?
        bool m_light;
        // possibility to disable rendering for performanc
        bool m_rendering_enabled;
        // display text?
        bool m_disp_text;
        // display crosshair?
        bool m_crosshair;

        // helper function to generate a human readable text from a plant code
        string translate_code(CodeBlock * code);

};

#endif // CVIEWER_H
