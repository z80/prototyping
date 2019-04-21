
#include "state_manager.h"
#include "state_intro.h"

int main(int argc, char *argv[])
{
    StateManager * sm = StateManager::getSingletonPtr();
    sm->initApp();
    sm->start( IntroState::getSingletonPtr() );
    sm->closeApp();

    return 0;
}
