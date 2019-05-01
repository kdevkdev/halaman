#include "CPlant.h"
#include "CWorld.h"

// ugly but didnt get it to work otherwise (with pointer-to-member functions and templates), the wrappers are also convenient to remap params correctly
int prope_topside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int minu, CPlant * plant)
{
    return wobj->onextdown( rperp, uperp, axis, minu, plant);
}
int prope_bottomside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int maxu, CPlant * plant)
{
    return wobj->onextup( rperp, uperp, axis, maxu, plant);
}
int prope_rigthside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int minu, CPlant * plant)
{
    return wobj->onextleft(uperp, rperp, axis, minu, plant);
}
int prope_leftside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int maxu, CPlant * plant)
{
    return wobj->onextright(uperp, rperp, axis, maxu, plant);
}
int prope_frontside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int minu, CPlant * plant)
{
    return wobj->onextin(axis, rperp, uperp, minu, plant);
}
int prope_backside_wrapper(CWorld *  wobj, int uperp, int rperp, int axis, int maxu, CPlant * plant)
{
    return wobj->onextinback(axis, rperp, uperp, maxu, plant);
}



CPlant::CPlant(int initial_energy, vector<CodeBlock> *codes, int x, int y, int z)
{
    // copy codes
    m_codes = *codes;
    m_num_green = 0;

    m_energy = initial_energy;
    m_energy_income = 0;
    m_seed_mode = true;

    m_x = x;
    m_y = y;
    m_z = z;
    m_topoffset = 0;
    m_rightoffset = 0;
    m_depthoffset = 0;

    m_current_pos = 0;
}

