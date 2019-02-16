
#include "tech_tree.h"


namespace Osp
{

TechTree::TechTree()
{

}

TechTree::~TechTree()
{

}

const PartDesc & TechTree::partDesc( const String & name )
{
    const size_t partsQty = partDescs.size();
    for ( size_t i=0; i<partsQty; i++ )
    {
        const PartDesc & pd = partDescs[i];
        if ( pd.name == name )
            return pd;
    }
    static PartDesc failureDesc;
    return failureDesc;
}

std::vector<CategoryDesc> & TechTree::getPanelContent()
{
    return panelContent;
}

std::vector<PartDesc> & TechTree::getPartDescs()
{
    return partDescs;
}

}




