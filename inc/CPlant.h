#ifndef CPLANT_H
#define CPLANT_H

#include <memory>
#include <utility>
#include <list>
#include <cassert>
#include <vector>

#include "defhelper.h"
#include "CSim.h"

using namespace std;
class CWorld;
class CSim;

typedef int(down_travers)(CWorld*, int, int, int, int, CPlant * );


int prope_topside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int minu, CPlant * plant);
int prope_bottomside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int maxu, CPlant * plant);
int prope_rigthside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int minu, CPlant * plant);
int prope_leftside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int maxu, CPlant * plant);
int prope_frontside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int minu, CPlant * plant);
int prope_backside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int maxu, CPlant * plant);


struct CodeBlock
{
    // holds order code (see defhelper for bitcodes)
    uint32_t data;
};
class CPlant
{
    public:
        CPlant(int initial_energy, vector<CodeBlock> *codes, int x, int y, int z);
        virtual ~CPlant();

        //int get_width();
        //int get_height();

        bool age(CSim * sim_ptr, CWorld * world_ptr, uint64_t * iterations);
        void die();

        void insert_code(int nr, int code);
        int delete_code(int nr);

        void update_energy_income(int delta_energy);

        void mutate_insert();
        void mutate_change();
        void mutate_delete();

        int get_biomass();
        int get_num_codes();

        // the position of the plant
        int get_x();
        int get_y();
        int get_z();

        int get_rightoffset();
        int get_topoffset();
        int get_depthoffset();
        bool is_seed();

        CodeBlock * get_code(int i);

    protected:

    private:


        void set_x(int x);
        void set_y(int y);
        void set_z(int z);


        // helper function
        void get_build_position_rnd(uint32_t data, int &roff, int &toff,int &doff, CWorld * world_ptr);
        void get_build_position_prop(uint32_t data, int &roff, int &toff,int &doff, CWorld * world_ptr);
        bool build_attempt_green(uint32_t data, int x, int y, int z, CSim *  sim_ptr , CWorld * world_ptr);
        bool build_attempt_seed(uint32_t data, int x, int y, int z, CSim *  sim_ptr , CWorld * world_ptr);


        template <down_travers dt>

        // function to return a point of a plant given the plane defined by the fracton in bvar1 as well as the orientation of the plane given by the rperp, uperp, and axis variables
        // axis is the direction of traversal of the plane
        // uperp is the direction "up" perpendicular to the axis
        // rperp is the direction "right" perpendicular to the axis
        // thus, the plane is given by the uperp and axis vectors
        // bvar2 contains the fraction of according to which the index of the axis point is calculated (of all outer plane points in the plane i)
        // the resulting point, specified by roff, toff, and doff is then the outermost point in the plane at this index
        void check_plane(int bvar1_frac, int bvar2_frac, int uperp_origin, int uperp_offset, int rperp_origin, int rperp_offset, int axis_origin, int axis_offset, int &new_uperp_offset, int &new_rperp_offset, int &new_axisoffset, CWorld * world_ptr);



        int m_x;
        int m_y;
        int m_z;

        // offset = width|height|depth -1
        int m_topoffset;
        int m_rightoffset;
        int m_depthoffset;

        // orders
        vector<CodeBlock> m_codes;

        // position of current code
        unsigned int m_current_pos;


        // characteristics: energy, number of cells...
        int64_t m_energy;
        int64_t m_energy_income;
        int m_num_green;
        bool m_seed_mode;

        friend class CPositionTests;
};

// appeareantly tempalte functions need to be implemented in the header file
template<down_travers dt>
void CPlant::check_plane(int bvar1_frac, int bvar2_frac, int uperp_origin, int uperp_offset, int rperp_origin, int rperp_offset, int axis_origin, int axis_offset, int &new_uperp_offset, int &new_rperp_offset, int &new_axisoffset, CWorld * world_ptr)
{
            assert((bvar1_frac>>16) <= 1);
            assert((bvar2_frac>>16) <= 1);

            new_rperp_offset = (bvar1_frac*rperp_offset+(1 << 15))>>16; // shift number back after multiplying with rightoffset to get the fractional distance, also round by adding 0.5 (1<<15)


            // scan  (at least once) the  plane  along the axis for the topmost (y) candidates of the plants
            // avoid using vector here to get around the mandatory initialization (without hacks) for performance reasons
            //std::unique_ptr<int[]> axis_points(new int[(axis_offset+1)*2]);

            // do it on the stack to be still faster, soulnd be an issue for the low dimension sizes
            int axis_points[(axis_offset+1)*2];
            int vi = 0;
            for(int k = axis_origin; k <= axis_origin + axis_offset; k++)
            {

#ifdef CORD_WRAP
                int cminy = dt(world_ptr, uperp_origin + uperp_offset, rperp_origin+new_rperp_offset, k, uperp_origin, this);
#else
                int cudist =  dt(world_ptr, uperp_origin + uperp_offset, rperp_origin+new_rperp_offset, k, uperp_origin, this);
#endif

                if(cudist <= uperp_offset)
                {
                    // plant has a cube at this coordinate
                    axis_points[vi]= k-axis_origin;
                    axis_points[vi+1]= cudist;
                    vi+=2;
                }
                else
                {
                    // plant has no cube at this coordinate
                }
            }
            // the index for getting th z coordinate is based on the fraction in bvar2 relative to the total number of uppermost zpoints
            // also round by adding 0.5 (1 << 15)
            // constraint: axis_points needs to contain at least one point
            // deduct 1 to get an 0-based index
            assert(vi>=2);
            int index = (bvar2_frac*((vi/2-1)/2)+(1<<15))>>16;

            // retrieve the z index back and calculate the offset, and do the same for the y index
            new_axisoffset = axis_points[index*2];
            new_uperp_offset = axis_points[index*2+1];

            assert(new_axisoffset <= axis_offset);
            assert(new_uperp_offset <= new_uperp_offset);

}



#endif // CPLANT_H
