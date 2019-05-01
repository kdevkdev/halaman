#ifndef CTEXTSCROLL_H
#define CTEXTSCROLL_H

#include"CRenderer.h"
#include<string>
#include<vector>
#include<algorithm>




class CTextScroll
{
    public:
        CTextScroll(CRenderer * renderer, float lineheight, float scale);
        virtual ~CTextScroll();

        void add_line(string line);
        void clear();
        void draw(int x, int y, int width, int height, vec3 text_color);
        void scroll(float step);

    protected:

    private:
        CRenderer * m_renderer;

        float m_lineheight;
        float m_scale;

        // the virtual y position relative to the first line
        float m_topliney;


        // contains the text
        vector <string> m_lines;
};

#endif // CTEXTSCROLL_H
