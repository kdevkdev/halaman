#include "CWorldSD.h"


CWorldSD::CWorldSD(int width, int height, int depth, int def_light_density):CWorld(width, height, depth, def_light_density)
{    //ctor
    //setup(width, height, def_light_density);
    m_effective_light_points.resize(width*height*depth);
    fill(m_effective_light_points.begin(),m_effective_light_points.end(),def_light_density);
}
CWorldSD::~CWorldSD()
{
    //dtor
}
void CWorldSD::iluminate_light_point(int x, int y, int z, int &cdensity)
{
        // check what material is  in this cell, lower light density below if necessary
        if(cdensity >MIN_LIGHT_DENSITY)
        {
            switch(get_material_point(x,y,z))
            {
                case MAT_PLANT_GREEN:
                    cdensity -= LIGHT_REDUCTION;
                break;

                case MAT_PLANT_SEED:
                    cdensity -= LIGHT_REDUCTION;
                break;
            }

            if(cdensity  < MIN_LIGHT_DENSITY)
            {
                cdensity = MIN_LIGHT_DENSITY;
            }
        }
        // check if plant in field, update energy income
        CPlant * cp = get_plant_point(x,y-1,z);
        if(cp != NULL)
        {
            cp->update_energy_income(cdensity- get_light_density(x,y-1,z));
        }
        set_light_density(cdensity, x, y-1,z);
}
void CWorldSD::repropagate_rays(int x, int y, int z)
{
    // this function recalculates the light rays through a certain point, taking into accoutn the obstructions

    // three rays - one from top, one from the top right, and one from the top left
    int cdensity_center = get_light_density(x,y,z);
    int lx = x+1;
    int rx = x-1;

    int cdensity_left   = cdensity_center;
    int cdensity_right  = cdensity_center;

    for(int i = y; i>0; i--)
    {

#ifdef CORD_WRAP
        if(rx >= WORLD_WIDTH)
        {
            rx -= WORLD_WIDTH;
        }
        if(lx < 0)
        {
            lx += WORLD_WIDTH;
        }
#else
        if(rx >= WORLD_WIDTH)
        {
            rx = WORLD_WIDTH-1;
        }
        if(lx < 0)
        {
            lx = 0;
        }
#endif // CORD_WRAP

        // left, middle and right ray
        iluminate_light_point(x, i,  z, cdensity_center);
        iluminate_light_point(lx, i, z, cdensity_left);
        iluminate_light_point(rx, i, z,cdensity_right);

        lx --;
        rx ++;
    }
}
void CWorldSD::deplete_light_point(int x, int y, int z, int delta)
{
    /*int t = get_light_density(x,y);
    int de = 0;

    if(t+delta > 0)
    {
        // energy cannot be higher than max
        if(t+ delta > DEF_LIGHT_DENSITY)
        {
            set_light_density(DEF_LIGHT_DENSITY, x, y);
            de = DEF_LIGHT_DENSITY - t;
        }
        else
        {
            set_light_density(t+delta, x, y);
            de = delta;
        }
    }
    else
    {
        // in this case all energy is gone in this square, but cannot be lower than 0
        set_light_density(0, x, y);
        de = -t;
    }

    // check if plant in this field, if yes update its energy
    CPlant * tp = get_plant_point(x,y);
    if(tp != nullptr && de != 0)
    {
        tp->update_energy_income(de);
    }*/

    // first step - update effective light points
    int oed = get_effective_light_density(x,y,z);
    int od = get_light_density(x,y,z);
    int de = 0;
    set_effective_light_density(oed+delta, x, y,z);

    // second step - update real light density and calculate delta energy

    assert(oed + delta <= DEF_LIGHT_DENSITY);

    if(oed + delta <= MIN_LIGHT_DENSITY)
    {
        set_light_density(MIN_LIGHT_DENSITY,x,y,z);

        if(od > MIN_LIGHT_DENSITY)
        {
            de = od - MIN_LIGHT_DENSITY;
        }
    }
    else
    {
        set_light_density(oed+delta,x,y,z);

            de = oed + delta - od;
    }

    // third step - update visible light density and plant energy income
    CPlant * tp = get_plant_point(x,y,z);
    if(tp != nullptr && de != 0)
    {
        tp->update_energy_income(de);
    }

}
void CWorldSD::set_effective_light_density(int density, int x, int y, int z)
{
    m_effective_light_points[m_width*m_height*z + y*m_width+x] = density;
}
int CWorldSD::get_effective_light_density(int x, int y, int z )
{
    return m_effective_light_points[m_width*m_height*z + y*m_width+x];
}
bool CWorldSD::update_rays(int x, int y, int z, int delta)
{
    // no boundary check since it should be in vector
    int lx = x-1;
    int rx = x+1;
    for(int i = y-1; i >= 0; i--)
    {
#ifdef CORD_WRAP
        if(rx >= WORLD_WIDTH)
        {
            rx -= WORLD_WIDTH;
        }
        if(lx < 0)
        {
            lx += WORLD_WIDTH;
        }
#else
        if(rx >= WORLD_WIDTH)
        {
            rx = WORLD_WIDTH-1;
        }
        if(lx < 0)
        {
            lx = 0;
        }
#endif // CORD_WRAP

        // left, middle and right ray
        deplete_light_point(x,i, z, delta);
        deplete_light_point(lx,i,z, delta);
        deplete_light_point(rx,i,z, delta);

        lx--;
        rx++;
    }
    return true;
}
void CWorldSD::remove_plant_cells(CPlant * plant)
{
    // iterate through all poosible squares where the plant can be and remove the cells
    int x_start = plant->get_x();
    int x_end = x_start + plant->get_rightoffset();

    int y_start = plant->get_y();
    int y_end = y_start + plant->get_topoffset();

    int z_start = plant->get_z();
    int z_end = z_start + plant->get_depthoffset();

    int removed = 0;

    // array with the highest y coordinate of the plant, to efficiently recalculate the light ray
    //vector<int> upper_y;
    //upper_y.resize((x_end-x_start+1)*(z_end - z_start +1));
    //fill(upper_y.begin(),upper_y.end(),0);
#ifdef CORD_WRAP
    for(int k = z_start; k <= z_end; k++)
    {
        for(int i = y_start; i <= y_end; i++)
        {
            for(int j = x_start; j <= x_end; j++)
            {

                if(get_plant_point(j % WORLD_WIDTH,i) == plant)
                {
                    // plant in this field - remove it, set highest point for light correction
                    set_plant_point(nullptr,j % WORLD_WIDTH,i,k);
                    set_material_point(MAT_EMPTY,j % WORLD_WIDTH,i,k);
                    update_rays(j % WORLD_WIDTH,i,k,LIGHT_REDUCTION);
                    removed ++;
                }
            }
        }
    }
#else
    for(int k = z_start; k <= z_end; k++)
    {
        for(int i = y_start; i <= y_end; i++)
        {
            for(int j = x_start; j <= x_end; j++)
            {
                if(get_plant_point(j,i,k) == plant)
                {
                    // plant in this field - remove it, set highest point for light correction
                    set_plant_point(nullptr,j,i,k);
                    set_material_point(MAT_EMPTY,j,i,k);
                    update_rays(j,i,k,LIGHT_REDUCTION);
                    removed ++;
                }
            }
        }
    }
#endif
    assert(removed == plant->get_biomass());

}
bool CWorldSD::fall_cell(int x, int y, int z )
{
  // check cell below
    if(!check_cell_empty(x,y-1, z))
    {
        // already occupied
        return false;
    }
    else
    {
        auto t = get_plant_point(x,y,z);

        // move cell one down
        set_plant_point(t, x, y-1,z);
        set_plant_point(nullptr, x, y,z);

        // update light
        update_rays(x,y,z,LIGHT_REDUCTION);
        update_rays(x,y-1,z,-LIGHT_REDUCTION);

        // move on material map
        set_material_point(MAT_EMPTY,x,y,z);
        set_material_point(MAT_PLANT_SEED,x,y-1,z);

        return true;
    }
}
