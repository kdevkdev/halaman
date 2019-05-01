#ifndef CWORLDSD_H
#define CWORLDSD_H

#include "CWorld.h"

class CPlant;
class CWorld;

class CWorldSD: public CWorld
{
    // this class inhericts from CWorld and light in it creates shadow to the sides, not just downwards

    public:
        CWorldSD(int width, int height, int depth, int def_light_density);

        virtual ~CWorldSD();

        void remove_plant_cells(CPlant * plant);

        bool fall_cell(int x, int y, int z);

    protected:

    private:

            // update light ray due to distortion (shaddow)
        bool distort_rays(int x, int y, int z, int reduction);


        // recalculate ray down from the given y coordinate
        void repropagate_rays(int x, int y, int z);


        bool update_rays(int x, int y, int z, int delta);



        // helper function to deplete light in a square
        void deplete_light_point(int x, int y, int z, int delta);

        // function to recalculate light density in a squarewhen the ray passes through
        void iluminate_light_point(int x, int y, int z, int &cdensity);

        // effective light density to keep the information how many rays are covering a point
        void set_effective_light_density(int density, int x, int y, int z);
        int get_effective_light_density(int x, int y, int z);


        vector<int> m_effective_light_points;
};

#endif // CWORLDSD_H
