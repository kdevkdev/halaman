#include "CTextScroll.h"

CTextScroll::CTextScroll(CRenderer * renderer, float lineheight, float scale)
{
    m_renderer = renderer;

    m_lineheight = lineheight;
    m_scale = scale;
    m_topliney = 0;
}
CTextScroll::~CTextScroll()
{
    //dtor
}
void CTextScroll::add_line(string line)
{
    m_lines.push_back(line);
}
void CTextScroll::draw(int x, int y, int width, int height, vec3 text_color)
{
    m_renderer->set_text_projection(x, y, width, height);

    int topi = m_topliney/ m_lineheight;
    float offsety = m_topliney - topi* m_lineheight;

    int i = topi;
    for(float y = height + offsety-m_lineheight; y > - m_lineheight && i < (int) m_lines.size(); y-= m_lineheight)
    {
        m_renderer->draw_text(m_lines[i], 0, y, text_color, m_scale);
        i++;
    }
    m_renderer->reset_text_projection();
}

void CTextScroll::clear(void)
{
    m_lines.clear();
    m_topliney = 0;
}
void CTextScroll::scroll(float step)
{
    m_topliney += step;

    if(m_topliney < 0)
    {
        m_topliney = 0;
    }
    else if((unsigned int)m_topliney > (m_lineheight* m_lines.size()-1))
    {
        m_topliney = (m_lineheight*m_lines.size()-1);
    }
}