CPlant::~CPlant()
{
    //dtor
}
void CPlant::set_x(int x)
{
    m_x =x;
}
void CPlant::set_y(int y)
{
    m_y = y;
}
void CPlant::set_z(int z)
{
    m_z = z;
}
int CPlant::get_x()
{
    return m_x;
}
int CPlant::get_y()
{
    return m_y;
}
int CPlant::get_z()
{
    return m_z;
}
int CPlant::get_rightoffset()
{
    return m_rightoffset;
}
int CPlant::get_topoffset()
{
    return m_topoffset;
}
int CPlant::get_depthoffset()
{
    return m_depthoffset;
}
bool CPlant::age(CSim * sim_ptr, CWorld * world_ptr, uint64_t * iterations)
{
    // returne false if dead - true if still alive

    // check if seed
    if(m_seed_mode)
    {
        (*iterations)++;
        m_energy -= ECOST_SEED;

        if(m_energy <0)
        {
            die();
            return false;
        }

        // check if not on ground
        if(m_y >0)
        {
            // no, try to fall down
            if(world_ptr->fall_cell(m_x, m_y, m_z))
            {
                // fallen successfully, update coordinates
                m_y--;
            }
        }
        else
        {
            // yes,try to sprout
            m_energy -= ECOST_SPROUT;

            if(m_energy>0)
            {
                m_seed_mode = false;

                // energy income is the light density from the plants locatation
                m_energy_income+= world_ptr->get_light_density(m_x, m_y, m_z);
                world_ptr->sprout_cell(m_x, m_y, m_z);
                m_num_green++;
            }
            else
            {
                die();
                return false;
            }
        }
    }
    else
    {
        if(m_energy <0 || m_current_pos >= m_codes.size())
        {
            die();
            (*iterations)++;
            return false;
        }

        // add energ income
        m_energy += m_energy_income;

        // subtract all fixed running energy costs
        // die if not enough energy to mantain its build code and all its cells
        m_energy -= m_num_green* ECOST_GREEN;
        m_energy -= m_codes.size()*ECOST_CODE;

        // run loop to process build instructions as long as enough energy for next intruction
        bool lstop = m_energy < 0;
        while(!lstop)
        {
            (*iterations)++;
            // die if buildcodes are all implemented
            if(m_current_pos >= m_codes.size())
            {
                lstop = true;
            }
            else
            {

                int data = m_codes[m_current_pos].data;

                // only two different instructions: all not nop is build
                if(data & BC_NOP)
                {
                    if(m_energy > ECOST_NOP)
                    {
                        // nop
                        m_energy -= ECOST_NOP;

                        // advance one instruction
                        m_current_pos++;
                    }
                    else
                    {
                        // not enough energy - stop
                        lstop = true;
                    }
                }
                else
                {
                    // ok try to build - offsets from bottom left to top and right and front
                    int toff = 0;
                    int roff = 0;
                    int doff = 0;
                    int placement_mode = BC_BITMASK_PM & data;

                    /*if(placement_mode==BC_PM_RAND)
                    {
                        get_build_position_rnd(data,&x,&y,&z, world_ptr);
                        assert(y >= -1 && y <= WORLD_HEIGHT);
                        assert(x >= -1 && x <= WORLD_WIDTH);
                        estop = build_attempt(data, x, y, z, sim_ptr, world_ptr);
                    }*/

                    switch(BC_BITMASK_MAT & data)
                    {
                        case BC_BUILD_GREEN:

                            if(placement_mode==BC_PM_PROP)
                            {
                                get_build_position_prop(data,roff,toff,doff, world_ptr);
                                assert(m_y + toff >= -1 && m_y + toff <= WORLD_HEIGHT);
                                //assert(m_x+ roff>= -1 && m_x + roff  <= WORLD_WIDTH);
                                lstop = build_attempt_green(data, roff, toff,doff, sim_ptr, world_ptr);
                            }
                            else
                            {
                                // unknown buildcode - avoid stalling
                                m_current_pos++;
                            }

                        break;

                        case BC_BUILD_SEED:

                            if(placement_mode==BC_PM_PROP)
                            {
                                get_build_position_prop(data,roff,toff,doff, world_ptr);
                                assert(m_y + toff >= -1 && m_y + toff <= WORLD_HEIGHT);
                                //assert(m_x+ roff>= -1 && m_x + roff  <= WORLD_WIDTH);
                                lstop = build_attempt_seed(data, roff, toff,doff, sim_ptr, world_ptr);
                            }
                            else
                            {
                                // unknown buildcode - avoid stalling
                                m_current_pos++;
                            }

                        break;

                        default:
                            // advance one instruction (build order inconclusive, avoid eternal loop)
                            m_current_pos++;
                        break;
                    }
                }
            }
        }
    }
    return true;
}
bool CPlant::build_attempt_green(uint32_t data, int roff, int toff, int doff, CSim * sim_ptr, CWorld * world_ptr)
{
    bool estop = false;

    #ifdef CORD_WRAP

        int x = (m_x + roff)% WORLD_WIDTH;

        if(x < 0)
        {
            x += WORLD_WIDTH;
        }


    #else
        int x = m_x + roff;
    #endif

    int y = m_y + toff;
    int z = m_z + doff;


    // check if enough energy - otherwise dont advance
    if(m_energy > ECOST_BUILD_GREEN)
    {
        if(world_ptr->build_cell(x,y,z,this,MAT_PLANT_GREEN))
        {
            m_energy -= ECOST_BUILD_GREEN;

            // succeeded in building the cell
            m_num_green++;

            // update energy income (energy density from the location)
            m_energy_income += world_ptr->get_light_density(x,y,z);

            // update borders if necessary
            // check on equality to avoid having to decrement by -1
            if(roff > m_rightoffset)
            {
                m_rightoffset = roff;
            }
            else if(roff < 0)
            {
                m_x = x;
                m_rightoffset+= -roff;
            }

            if(toff > m_topoffset)
            {
                m_topoffset = toff;
            }
            else if(toff< 0)
            {
                m_y =  y;
                m_topoffset +=- toff;
            }

            if(doff > m_depthoffset)
            {
                m_depthoffset = doff;
            }
            else if(doff < 0)
            {
                m_z = z;
                m_depthoffset += -doff;
            }
        }
        else
        {
            // failed to build - calculate the factor
            m_energy -= (ECOST_BUILD_GREEN* ECOST_FACTOR_BFAIL_GREEN)/ECOST_FACTOR_BFAIL_MAX;
        }
        // advnce one instruction
        m_current_pos++;
    }
    else
    {
        // not enough energy - stop loop and try egaion next round
        estop = true;
    }

    assert(m_rightoffset >=0 && m_rightoffset < WORLD_WIDTH);
    return estop;
}
bool CPlant::build_attempt_seed(uint32_t data, int roff, int toff, int doff, CSim * sim_ptr, CWorld * world_ptr)
{
    bool estop = false;

    #ifdef CORD_WRAP

        int x = (m_x + roff)% WORLD_WIDTH;

        if(x < 0)
        {
            x += WORLD_WIDTH;
        }


    #else
        int x = m_x + roff;
    #endif

    int y = m_y + toff;
    int z = m_z + doff;

    // check if enough energy - otherwise dont advance
    if(m_energy > ECOST_BUILD_SEED)
    {
        // check if empty cell and build new plant
        if(world_ptr->check_cell_empty(x,y,z))
        {
            m_energy -= ECOST_BUILD_SEED;
            // span new plant
            CPlant * new_plant = new CPlant(INITIAL_ENERGY, &m_codes, x,y,z);
            // this function should not be used without explicit call to check_cell_empty first!
            world_ptr->build_cell_unchecked(x,y,z,new_plant,MAT_PLANT_SEED);

            // succeeded creating new plant
            sim_ptr->add_plant(new_plant);
        }
        else
        {
            // failed to build - calculate the factor
            m_energy -= (ECOST_BUILD_GREEN* ECOST_FACTOR_BFAIL_SEED)/ECOST_FACTOR_BFAIL_MAX;
        }
        // advnce one instruction
        m_current_pos++;
    }
    else
    {
        // not enough energy - stop loop and try egaion next round
        estop = true;
    }
    assert(m_rightoffset >=0 && m_rightoffset < WORLD_WIDTH);
    return estop;
}
void CPlant::get_build_position_rnd(uint32_t data, int &roff, int &toff,int &doff, CWorld * world_ptr)
{
    // create random position withing the plant boundaries and find the next point
    // in the according rectangular line the plant
/*
    // check for the direction
    switch(BC_BITMASK_DIR & data)
    {
        case BC_BUILD_DIR_UP:

            roff = rnd(0, m_rightoffset);
            toff = m_topoffset + world_ptr->onextbottom(m_x+roff,m_y+m_topoffset,this)+1;

        break;

        case BC_BUILD_DIR_RIGHT:

            toff = rnd(0,m_topoffset);
            roff = m_rightoffset + world_ptr->onextleft(m_x+m_rightoffset, m_y+toff,this)+1;

        break;

        case BC_BUILD_DIR_BOTTOM:

            roff = rnd(0, m_rightoffset);
            toff = world_ptr->onexttop(m_x+roff, m_y, this)-1;

        break;

        case BC_BUILD_DIR_LEFT:

            toff = rnd(0, m_topoffset);
            roff = world_ptr->onextright(m_x, m_y+toff, this)-1;

        break;
    }*/
}
void CPlant::get_build_position_prop(unsigned int data, int &roff, int &toff, int &doff, CWorld * world_ptr)
{
    // create random position withing the plant boundaries and find the next point
    // in the according rectangular line the plant


        // in this mode x is set by the value in bvar
    // (it is the fraction of the distance to the origin)
    // bvar has a max defined in defhelber.c of BC_BVAR_MAX, bvar/BC_BVAR_MAX is the resulting fraction
    // also perform a bitshift first to prevent loss of precision
    uint_fast64_t bvar1 = 0;
    bvar1 = (((uint_fast64_t)data & BC_BITMASK_BVAR1)>> BC_BVAR1_SHIFT);
    bvar1 =  (bvar1<<16);
    bvar1 = bvar1/BC_BVAR_MAX;


    uint_fast64_t bvar2 = 0;
    bvar2 = (((uint_fast64_t)data & BC_BITMASK_BVAR2)>> BC_BVAR2_SHIFT);
    bvar2 =  (bvar2<<16);
    bvar2 = bvar2/BC_BVAR_MAX;



    // check for the direction
    switch(BC_BITMASK_DIR & data)
    {
        case BC_BUILD_DIR_UP:

            check_plane<prope_topside_wrapper>(bvar1, bvar2, m_y, m_topoffset, m_x, m_rightoffset, m_z, m_depthoffset, toff, roff, doff, world_ptr);
            toff = m_topoffset - toff +1;

            assert(toff > -2 && toff - m_topoffset < 2);

        break;

        case BC_BUILD_DIR_DOWN:


            check_plane<prope_bottomside_wrapper>(bvar1, bvar2, m_y, m_topoffset, m_x, m_rightoffset, m_z, m_depthoffset, toff, roff, doff, world_ptr);
            toff--;

            assert(toff > -2 && toff - m_topoffset < 2);

        break;

        case BC_BUILD_DIR_RIGHT:

            check_plane<prope_rigthside_wrapper>(bvar1, bvar2, m_x, m_rightoffset, m_y, m_topoffset, m_z, m_depthoffset, roff, toff, doff, world_ptr);
            roff = m_rightoffset - roff +1;

            assert(roff > -2 && roff - m_rightoffset< 2);

        break;

        case BC_BUILD_DIR_LEFT:

            check_plane<prope_leftside_wrapper>(bvar1, bvar2, m_x, m_rightoffset, m_y, m_topoffset, m_z, m_depthoffset, roff, toff, doff, world_ptr);
            roff--;

            assert(roff > -2 && roff - m_rightoffset < 2);

        break;

        case BC_BUILD_DIR_FRONT:

            check_plane<prope_frontside_wrapper>(bvar1, bvar2, m_z, m_depthoffset, m_y, m_topoffset, m_x, m_rightoffset, doff, toff, roff, world_ptr);
            doff = m_depthoffset- doff +1;

        break;

        case BC_BUILD_DIR_BACK:

            check_plane<prope_backside_wrapper>(bvar1, bvar2, m_z, m_depthoffset, m_y, m_topoffset, m_x, m_rightoffset, doff, toff, roff, world_ptr);
            doff--;

        break;
    }
}
void CPlant::update_energy_income(int delta_energy)
{
    if(!m_seed_mode)
    {
        // in seed mode plant cannot consume!gain energy
        m_energy_income += delta_energy;
    }

    assert(m_energy_income >= 0);
}
void CPlant::die()
{

}
void CPlant::mutate_insert()
{
    // actualy performs duplication -> so there needs to be at least one element
    if(m_codes.size()>0)
    {
        // calculate insert position
        int pos = rnd(0,m_codes.size()-1);
        CodeBlock nc;
        nc.data = m_codes[pos].data;
        auto inspos = m_codes.begin()+pos;
        m_codes.insert(inspos,nc);
    }
}
void CPlant::mutate_delete()
{
    // delete random code
    // there needs to be at least one element
    if(m_codes.size()>0)
    {
        // calculate insert position
        int pos = rnd(0,m_codes.size()-1);
        auto rmpos = m_codes.begin()+pos;

        m_codes.erase(rmpos);
    }
}
void CPlant::mutate_change()
{
    // change random bit
    if(m_codes.size()>0)
    {
        // calculate insert position
        int pos = rnd(0,m_codes.size()-1);
        auto rndpos = m_codes.begin()+pos;

        // get a random bit
        int rbp = rnd(0,BC_NUM_BITS-1);

        // switch bit at the random position
        (*rndpos).data ^= 1 << rbp;
    }
}
int CPlant::get_biomass()
{
    return m_num_green + (int)m_seed_mode;
}
int CPlant::get_num_codes()
{
    return m_codes.size();
}
bool CPlant::is_seed()
{
    return m_seed_mode;
}
CodeBlock * CPlant::get_code(int i)
{
    return &m_codes[i];
}
