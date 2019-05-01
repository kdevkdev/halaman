#ifndef DEFHELPER_H
#define DEFHELPER_H

#include "xoroshiro128.h"

using namespace std;

// preprocessor constants are bad ... never use them they said ... well, look who is lazy

// random number stuff
// make it global because the intention is to use just one randome object and seed it
// so that the simulation becomes deterministic
// to avoid passing this random object to every class make it global
// important to only use it in one thread (generate numbers with the rnd function)
#define RAND_SEED 4444
//extern mt19937 rand_gen;
//extern minstd_rand rand_gen;


//Screen dimension constants
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 768

#define FOV_ANGLE 45.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 1000.0f

#define CROSSHAIR_LENGTH SCREEN_WIDTH/100.0f


#define WORLD_WIDTH 120
#define WORLD_HEIGHT 50
#define WORLD_DEPTH 120

//#define CORD_WRAP
//#define PROFILERUN

#define MAT_EMPTY 0
#define MAT_PLANT_GREEN 1
#define MAT_PLANT_SEED 2

#define GREEN_COLOR_R 0.0f
#define GREEN_COLOR_G 0.8f
#define GREEN_COLOR_B 0.0f

#define SEED_COLOR_R 0.5f
#define SEED_COLOR_G 0.2f
#define SEED_COLOR_B 0.05f

#define GREEN_HIGHLIGHT_COLOR_R 0.8f
#define GREEN_HIGHLIGHT_COLOR_G 1.0f
#define GREEN_HIGHLIGHT_COLOR_B 0.8f

#define SEED_HIGHLIGHT_COLOR_R 0.9f
#define SEED_HIGHLIGHT_COLOR_G 0.6f
#define SEED_HIGHLIGHT_COLOR_B 0.45f



// bits for the 32 bit buildcode (for now on) mmmmmmmmmmmmnnnnnnnnnnnnzppxxyyy, where
// - is unused
// n is a variable bvar1, 12 bits
// m is another variable bvar2, 12 bits
// z is nop
// p is placement mode
// x is the type of material
// y is the build direction


// masks and
// define  bits to test
// buildcodes- directions
#define BC_BITMASK_DIR 7

#define BC_BUILD_DIR_UP  0
#define BC_BUILD_DIR_RIGHT  1
#define BC_BUILD_DIR_LEFT   2
#define BC_BUILD_DIR_DOWN  3
#define BC_BUILD_DIR_FRONT 4
#define BC_BUILD_DIR_BACK 5

// materials
#define BC_BITMASK_MAT (3 << 3)

#define BC_BUILD_GREEN  (0 << 3)
#define BC_BUILD_SEED   (1 << 3)
// other materials not yet used - two reserve

// nop bit
#define BC_NOP             (1 << 7)

// placement mode
#define BC_BITMASK_PM       (3 << 5)
#define BC_PM_RAND          (0 << 5)
#define BC_PM_PROP          (1 << 5)

// maximal value of bvar
#define BC_BVAR_MAX          0xFFF

// shifts for bvar1 and bvar2
#define BC_BVAR1_SHIFT         8
#define BC_BVAR2_SHIFT        20

// build variables masks
#define BC_BITMASK_BVAR1     (0xFFF << BC_BVAR1_SHIFT)
#define BC_BITMASK_BVAR2     (0xFFF << BC_BVAR2_SHIFT)

// how many bits are used for the code?
#define BC_NUM_BITS           32


// how much energy comes in? how much is obstructed by an occupied cell?
#define DEF_LIGHT_DENSITY 400
#define MIN_LIGHT_DENSITY 50
#define LIGHT_REDUCTION 100
#define INITIAL_ENERGY     200

// energy costs to maintain
#define ECOST_SEED          6
#define ECOST_GREEN         6
#define ECOST_CODE          1

// cost of doingnothing
#define ECOST_NOP           5

// build costs
#define ECOST_SPROUT       40
#define ECOST_BUILD_SEED   40
#define ECOST_BUILD_GREEN  20

//  in 1/ MAX
#define ECOST_FACTOR_BFAIL_GREEN 5000
#define ECOST_FACTOR_BFAIL_SEED 5000
#define ECOST_FACTOR_BFAIL_MAX 10000

// Mutation probaabilities: in 1/1 000 000
#define MUTPROB_MAX         100000000

// mutations - three kinds: change byte, insertion, deletions
// probabilities per round per plant
#define MUTPROB_CHANGE_PP        400
#define MUTPROB_INSERT_PP        350
#define MUTPROB_DELETE_PP        200

// probabilities per round per plant per size
#define MUTPROB_CHANGE_PS        0
#define MUTPROB_INSERT_PS        0
#define MUTPROB_DELETE_PS        0

// probabilities per round per plant per code
#define MUTPROB_CHANGE_PC        40
#define MUTPROB_INSERT_PC        0
#define MUTPROB_DELETE_PC        0

// mouse (yaw and pitch),zoom and move speed
#define TURN_SENSIVITY    3.0f
#define ZOOM_SPEED        2
#define MOVE_SPEED        40
#define SCROLL_SPEED      30 // in pixels


#define TARGET_FRAMERATE   29


//grid
#define GRID_COLOR 0x272727

// outline
#define OUTLINE_COLOR  0xFF7777




#endif // DEFHELPER_H_INCLUDED
