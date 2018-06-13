
#ifndef __SPHERE_SOURCE_H_
#define __SPHERE_SOURCE_H_

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreVolumeChunk.h"
#include "OgreVolumeCSGSource.h"
#include "OgreVolumeCacheSource.h"
#include "OgreVolumeTextureSource.h"

#include "noise/noise.h"
#include "noiseutils.h"

using namespace Ogre;
using namespace Volume;

class SphereSource: public Source
{
protected:
    const Real mR;
    const Vector3 mCenter;
public:

    SphereSource(const Real r, const Vector3 &center);
    virtual Vector4 getValueAndGradient(const Vector3 &position) const;
    virtual Real getValue(const Vector3 &position) const;

    // Noise generators for building a landscape.
    module::Perlin baseContinentDef_pe0;
    module::Curve baseContinentDef_cu;
    module::Perlin baseContinentDef_pe1;
    module::ScaleBias baseContinentDef_sb;
    module::Min baseContinentDef_mi;
    module::Clamp baseContinentDef_cl;
    module::Cache baseContinentDef;
    module::Turbulence continentDef_tu0;
    module::Turbulence continentDef_tu1;
    module::Turbulence continentDef_tu2;
    module::Select continentDef_se;
    module::Cache continentDef;
    module::Turbulence terrainTypeDef_tu;
    module::Terrace terrainTypeDef_te;
    module::Cache terrainTypeDef;
    module::RidgedMulti mountainBaseDef_rm0;
    module::ScaleBias mountainBaseDef_sb0;
    module::RidgedMulti mountainBaseDef_rm1;
    module::ScaleBias mountainBaseDef_sb1;
    module::Const mountainBaseDef_co;
    module::Blend mountainBaseDef_bl;
    module::Turbulence mountainBaseDef_tu0;
    module::Turbulence mountainBaseDef_tu1;
    module::Cache mountainBaseDef;
    module::RidgedMulti mountainousHigh_rm0;
    module::RidgedMulti mountainousHigh_rm1;
    module::Max mountainousHigh_ma;
    module::Turbulence mountainousHigh_tu;
    module::Cache mountainousHigh;
    module::RidgedMulti mountainousLow_rm0;
    module::RidgedMulti mountainousLow_rm1;
    module::Multiply mountainousLow_mu;
    module::Cache mountainousLow;
    module::ScaleBias mountainousTerrain_sb0;
    module::ScaleBias mountainousTerrain_sb1;
    module::Add mountainousTerrain_ad;
    module::Select mountainousTerrain_se;
    module::ScaleBias mountainousTerrain_sb2;
    module::Exponent mountainousTerrain_ex;
    module::Cache mountainousTerrain;
    module::Billow hillyTerrain_bi;
    module::ScaleBias hillyTerrain_sb0;
    module::RidgedMulti hillyTerrain_rm;
    module::ScaleBias hillyTerrain_sb1;
    module::Const hillyTerrain_co;
    module::Blend hillyTerrain_bl;
    module::ScaleBias hillyTerrain_sb2;
    module::Exponent hillyTerrain_ex;
    module::Turbulence hillyTerrain_tu0;
    module::Turbulence hillyTerrain_tu1;
    module::Cache hillyTerrain;
    module::Billow plainsTerrain_bi0;
    module::ScaleBias plainsTerrain_sb0;
    module::Billow plainsTerrain_bi1;
    module::ScaleBias plainsTerrain_sb1;
    module::Multiply plainsTerrain_mu;
    module::ScaleBias plainsTerrain_sb2;
    module::Cache plainsTerrain;
    module::RidgedMulti badlandsSand_rm;
    module::ScaleBias badlandsSand_sb0;
    module::Voronoi badlandsSand_vo;
    module::ScaleBias badlandsSand_sb1;
    module::Add badlandsSand_ad;
    module::Cache badlandsSand;
    module::Perlin badlandsCliffs_pe;
    module::Curve badlandsCliffs_cu;
    module::Clamp badlandsCliffs_cl;
    module::Terrace badlandsCliffs_te;
    module::Turbulence badlandsCliffs_tu0;
    module::Turbulence badlandsCliffs_tu1;
    module::Cache badlandsCliffs;
    module::ScaleBias badlandsTerrain_sb;
    module::Max badlandsTerrain_ma;
    module::Cache badlandsTerrain;
    module::RidgedMulti riverPositions_rm0;
    module::Curve riverPositions_cu0;
    module::RidgedMulti riverPositions_rm1;
    module::Curve riverPositions_cu1;
    module::Min riverPositions_mi;
    module::Turbulence riverPositions_tu;
    module::Cache riverPositions;
    module::ScaleBias scaledMountainousTerrain_sb0;
    module::Perlin scaledMountainousTerrain_pe;
    module::Exponent scaledMountainousTerrain_ex;
    module::ScaleBias scaledMountainousTerrain_sb1;
    module::Multiply scaledMountainousTerrain_mu;
    module::Cache scaledMountainousTerrain;
    module::ScaleBias scaledHillyTerrain_sb0;
    module::Perlin scaledHillyTerrain_pe;
    module::Exponent scaledHillyTerrain_ex;
    module::ScaleBias scaledHillyTerrain_sb1;
    module::Multiply scaledHillyTerrain_mu;
    module::Cache scaledHillyTerrain;
    module::ScaleBias scaledPlainsTerrain_sb;
    module::Cache scaledPlainsTerrain;
    module::ScaleBias scaledBadlandsTerrain_sb;
    module::Cache scaledBadlandsTerrain;
    module::Terrace continentalShelf_te;
    module::RidgedMulti continentalShelf_rm;
    module::ScaleBias continentalShelf_sb;
    module::Clamp continentalShelf_cl;
    module::Add continentalShelf_ad;
    module::Cache continentalShelf;
    module::ScaleBias baseContinentElev_sb;
    module::Select baseContinentElev_se;
    module::Cache baseContinentElev;
    module::Add continentsWithPlains_ad;
    module::Cache continentsWithPlains;
    module::Add continentsWithHills_ad;
    module::Select continentsWithHills_se;
    module::Cache continentsWithHills;
    module::Add continentsWithMountains_ad0;
    module::Curve continentsWithMountains_cu;
    module::Add continentsWithMountains_ad1;
    module::Select continentsWithMountains_se;
    module::Cache continentsWithMountains;
    module::Perlin continentsWithBadlands_pe;
    module::Add continentsWithBadlands_ad;
    module::Select continentsWithBadlands_se;
    module::Max continentsWithBadlands_ma;
    module::Cache continentsWithBadlands;
    module::ScaleBias continentsWithRivers_sb;
    module::Add continentsWithRivers_ad;
    module::Select continentsWithRivers_se;
    module::Cache continentsWithRivers;
    module::Cache unscaledFinalPlanet;
    module::ScaleBias finalPlanet_sb;
    module::Cache finalPlanet;
};






#endif
