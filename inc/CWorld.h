#ifndef CWORLD_H
#define CWORLD_H

#include <algorithm>
#include <vector>
#include <list>
#include "SDL2/SDL.h"
#include "defhelper.h"
#include "CPlant.h"


using namespace std;
class CPlant;



class CWorld
{
    public:
        CWorld(int width, int height, int depth, int def_light_density);
        virtual ~CWorld();

        // these return the (absolute) distance of the next point of "plant" relative to the x, y, z coordinate
        // if no such point is found, the value returned is greater than the maximal distance (which is given relative to the the max and min parameters)
        int onextdown(int x, int y, int z, int miny, CPlant * plant);
        int onextup(int x, int y, int z, int maxy, CPlant * plant);
        int onextleft(int x, int  y, int z, int minx, CPlant * plant);
        int onextright(int x, int  y, int z, int maxx, CPlant * plant);
        int onextin(int x, int  y, int z, int minz, CPlant * plant);
        int onextinback(int x, int  y, int z, int maxz, CPlant * plant);

        bool build_cell(int x, int y, int z, CPlant * plant, int mat);
        bool build_cell_unchecked(int x, int y, int z, CPlant * plant, int mat);
        virtual bool fall_cell(int x, int y, int z);
        void sprout_cell(int x, int y, int z);
        bool check_cell_empty(int x, int y, int z);

        virtual void remove_plant_cells(CPlant * plant);


        // used also by viewer thread
        CPlant * get_plant_point(int x, int y, int z);
        int get_material_point      (int x, int y, int z);
        int get_light_density(int x, int y, int z);

        int get_width(void);
        int get_height(void);
        int get_depth(void);

    protected:

        void set_light_density(int density, int x, int y, int z);

        void set_material_point(int material, int x, int y, int z);
        void set_plant_point(CPlant* plant, int x, int y, int z);


        // recalculate ray down from the given y coordinate
        virtual void repropagate_rays(int x, int y, int z);


        void setup(int width, int height, int depth, int def_light_density);


        int m_width;
        int m_height;
        int m_depth;

        int m_def_light_density;


        virtual bool update_rays(int x, int y, int z, int reduction);

        vector<int> m_light_points;
        vector<int> m_material_points;
        vector<CPlant*> m_plant_points;



};

#endif // CWORLD_H
