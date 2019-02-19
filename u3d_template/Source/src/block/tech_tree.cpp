
#include "tech_tree.h"


namespace Osp
{

TechTree::TechTree()
{
    debugInit();
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

void TechTree::debugInit()
{
    PartDesc pd;
    pd.name        = "Box";
    pd.description = "Test box-like construction block";
    pd.category    = "test";
    pd.icon        = "Textures/Spot.png";

    ConnectionDesc conn;
    conn.r = Vector3( 0.0, 1.0, 0.0 );
    pd.connections.push_back( conn );

    conn.r = Vector3( 0.0, -1.0, 0.0 );
    pd.connections.push_back( conn );

    partDescs.push_back( pd );

    pd.name = "Box_2";
    partDescs.push_back( pd );

    CategoryDesc cd;
    cd.name = "Category 1";
    cd.description = "None";
    cd.items.push_back( 0 );
    cd.items.push_back( 1 );

    panelContent.push_back( cd );
}

}



