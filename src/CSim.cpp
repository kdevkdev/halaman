#include "CSim.h"
#include "CWorld.h"
#include "CWorldSD.h"

CSim::CSim()
{
    m_quit = false;
    m_running = false;
    m_world = new CWorldSD(WORLD_WIDTH, WORLD_HEIGHT, WORLD_DEPTH, DEF_LIGHT_DENSITY);
    //CWorldSD *worldsd = new CWorldSD(WORLD_WIDTH, WORLD_HEIGHT, DEF_LIGHT_DENSITY);

    m_rounds = 0;
    m_cum_iterations = 0;
    m_single_rounds = 0;
    m_sim_lock = 0;

    m_num_change_muts = 0;
    m_num_insert_muts = 0;
    m_num_delete_muts = 0;
    m_num_plants = 0;
    m_num_dead_plants = 0;
    m_codemass = 0;
    m_biomass = 0;
}

CSim::~CSim()
{
    delete m_world;
}
void CSim::quit()
{
    m_quit = true;
}
void CSim::run_sim()
{
    //  list of plants from the world -> go through list and "age" them
    list<CPlant *>::iterator current_plant = m_plant_list.begin();

    int biomass = 0;
    int codemass = 0;
    m_num_plants = m_plant_list.size();
    int seeds = 0;
    CPlant * cp;

    while(current_plant != m_plant_list.end())
    {
        cp = *current_plant;
        bool death = !cp->age(this,m_world, &m_cum_iterations);


        if(!cp->is_seed())
        {
            biomass+= cp->get_biomass();
        }
        else
        {
            seeds++;
        }

        codemass += cp->get_num_codes();
        if(death)
        {
            // plant has died - note that the iterator is alreaady advanced in the following statement
            // delete plant
            m_num_dead_plants++;
            m_world->remove_plant_cells(cp);

            // this also advances the iterator
            current_plant = m_plant_list.erase(current_plant);
            delete cp;
        }
        else
        {
            // do the mutations change - insert/duplicate - delete
            if(rnd(0, MUTPROB_MAX)< MUTPROB_CHANGE_PP + MUTPROB_CHANGE_PS*cp->get_biomass() + MUTPROB_CHANGE_PC*cp->get_num_codes())
            {
                cp->mutate_change();
                m_num_change_muts++;
            }
            if(rnd(0, MUTPROB_MAX)< MUTPROB_INSERT_PP + MUTPROB_INSERT_PS*cp->get_biomass() + MUTPROB_INSERT_PC*cp->get_num_codes())
            {
                cp->mutate_insert();
                m_num_insert_muts++;
            }
            if(rnd(0, MUTPROB_MAX)< MUTPROB_DELETE_PP + MUTPROB_DELETE_PS*cp->get_biomass() + MUTPROB_DELETE_PC*cp->get_num_codes())
            {
                cp->mutate_delete();
                m_num_delete_muts++;
            }
            // advance iterator if it has not been done yet
            current_plant++;
        }
    }
    m_biomass = biomass;
    m_codemass = codemass;
    m_num_seeds = seeds;
    m_rounds++;
}
int CSim::sim_loop()
{
    rnd_init(RAND_SEED);


    // initial genetic code
    vector<CodeBlock>codes;
    CodeBlock tc;
#ifdef PROFILERUN

    tc.data =  BC_BUILD_SEED | BC_BUILD_DIR_BACK  | BC_PM_PROP | ((BC_BVAR_MAX) << BC_BVAR1_SHIFT) | ((BC_BVAR_MAX) << BC_BVAR2_SHIFT);
    codes.push_back(tc);

    tc.data =  BC_BUILD_SEED | BC_BUILD_DIR_FRONT  | BC_PM_PROP | ((BC_BVAR_MAX) << BC_BVAR1_SHIFT) | ((BC_BVAR_MAX) << BC_BVAR2_SHIFT);
    codes.push_back(tc);

    // seed a plant<
    CPlant * plant = new CPlant(INITIAL_ENERGY,&codes,5,5,5);
    m_world->build_cell(5,5,5,plant,MAT_PLANT_SEED);

    m_biomass++;

#else



    tc.data =  BC_BUILD_SEED | BC_BUILD_DIR_BACK  | BC_PM_PROP | ((BC_BVAR_MAX) << BC_BVAR1_SHIFT) | ((BC_BVAR_MAX) << BC_BVAR2_SHIFT);
    codes.push_back(tc);

    tc.data =  BC_BUILD_SEED | BC_BUILD_DIR_FRONT  | BC_PM_PROP | ((BC_BVAR_MAX) << BC_BVAR1_SHIFT) | ((BC_BVAR_MAX) << BC_BVAR2_SHIFT);
    codes.push_back(tc);


   /* tc.data =  BC_BUILD_SEED | BC_BUILD_DIR_LEFT | BC_PM_PROP | ((BC_BVAR_MAX) << BC_BVAR1_SHIFT) | ((BC_BVAR_MAX) << BC_BVAR2_SHIFT);
    codes.push_back(tc);
    tc.data =  BC_BUILD_SEED | BC_BUILD_DIR_RIGHT | BC_PM_PROP | ((BC_BVAR_MAX) << BC_BVAR1_SHIFT) | ((BC_BVAR_MAX) << BC_BVAR2_SHIFT);
    codes.push_back(tc);*/

        // seed a plant<
    CPlant * plant = new CPlant(INITIAL_ENERGY,&codes,5,5,5);
    m_world->build_cell(5,5,5,plant,MAT_PLANT_SEED);

    m_biomass++;

#endif // PROFILERUN

    m_plant_list.push_front(plant);

    // iteration limit is for profiling for profiling
#ifdef PROFILERUN
    while(!m_quit && m_cum_iterations < 9000000)
#else
    while(!m_quit)
#endif
    {
        if(m_running)
        {
            run_sim();
        }
        else
        {
            // check for single runs
            // lock
            SDL_AtomicLock( &m_sim_lock );

            if(m_single_rounds>0)
            {
                m_single_rounds--;
                run_sim();
            }
            // unlock
            SDL_AtomicUnlock(&m_sim_lock);

            SDL_Delay(30);
        }
    }

    // delete remainnig plants
    list<CPlant *>::iterator current_plant = m_plant_list.begin();
    CPlant * cp;

    while(current_plant != m_plant_list.end())
    {
        cp = * current_plant;
        m_num_dead_plants++;
        m_world->remove_plant_cells(cp);
        current_plant = m_plant_list.erase(current_plant);
        delete cp;
    }

    return 1;
}
void CSim::add_plant(CPlant * plant)
{
    m_plant_list.push_front(plant);
}
CWorld * CSim::get_world()
{
    return m_world;
}
uint64_t CSim::get_cum_iterations(void)
{
    return m_cum_iterations;
}
uint64_t CSim::get_rounds(void)
{
    return m_rounds;
}
void CSim::flip_stop(void)
{
    m_running =! m_running;

    // lock
    SDL_AtomicLock( &m_sim_lock );

    // reset this every time its paused, to avoid unexpected runs
    m_single_rounds = 0;

    // unlock
    SDL_AtomicUnlock(&m_sim_lock);
}
void CSim::single_round()
{
    // lock
    SDL_AtomicLock( &m_sim_lock );

    m_single_rounds++;

    // unlock
    SDL_AtomicUnlock(&m_sim_lock);
}
int CSim::get_num_change_muts(void)
{
    return m_num_change_muts;
}
int CSim::get_num_insert_muts(void)
{
    return m_num_insert_muts;
}
int CSim::get_num_delete_muts(void)
{
    return m_num_delete_muts;
}
int CSim::get_biomass()
{
    return m_biomass;
}
int CSim::get_codemass()
{
    return m_codemass;
}
int CSim::get_num_plants()
{
    return m_num_plants-m_num_seeds;
}
uint64_t CSim::get_num_dead_plants()
{
    return m_num_dead_plants;
}
int CSim::get_num_seeds()
{
    return m_num_seeds;
}
