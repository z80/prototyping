#include <Urho3D/Urho3DAll.h>

// Control bits we define
static const unsigned CTRL_FORWARD = 1 << 0; //1
static const unsigned CTRL_BACK = 1 << 2; //2
static const unsigned CTRL_LEFT = 1 << 3; //4
static const unsigned CTRL_RIGHT = 1 << 4; //8
static const unsigned CTRL_JUMP = 1 << 5; //16
static const unsigned CTRL_ACTION = 1 << 6; //32
static const unsigned CTRL_SPRINT = 1 << 7; //64
static const unsigned CTRL_UP = 1 << 8;

// My own actions.
static const unsigned CTRL_YAW_UP   = 1 << 9;
static const unsigned CTRL_YAW_DN   = 1 << 10;
static const unsigned CTRL_PITCH_UP = 1 << 11;
static const unsigned CTRL_PITCH_DN = 1 << 12;
static const unsigned CTRL_ROLL_UP  = 1 << 13;
static const unsigned CTRL_ROLL_DN  = 1 << 14;
static const unsigned CTRL_TRANSLATION  = 1 << 15;
static const unsigned CTRL_TRIGGER  = 1 << 16;
static const unsigned CTRL_THRUST_UP  = 1 << 17;
static const unsigned CTRL_THRUST_DN  = 1 << 18;

static const unsigned COLLISION_MASK_PLAYER = 1 << 0; //1
static const unsigned COLLISION_MASK_CHECKPOINT = 1 << 1; //2 
static const unsigned COLLISION_MASK_OBSTACLES = 1 << 2; //4

static String APPLICATION_FONT = "Fonts/Muli-Regular.ttf";
