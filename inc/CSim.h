#ifndef CSIM_H
#define CSIM_H

#include "defhelper.h"

//#include "CWorld.h"
//#include "CWorldSD.h"
//#include "CPlant.h"
#include <SDL2/SDL_thread.h>
#include <list>

class CPlant;
class CWorld;


class CSim
{
    public:
        CSim();
        virtual ~CSim();
        int sim_loop();

        void quit();

        void add_plant(CPlant * plant);
        CWorld * get_world();

        uint64_t get_cum_iterations(void);
        uint64_t get_rounds(void);

        // function to pause the simulation
        void flip_stop();

        // function to run a single round of the simulation (only possible when stopped)
        void single_round();

        int get_num_change_muts(void);
        int get_num_insert_muts(void);
        int get_num_delete_muts(void);
        int get_biomass(void);
        int get_codemass(void);
        int get_num_plants(void);
        uint_fast64_t get_num_dead_plants(void);
        int get_num_seeds(void);


    protected:

    private:

        void run_sim();
        bool m_quit;
        bool m_running;




        // # of mutations
        int m_num_change_muts;
        int m_num_insert_muts;
        int m_num_delete_muts;


        // int single rounds : contains the number of single iterations to be done in stopped mode, increased
        // by single round
        int m_single_rounds;

        // spinlock
        SDL_SpinLock m_sim_lock;

        // world age
        uint64_t m_rounds;

        // all iterations
        uint64_t m_cum_iterations;

        // plants
        list<CPlant*> m_plant_list;

        // contains all the spatial representations, like density, cells
        CWorld * m_world;

        // biomass
        int m_biomass;
        int m_codemass;
        int m_num_plants;
        int m_num_seeds;
        uint64_t m_num_dead_plants;

};

#endif // CSIM_H
