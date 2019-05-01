#include "CViewer.h"
#include "CSim.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdio.h>

int sim_thread_func( void* data )
{
    // run simulation
    printf("sim thread started");

    CSim * sim = (CSim *) data;
    sim->sim_loop();

    return 0;
}
int main( int argc, char* argv[] )
{
    CSim simulation;

#ifdef PROFILERUN
    //this code is for profiling - valrgrind has trouble with the second thread so run just the simulation without grahphciss
    simulation.flip_stop();
    simulation.sim_loop();
#else

    SDL_Thread * sim_thread = SDL_CreateThread( sim_thread_func, "sim_thread", (void*) &simulation);

    CViewer  viewer(&simulation);
    viewer.render_loop();


    SDL_WaitThread( sim_thread, nullptr );

#endif

    return 0;
}
