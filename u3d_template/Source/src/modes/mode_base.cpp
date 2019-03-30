
#include "mode_base.h"

namespace Osp
{

ModeBase::ModeBase( Context * ctx )
    : LogicComponent( ctx )
{
    name_ = "ModeBase";
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

}

void ModeBase::deactivate()
{

}

void ModeBase::DelayedStart()
{
    activate();
}

void ModeBase::Stop()
{
    deactivate();
}

}

