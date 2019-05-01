#include "CWorld.h"

CWorld::CWorld(int width, int height, int depth, int def_light_density)
{
    setup(width, height, depth, def_light_density);
}
CWorld::~CWorld()
{
}
void CWorld::setup(int width, int height, int depth, int def_light_density)
{
    m_width = width;
    m_height = height;
    m_depth = depth;
    m_def_light_density = def_light_density;

    // create arrays to contain information about the simulated world (light, materials, and pointers to plants)
    m_light_points.resize(width*height*depth);
    m_material_points.resize(width*height*depth);
    m_plant_points.resize(width*height*depth);

    // fill with default values
    fill(m_light_points.begin(),m_light_points.end(),def_light_density);
    fill(m_material_points.begin(), m_material_points.end(), MAT_EMPTY);
    fill(m_plant_points.begin(), m_plant_points.end(), nullptr);
}
bool CWorld::update_rays(int x, int y, int z, int delta)
{
    // this function updates teh rays below the given position
    // no boundary check since it should be in vector
    for(int i = y-1; i >= 0; i--)
    {
        int t = get_light_density(x,i,z);
        int de = 0;

        if(t + delta > MIN_LIGHT_DENSITY)
        {
            // energy cannot be higher than max
            if(t+ delta > DEF_LIGHT_DENSITY)
            {
                set_light_density(DEF_LIGHT_DENSITY, x, i, z);
                de = DEF_LIGHT_DENSITY - t;
            }
            else
            {
                set_light_density(t+delta, x, i, z);
                de = delta;
            }
        }
        else
        {
            // in this case all energy is gone in this square, but cannot be lower than 0
            set_light_density(MIN_LIGHT_DENSITY, x, i, z);
            de = -MIN_LIGHT_DENSITY;
        }
        // check if plant in this field, if yes update its energy
        CPlant * tp = get_plant_point(x,i,z);
        if(tp != nullptr && de != 0 )
        {
            tp->update_energy_income(de);
        }

    }
    return true;
}
void CWorld::repropagate_rays(int x, int y, int z)
{
    // this function recalculates the light ray from a certain point, taking into accoutn the obstructions
    int cdensity = get_light_density(x,y,z);
    for(int i = y; i>0; i--)
    {
        // check what material is  in this cell, lower light density if necessary
        if(cdensity >MIN_LIGHT_DENSITY)
        {
            switch(get_material_point(x,i,z))
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
        // check if plant in field below, update energy income
        CPlant * cp = get_plant_point(x,i-1,z);
        if(cp != NULL)
        {
            cp->update_energy_income(cdensity- get_light_density(x, i-1,z));
        }
        set_light_density(cdensity, x, i-1,z);
    }
}
int CWorld::get_light_density(int x, int y, int z)
{
    // access higher than m_height possible, because energy income for plants
    // refers to one cell above green cells
    if(y >= m_height)
    {
        return m_def_light_density;
    }

    // no boundary check since it should be in vector
    return m_light_points[z*m_width*m_height+y*m_width+x];
}
void CWorld::set_light_density(int density, int x, int y, int z)
{
    m_light_points[z*m_width*m_height+y*m_width+x] = density;
    assert(density >= MIN_LIGHT_DENSITY);
    assert(density <= DEF_LIGHT_DENSITY);
}
void CWorld::set_material_point(int material, int x, int y, int z)
{
    // no boundary check since it should be in vector
    m_material_points[z*m_width*m_height+y*m_width+x] = material;
}
int CWorld::get_material_point(int x, int y, int z)
{
    // no boundary check since it should be in vector
    return m_material_points[z*m_width*m_height+y*m_width+x];

}
void CWorld::set_plant_point(CPlant * plant, int x, int y, int z)
{
    // no boundary check since it should be in vector
    m_plant_points[z*m_width*m_height+y*m_width+x] = plant;
}
CPlant* CWorld::get_plant_point(int x, int y, int z)
{
    assert(x >= 0);
    assert(x <= WORLD_WIDTH);
    // no boundary check since it should be in vector
    return m_plant_points[z*m_width*m_height+y*m_width+x];
}
int CWorld::onextdown(int x, int y, int z, int miny, CPlant * plant)
{
    int i = y;
    while(i >= miny && get_plant_point(x, i, z)!= plant)
    {
        i--;
    }
    return y-i;
}
int CWorld::onextup(int x, int y, int z, int maxy, CPlant * plant)
{
    int i = y;
    while(i <= maxy &&get_plant_point(x, i,z) != plant)
    {
        i++;
    }
    return i-y;
}
int CWorld::onextin(int x, int y, int z, int minz, CPlant * plant)
{
    int i = z;
    while(i >= minz && get_plant_point(x, y, i)!= plant)
    {
        i--;
    }
    return z-i;
}
int CWorld::onextinback(int x, int y, int z, int maxz, CPlant * plant)
{
    int i = z;
    while(i <= maxz &&get_plant_point(x, y,i) != plant)
    {
        i++;
    }
    return i-z;
}
int CWorld::onextright(int x, int y, int z, int maxx, CPlant * plant)
{
    int i = x;
#ifdef CORD_WRAP
    while(i <= maxx && get_plant_point(i % WORLD_WIDTH, y, z) != plant)
    {
        i++;
    }
#else
    while(i <= maxx && get_plant_point(i , y, z) != plant)
    {
        i++;
    }
#endif

    return i - x;
}
int CWorld::onextleft(int x, int y, int z, int minx, CPlant * plant)
{
    int i = x;

#ifdef CORD_WRAP
    int j = x;
    while(j >= minx && get_plant_point(i, y, z) != plant)
    {

        i--;
        j--;

        if(i < 0)
        {
            i+= WORLD_WIDTH;
        }
    }
    return j - x;
#else
    while(i >= minx && get_plant_point(i, y, z) != plant)
    {
        i--;
    }
    return x - i;
#endif
}
bool CWorld::build_cell(int x, int y, int z, CPlant * plant, int mat)
{
    if(!check_cell_empty(x,y,z))
    {
        return false;
    }

    // boundaries are ok and square is not taken, set light, material and pointer
    // importnt to call distort_ray before setting the plant point, as otherwise
    // the currrently building plant will have energy income deducted  dad it did not add yet
    // and the simulation state will become invalid
    set_material_point(mat, x, y, z);
    update_rays(x,y,z, -LIGHT_REDUCTION);

    set_plant_point(plant, x, y, z);

    return true;
}
bool CWorld::build_cell_unchecked(int x, int y,int z, CPlant * plant, int mat)
{
    // this is dangerous - dont use it without explicitly checking (call check_cell_empty!) first -
    // here for performance reasons
    // boundaries are ok and square is not taken, set light, material and pointer
    set_plant_point(plant, x, y, z);
    set_material_point(mat, x, y, z);
    update_rays(x,y,z, -LIGHT_REDUCTION);

    return true;
}
bool CWorld::check_cell_empty(int x, int y, int z)
{
    // check if cell is empty - return true if yes

    // check if boundaries are ok
    if(!(x >= 0 && x < m_width && y >= 0 && y < m_height && z >=0 && z < m_depth))
    {
        return false;
    }

    // check firsst if space is available
    if(get_plant_point(x,y,z) != nullptr)
    {
        return false;
    }

    return true;
}
bool CWorld::fall_cell(int x, int y, int z)
{
    // check cell below
    if(!check_cell_empty(x,y-1,z))
    {
        // already occupied
        return false;
    }
    else
    {
        auto t = get_plant_point(x,y,z);

        // move cell one down
        set_plant_point(t, x, y-1, z);
        set_plant_point(nullptr, x, y,z);

        // update light - since teh cell fell one square down, the only change
        // is in the square of the cell - it must be the same ligth density as above as it is not obstructed
        // by an occupied square from above anymore
        set_light_density(get_light_density(x,y,z),x,y-1,z);

        // move on material map
        set_material_point(MAT_EMPTY,x,y,z);
        set_material_point(MAT_PLANT_SEED,x,y-1,z);

        return true;
    }
}
void CWorld::sprout_cell(int x, int y, int z)
{
    set_material_point(MAT_PLANT_GREEN,x, y, z);
}
void CWorld::remove_plant_cells(CPlant * plant)
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
    vector<int> upper_y;
    upper_y.resize((x_end-x_start+1)*(z_end-z_start+1));
    fill(upper_y.begin(),upper_y.end(),0);

#ifdef CORD_WRAP
    for(int k = z_start; k <= z_end; k++)
    {
        for(int i = y_start; i <= y_end; i++)
        {
            for(int j = x_start; j <= x_end; j++)
            {

                if(get_plant_point(j % WORLD_WIDTH,i,k) == plant)
                {
                    // plant in this field - remove it, set highest point for light correction
                    set_plant_point(nullptr,j % WORLD_WIDTH,i,k);
                    set_material_point(MAT_EMPTY,j % WORLD_WIDTH,i,k);
                    upper_y[k*(x_end-x_start+1) +j - x_start]= i;
                    removed++;
                }
            }
        }
    }

    // update the light rays
    for(int k = z_start; k <= z_end; k++)
    {
        for(int i = x_start; i<= x_end;i++)
        {
            repropagate_rays(i % WORLD_WIDTH,upper_y[k*(x_end-x_start+1)+i-x_start],k);
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
                    set_material_point(MAT_EMPTY,j ,i,k);
                    upper_y[k*(x_end-x_start+1) +j - x_start]= i;
                    removed++;
                }
            }
        }
    }
    assert(removed == plant->get_biomass());

    // update the light rays
    for(int k = z_start; k <= z_end; k++)
    {
        for(int i = x_start; i<= x_end;i++)
        {
            repropagate_rays(i,upper_y[k*(x_end-x_start+1)+i-x_start],k);
        }
    }
#endif
}
int CWorld::get_width(void)
{
    return m_width;
}
int CWorld::get_height(void)
{
    return m_height;
}
int CWorld::get_depth(void)
{
    return m_height;
}
