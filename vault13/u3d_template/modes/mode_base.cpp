
#include "mode_base.h"

namespace Osp
{

ModeBase::ModeBase( Context * ctx )
    : LogicComponent( ctx )
{
    name_ = "ModeBase";
    active_ = false;
}

ModeBase::~ModeBase()
{

}

const String & ModeBase::name() const
{
    return name_;
}

void ModeBase::activate()
{
    active_ == true;
}

void ModeBase::deactivate()
{
    active_ = false;
}

bool ModeBase::isActive() const
{
    return active_;
}

void ModeBase::DelayedStart()
{
    active_ = true;
    activate();
}

void ModeBase::Stop()
{
    deactivate();
    active_ = false;
}

}

