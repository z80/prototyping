
#include "sphere_source.h"

#include "noise/noise.h"
#include "noiseutils.h"


namespace IcoHeightmap
{


DumbSphere::DumbSphere( int32 n )
  : maxLevel( n )
{
}

DumbSphere::~DumbSphere()
{
}

bool DumbSphere::subdrive( const Icosphere * s, const Triangle * tri ) const
{
    const bool doThat = ( tri->level < maxLevel );
    return doThat;
}






SphereSubdrive::SphereSubdrive()
{
    maxLevel1 = 7;
    maxLevel2 = 11;
    maxLevel3 = 18;

    maxD2 = 0.09;
    maxD3 = 0.001;

    at.x =  0.0;
    at.y =  0.0;
    at.z =  1.0;
}

SphereSubdrive::~SphereSubdrive()
{
}

void SphereSubdrive::setCameraAt(const Vector3 & camAt )
{
    NeedSubdrive::setCameraAt( camAt );
    this->camNormAt = camAt;
    this->camNormAt.normalise();
}

bool SphereSubdrive::subdrive( const Icosphere * s, const Triangle * tri ) const
{
    for ( int32 i=0; i<3; i++ )
    {
        const int32 vertInd = tri->vertInds[i];
        const Vertex & v = s->verts[vertInd];
        const Real d = distanceL1( at, v.at );
        if ( d <= maxD2 )
        {
            if ( tri->level < maxLevel2 )
                return true;
            if ( d <= maxD3 )
            {
                if ( tri->level < maxLevel3 )
                    return true;
            }
            const Real dNorm = distanceL1( camNormAt, v.at );
            if ( dNorm <= maxD3 )
            {
                if ( tri->level < maxLevel3 )
                    return true;
            }
        }
    }
    if ( tri->level < maxLevel1 )
        return true;
    return false;
}

SphereRebuild::SphereRebuild()
{
    firstTime = true;
}

SphereRebuild::~SphereRebuild()
{

}

bool SphereRebuild::rebuild( Source * src, const Vector3 & at )
{
    if ( firstTime )
    {
        firstTime = false;
        camAt = at;
        return true;
    }
    Vector3 unitAt = at;
    unitAt.normalise();
    const Real h = src->dh( unitAt );
    const Real r = 1.0 + h;
    Vector3 surfaceAt = unitAt * r;
    // Distance to the surface.
    const Real height = at.distance( surfaceAt );
    // Distance to previous location.
    const Real dist   = at.distance( camAt );

    if ( height < 0.01 )
    {
        if ( dist > 0.00015 )
        {
            camAt = at;
            return true;
        }
        return false;
    }
    else if ( height < 0.005 )
    {
        if ( dist > 0.0025 )
        {
            camAt = at;
            return true;
        }
        return false;
    }
    return false;
}











class SphereSource::PD
{
public:
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


    PD();
    ~PD();
    void init();
    Real dh( const Vector3 & at ) const;
};

SphereSource::PD::PD()
{
    init();
}

SphereSource::PD::~PD()
{
}

void SphereSource::PD::init()
{
// Southernmost coordinate of elevation grid.
const double SOUTH_COORD = -90;

// Northernmost coordinate of elevation grid.
const double NORTH_COORD = 90;

// Westernmost coordinate of elevation grid.
const double WEST_COORD = -180;

// Easternmost coordinate of elevation grid.
const double EAST_COORD = 180;

// Width of elevation grid, in points.
const int GRID_WIDTH = 4096;

// Height of elevation grid, in points.
const int GRID_HEIGHT = 2048;

// Planet seed.  Change this to generate a different planet.
const int CUR_SEED = 0;

// Circumference of the planet, in meters.
const double PLANET_CIRCUMFERENCE = 44236800.0;

// Minimum elevation on the planet, in meters.  This value is approximate.
const double MIN_ELEV = -0.001; //-8192.0;

// Maximum elevation on the planet, in meters.  This value is approximate.
const double MAX_ELEV = 0.001; //8192.0;

// Frequency of the planet's continents.  Higher frequency produces smaller,
// more numerous continents.  This value is measured in radians.
const double CONTINENT_FREQUENCY = 1.0;

// Lacunarity of the planet's continents.  Changing this value produces
// slightly different continents.  For the best results, this value should
// be random, but close to 2.0.
const double CONTINENT_LACUNARITY = 2.208984375;

// Lacunarity of the planet's mountains.  Changing this value produces
// slightly different mountains.  For the best results, this value should
// be random, but close to 2.0.
const double MOUNTAIN_LACUNARITY = 2.142578125;

// Lacunarity of the planet's hills.  Changing this value produces slightly
// different hills.  For the best results, this value should be random, but
// close to 2.0.
const double HILLS_LACUNARITY = 2.162109375;

// Lacunarity of the planet's plains.  Changing this value produces slightly
// different plains.  For the best results, this value should be random, but
// close to 2.0.
const double PLAINS_LACUNARITY = 2.314453125;

// Lacunarity of the planet's badlands.  Changing this value produces
// slightly different badlands.  For the best results, this value should be
// random, but close to 2.0.
const double BADLANDS_LACUNARITY = 2.212890625;

// Specifies the "twistiness" of the mountains.
const double MOUNTAINS_TWIST = 1.0;

// Specifies the "twistiness" of the hills.
const double HILLS_TWIST = 1.0;

// Specifies the "twistiness" of the badlands.
const double BADLANDS_TWIST = 1.0;

// Specifies the planet's sea level.  This value must be between -1.0
// (minimum planet elevation) and +1.0 (maximum planet elevation.)
const double SEA_LEVEL = 0.0;

// Specifies the level on the planet in which continental shelves appear.
// This value must be between -1.0 (minimum planet elevation) and +1.0
// (maximum planet elevation), and must be less than SEA_LEVEL.
const double SHELF_LEVEL = -0.375;

// Determines the amount of mountainous terrain that appears on the
// planet.  Values range from 0.0 (no mountains) to 1.0 (all terrain is
// covered in mountains).  Mountainous terrain will overlap hilly terrain.
// Because the badlands terrain may overlap parts of the mountainous
// terrain, setting MOUNTAINS_AMOUNT to 1.0 may not completely cover the
// terrain in mountains.
const double MOUNTAINS_AMOUNT = 0.5;

// Determines the amount of hilly terrain that appears on the planet.
// Values range from 0.0 (no hills) to 1.0 (all terrain is covered in
// hills).  This value must be less than MOUNTAINS_AMOUNT.  Because the
// mountainous terrain will overlap parts of the hilly terrain, and
// the badlands terrain may overlap parts of the hilly terrain, setting
// HILLS_AMOUNT to 1.0 may not completely cover the terrain in hills.
const double HILLS_AMOUNT = (1.0 + MOUNTAINS_AMOUNT) / 2.0;

// Determines the amount of badlands terrain that covers the planet.
// Values range from 0.0 (no badlands) to 1.0 (all terrain is covered in
// badlands.)  Badlands terrain will overlap any other type of terrain.
const double BADLANDS_AMOUNT = 0.03125;

// Offset to apply to the terrain type definition.  Low values (< 1.0) cause
// the rough areas to appear only at high elevations.  High values (> 2.0)
// cause the rough areas to appear at any elevation.  The percentage of
// rough areas on the planet are independent of this value.
const double TERRAIN_OFFSET = 1.0;

// Specifies the amount of "glaciation" on the mountains.  This value
// should be close to 1.0 and greater than 1.0.
const double MOUNTAIN_GLACIATION = 1.375;

// Scaling to apply to the base continent elevations, in planetary elevation
// units.
const double CONTINENT_HEIGHT_SCALE = (1.0 - SEA_LEVEL) / 4.0;

// Maximum depth of the rivers, in planetary elevation units.
const double RIVER_DEPTH = 0.0234375;

    //module::Perlin baseContinentDef_pe0;
    baseContinentDef_pe0.SetSeed (CUR_SEED + 0);
    baseContinentDef_pe0.SetFrequency (CONTINENT_FREQUENCY);
    baseContinentDef_pe0.SetPersistence (0.5);
    baseContinentDef_pe0.SetLacunarity (CONTINENT_LACUNARITY);
    baseContinentDef_pe0.SetOctaveCount (14);
    baseContinentDef_pe0.SetNoiseQuality (QUALITY_STD);

    // 2: [Continent-with-ranges module]: Next, a curve module modifies the
    //    output value from the continent module so that very high values appear
    //    near sea level.  This defines the positions of the mountain ranges.
    //module::Curve baseContinentDef_cu;
    baseContinentDef_cu.SetSourceModule (0, baseContinentDef_pe0);
    baseContinentDef_cu.AddControlPoint (-2.0000 + SEA_LEVEL,-1.625 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint (-1.0000 + SEA_LEVEL,-1.375 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint ( 0.0000 + SEA_LEVEL,-0.375 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint ( 0.0625 + SEA_LEVEL, 0.125 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint ( 0.1250 + SEA_LEVEL, 0.250 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint ( 0.2500 + SEA_LEVEL, 1.000 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint ( 0.5000 + SEA_LEVEL, 0.250 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint ( 0.7500 + SEA_LEVEL, 0.250 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint ( 1.0000 + SEA_LEVEL, 0.500 + SEA_LEVEL);
    baseContinentDef_cu.AddControlPoint ( 2.0000 + SEA_LEVEL, 0.500 + SEA_LEVEL);

    // 3: [Carver module]: This higher-frequency Perlin-noise module will be
    //    used by subsequent noise modules to carve out chunks from the mountain
    //    ranges within the continent-with-ranges module so that the mountain
    //    ranges will not be complely impassible.
    //module::Perlin baseContinentDef_pe1;
    baseContinentDef_pe1.SetSeed (CUR_SEED + 1);
    baseContinentDef_pe1.SetFrequency (CONTINENT_FREQUENCY * 4.34375);
    baseContinentDef_pe1.SetPersistence (0.5);
    baseContinentDef_pe1.SetLacunarity (CONTINENT_LACUNARITY);
    baseContinentDef_pe1.SetOctaveCount (11);
    baseContinentDef_pe1.SetNoiseQuality (QUALITY_STD);

    // 4: [Scaled-carver module]: This scale/bias module scales the output
    //    value from the carver module such that it is usually near 1.0.  This
    //    is required for step 5.
    //module::ScaleBias baseContinentDef_sb;
    baseContinentDef_sb.SetSourceModule (0, baseContinentDef_pe1);
    baseContinentDef_sb.SetScale (0.375);
    baseContinentDef_sb.SetBias (0.625);

    // 5: [Carved-continent module]: This minimum-value module carves out chunks
    //    from the continent-with-ranges module.  It does this by ensuring that
    //    only the minimum of the output values from the scaled-carver module
    //    and the continent-with-ranges module contributes to the output value
    //    of this subgroup.  Most of the time, the minimum-value module will
    //    select the output value from the continents-with-ranges module since
    //    the output value from the scaled-carver module is usually near 1.0.
    //    Occasionally, the output value from the scaled-carver module will be
    //    less than the output value from the continent-with-ranges module, so
    //    in this case, the output value from the scaled-carver module is
    //    selected.
    //module::Min baseContinentDef_mi;
    baseContinentDef_mi.SetSourceModule (0, baseContinentDef_sb);
    baseContinentDef_mi.SetSourceModule (1, baseContinentDef_cu);

    // 6: [Clamped-continent module]: Finally, a clamp module modifies the
    //    carved-continent module to ensure that the output value of this
    //    subgroup is between -1.0 and 1.0.
    //module::Clamp baseContinentDef_cl;
    baseContinentDef_cl.SetSourceModule (0, baseContinentDef_mi);
    baseContinentDef_cl.SetBounds (-1.0, 1.0);

    // 7: [Base-continent-definition subgroup]: Caches the output value from the
    //    clamped-continent module.
    //module::Cache baseContinentDef;
    baseContinentDef.SetSourceModule (0, baseContinentDef_cl);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: continent definition (5 noise modules)
    //
    // This subgroup warps the output value from the the base-continent-
    // definition subgroup, producing more realistic terrain.
    //
    // Warping the base continent definition produces lumpier terrain with
    // cliffs and rifts.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Coarse-turbulence module]: This turbulence module warps the output
    //    value from the base-continent-definition subgroup, adding some coarse
    //    detail to it.
    //module::Turbulence continentDef_tu0;
    continentDef_tu0.SetSourceModule (0, baseContinentDef);
    continentDef_tu0.SetSeed (CUR_SEED + 10);
    continentDef_tu0.SetFrequency (CONTINENT_FREQUENCY * 15.25);
    continentDef_tu0.SetPower (CONTINENT_FREQUENCY / 113.75);
    continentDef_tu0.SetRoughness (13);

    // 2: [Intermediate-turbulence module]: This turbulence module warps the
    //    output value from the coarse-turbulence module.  This turbulence has
    //    a higher frequency, but lower power, than the coarse-turbulence
    //    module, adding some intermediate detail to it.
    //module::Turbulence continentDef_tu1;
    continentDef_tu1.SetSourceModule (0, continentDef_tu0);
    continentDef_tu1.SetSeed (CUR_SEED + 11);
    continentDef_tu1.SetFrequency (CONTINENT_FREQUENCY * 47.25);
    continentDef_tu1.SetPower (CONTINENT_FREQUENCY / 433.75);
    continentDef_tu1.SetRoughness (12);

    // 3: [Warped-base-continent-definition module]: This turbulence module
    //    warps the output value from the intermediate-turbulence module.  This
    //    turbulence has a higher frequency, but lower power, than the
    //    intermediate-turbulence module, adding some fine detail to it.
    //module::Turbulence continentDef_tu2;
    continentDef_tu2.SetSourceModule (0, continentDef_tu1);
    continentDef_tu2.SetSeed (CUR_SEED + 12);
    continentDef_tu2.SetFrequency (CONTINENT_FREQUENCY * 95.25);
    continentDef_tu2.SetPower (CONTINENT_FREQUENCY / 1019.75);
    continentDef_tu2.SetRoughness (11);

    // 4: [Select-turbulence module]: At this stage, the turbulence is applied
    //    to the entire base-continent-definition subgroup, producing some very
    //    rugged, unrealistic coastlines.  This selector module selects the
    //    output values from the (unwarped) base-continent-definition subgroup
    //    and the warped-base-continent-definition module, based on the output
    //    value from the (unwarped) base-continent-definition subgroup.  The
    //    selection boundary is near sea level and has a relatively smooth
    //    transition.  In effect, only the higher areas of the base-continent-
    //    definition subgroup become warped; the underwater and coastal areas
    //    remain unaffected.
    //module::Select continentDef_se;
    continentDef_se.SetSourceModule (0, baseContinentDef);
    continentDef_se.SetSourceModule (1, continentDef_tu2);
    continentDef_se.SetControlModule (baseContinentDef);
    continentDef_se.SetBounds (SEA_LEVEL - 0.0375, SEA_LEVEL + 1000.0375);
    continentDef_se.SetEdgeFalloff (0.0625);

    // 7: [Continent-definition group]: Caches the output value from the
    //    clamped-continent module.  This is the output value for the entire
    //    continent-definition group.
    //module::Cache continentDef;
    continentDef.SetSourceModule (0, continentDef_se);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: terrain type definition
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: terrain type definition (3 noise modules)
    //
    // This subgroup defines the positions of the terrain types on the planet.
    //
    // Terrain types include, in order of increasing roughness, plains, hills,
    // and mountains.
    //
    // This subgroup's output value is based on the output value from the
    // continent-definition group.  Rougher terrain mainly appears at higher
    // elevations.
    //
    // -1.0 represents the smoothest terrain types (plains and underwater) and
    // +1.0 represents the roughest terrain types (mountains).
    //

    // 1: [Warped-continent module]: This turbulence module slightly warps the
    //    output value from the continent-definition group.  This prevents the
    //    rougher terrain from appearing exclusively at higher elevations.
    //    Rough areas may now appear in the the ocean, creating rocky islands
    //    and fjords.
    //module::Turbulence terrainTypeDef_tu;
    terrainTypeDef_tu.SetSourceModule (0, continentDef);
    terrainTypeDef_tu.SetSeed (CUR_SEED + 20);
    terrainTypeDef_tu.SetFrequency (CONTINENT_FREQUENCY * 18.125);
    terrainTypeDef_tu.SetPower (CONTINENT_FREQUENCY / 20.59375
      * TERRAIN_OFFSET);
    terrainTypeDef_tu.SetRoughness (3);

    // 2: [Roughness-probability-shift module]: This terracing module sharpens
    //    the edges of the warped-continent module near sea level and lowers
    //    the slope towards the higher-elevation areas.  This shrinks the areas
    //    in which the rough terrain appears, increasing the "rarity" of rough
    //    terrain.
    //module::Terrace terrainTypeDef_te;
    terrainTypeDef_te.SetSourceModule (0, terrainTypeDef_tu);
    terrainTypeDef_te.AddControlPoint (-1.00);
    terrainTypeDef_te.AddControlPoint (SHELF_LEVEL + SEA_LEVEL / 2.0);
    terrainTypeDef_te.AddControlPoint (1.00);

    // 3: [Terrain-type-definition group]: Caches the output value from the
    //    roughness-probability-shift module.  This is the output value for
    //    the entire terrain-type-definition group.
    //module::Cache terrainTypeDef;
    terrainTypeDef.SetSourceModule (0, terrainTypeDef_te);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: mountainous terrain
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: mountain base definition (9 noise modules)
    //
    // This subgroup generates the base-mountain elevations.  Other subgroups
    // will add the ridges and low areas to the base elevations.
    //
    // -1.0 represents low mountainous terrain and +1.0 represents high
    // mountainous terrain.
    //

    // 1: [Mountain-ridge module]: This ridged-multifractal-noise module
    //    generates the mountain ridges.
    //module::RidgedMulti mountainBaseDef_rm0;
    mountainBaseDef_rm0.SetSeed (CUR_SEED + 30);
    mountainBaseDef_rm0.SetFrequency (1723.0);
    mountainBaseDef_rm0.SetLacunarity (MOUNTAIN_LACUNARITY);
    mountainBaseDef_rm0.SetOctaveCount (4);
    mountainBaseDef_rm0.SetNoiseQuality (QUALITY_STD);

    // 2: [Scaled-mountain-ridge module]: Next, a scale/bias module scales the
    //    output value from the mountain-ridge module so that its ridges are not
    //    too high.  The reason for this is that another subgroup adds actual
    //    mountainous terrain to these ridges.
    //module::ScaleBias mountainBaseDef_sb0;
    mountainBaseDef_sb0.SetSourceModule (0, mountainBaseDef_rm0);
    mountainBaseDef_sb0.SetScale (0.5);
    mountainBaseDef_sb0.SetBias (0.375);

    // 3: [River-valley module]: This ridged-multifractal-noise module generates
    //    the river valleys.  It has a much lower frequency than the mountain-
    //    ridge module so that more mountain ridges will appear outside of the
    //    valleys.  Note that this noise module generates ridged-multifractal
    //    noise using only one octave; this information will be important in the
    //    next step.
    //module::RidgedMulti mountainBaseDef_rm1;
    mountainBaseDef_rm1.SetSeed (CUR_SEED + 31);
    mountainBaseDef_rm1.SetFrequency (367.0);
    mountainBaseDef_rm1.SetLacunarity (MOUNTAIN_LACUNARITY);
    mountainBaseDef_rm1.SetOctaveCount (1);
    mountainBaseDef_rm1.SetNoiseQuality (QUALITY_BEST);

    // 4: [Scaled-river-valley module]: Next, a scale/bias module applies a
    //    scaling factor of -2.0 to the output value from the river-valley
    //    module.  This stretches the possible elevation values because one-
    //    octave ridged-multifractal noise has a lower range of output values
    //    than multiple-octave ridged-multifractal noise.  The negative scaling
    //    factor inverts the range of the output value, turning the ridges from
    //    the river-valley module into valleys.
    //module::ScaleBias mountainBaseDef_sb1;
    mountainBaseDef_sb1.SetSourceModule (0, mountainBaseDef_rm1);
    mountainBaseDef_sb1.SetScale (-2.0);
    mountainBaseDef_sb1.SetBias (-0.5);

    // 5: [Low-flat module]: This low constant value is used by step 6.
    //module::Const mountainBaseDef_co;
    mountainBaseDef_co.SetConstValue (-1.0);

    // 6: [Mountains-and-valleys module]: This blender module merges the
    //    scaled-mountain-ridge module and the scaled-river-valley module
    //    together.  It causes the low-lying areas of the terrain to become
    //    smooth, and causes the high-lying areas of the terrain to contain
    //    ridges.  To do this, it uses the scaled-river-valley module as the
    //    control module, causing the low-flat module to appear in the lower
    //    areas and causing the scaled-mountain-ridge module to appear in the
    //    higher areas.
    //module::Blend mountainBaseDef_bl;
    mountainBaseDef_bl.SetSourceModule (0, mountainBaseDef_co);
    mountainBaseDef_bl.SetSourceModule (1, mountainBaseDef_sb0);
    mountainBaseDef_bl.SetControlModule (mountainBaseDef_sb1);

    // 7: [Coarse-turbulence module]: This turbulence module warps the output
    //    value from the mountain-and-valleys module, adding some coarse detail
    //    to it.
    //module::Turbulence mountainBaseDef_tu0;
    mountainBaseDef_tu0.SetSourceModule (0, mountainBaseDef_bl);
    mountainBaseDef_tu0.SetSeed (CUR_SEED + 32);
    mountainBaseDef_tu0.SetFrequency (1337.0);
    mountainBaseDef_tu0.SetPower (1.0 / 6730.0 * MOUNTAINS_TWIST);
    mountainBaseDef_tu0.SetRoughness (4);

    // 8: [Warped-mountains-and-valleys module]: This turbulence module warps
    //    the output value from the coarse-turbulence module.  This turbulence
    //    has a higher frequency, but lower power, than the coarse-turbulence
    //    module, adding some fine detail to it.
    //module::Turbulence mountainBaseDef_tu1;
    mountainBaseDef_tu1.SetSourceModule (0, mountainBaseDef_tu0);
    mountainBaseDef_tu1.SetSeed (CUR_SEED + 33);
    mountainBaseDef_tu1.SetFrequency (21221.0);
    mountainBaseDef_tu1.SetPower (1.0 / 120157.0 * MOUNTAINS_TWIST);
    mountainBaseDef_tu1.SetRoughness (6);

    // 9: [Mountain-base-definition subgroup]: Caches the output value from the
    //    warped-mountains-and-valleys module.
    //module::Cache mountainBaseDef;
    mountainBaseDef.SetSourceModule (0, mountainBaseDef_tu1);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: high mountainous terrain (5 noise modules)
    //
    // This subgroup generates the mountainous terrain that appears at high
    // elevations within the mountain ridges.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Mountain-basis-0 module]: This ridged-multifractal-noise module,
    //    along with the mountain-basis-1 module, generates the individual
    //    mountains.
    //module::RidgedMulti mountainousHigh_rm0;
    mountainousHigh_rm0.SetSeed (CUR_SEED + 40);
    mountainousHigh_rm0.SetFrequency (2371.0);
    mountainousHigh_rm0.SetLacunarity (MOUNTAIN_LACUNARITY);
    mountainousHigh_rm0.SetOctaveCount (3);
    mountainousHigh_rm0.SetNoiseQuality (QUALITY_BEST);

    // 2: [Mountain-basis-1 module]: This ridged-multifractal-noise module,
    //    along with the mountain-basis-0 module, generates the individual
    //    mountains.
    //module::RidgedMulti mountainousHigh_rm1;
    mountainousHigh_rm1.SetSeed (CUR_SEED + 41);
    mountainousHigh_rm1.SetFrequency (2341.0);
    mountainousHigh_rm1.SetLacunarity (MOUNTAIN_LACUNARITY);
    mountainousHigh_rm1.SetOctaveCount (3);
    mountainousHigh_rm1.SetNoiseQuality (QUALITY_BEST);

    // 3: [High-mountains module]: Next, a maximum-value module causes more
    //    mountains to appear at the expense of valleys.  It does this by
    //    ensuring that only the maximum of the output values from the two
    //    ridged-multifractal-noise modules contribute to the output value of
    //    this subgroup.
    //module::Max mountainousHigh_ma;
    mountainousHigh_ma.SetSourceModule (0, mountainousHigh_rm0);
    mountainousHigh_ma.SetSourceModule (1, mountainousHigh_rm1);

    // 4: [Warped-high-mountains module]: This turbulence module warps the
    //    output value from the high-mountains module, adding some detail to it.
    //module::Turbulence mountainousHigh_tu;
    mountainousHigh_tu.SetSourceModule (0, mountainousHigh_ma);
    mountainousHigh_tu.SetSeed (CUR_SEED + 42);
    mountainousHigh_tu.SetFrequency (31511.0);
    mountainousHigh_tu.SetPower (1.0 / 180371.0 * MOUNTAINS_TWIST);
    mountainousHigh_tu.SetRoughness (4);

    // 5: [High-mountainous-terrain subgroup]: Caches the output value from the
    //    warped-high-mountains module.
    //module::Cache mountainousHigh;
    mountainousHigh.SetSourceModule (0, mountainousHigh_tu);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: low mountainous terrain (4 noise modules)
    //
    // This subgroup generates the mountainous terrain that appears at low
    // elevations within the river valleys.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Lowland-basis-0 module]: This ridged-multifractal-noise module,
    //    along with the lowland-basis-1 module, produces the low mountainous
    //    terrain.
    //module::RidgedMulti mountainousLow_rm0;
    mountainousLow_rm0.SetSeed (CUR_SEED + 50);
    mountainousLow_rm0.SetFrequency (1381.0);
    mountainousLow_rm0.SetLacunarity (MOUNTAIN_LACUNARITY);
    mountainousLow_rm0.SetOctaveCount (8);
    mountainousLow_rm0.SetNoiseQuality (QUALITY_BEST);

    // 1: [Lowland-basis-1 module]: This ridged-multifractal-noise module,
    //    along with the lowland-basis-0 module, produces the low mountainous
    //    terrain.
    //module::RidgedMulti mountainousLow_rm1;
    mountainousLow_rm1.SetSeed (CUR_SEED + 51);
    mountainousLow_rm1.SetFrequency (1427.0);
    mountainousLow_rm1.SetLacunarity (MOUNTAIN_LACUNARITY);
    mountainousLow_rm1.SetOctaveCount (8);
    mountainousLow_rm1.SetNoiseQuality (QUALITY_BEST);

    // 3: [Low-mountainous-terrain module]: This multiplication module combines
    //    the output values from the two ridged-multifractal-noise modules.
    //    This causes the following to appear in the resulting terrain:
    //    - Cracks appear when two negative output values are multiplied
    //      together.
    //    - Flat areas appear when a positive and a negative output value are
    //      multiplied together.
    //    - Ridges appear when two positive output values are multiplied
    //      together.
    //module::Multiply mountainousLow_mu;
    mountainousLow_mu.SetSourceModule (0, mountainousLow_rm0);
    mountainousLow_mu.SetSourceModule (1, mountainousLow_rm1);

    // 4: [Low-mountainous-terrain subgroup]: Caches the output value from the
    //    low-moutainous-terrain module.
    //module::Cache mountainousLow;
    mountainousLow.SetSourceModule (0, mountainousLow_mu);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: mountainous terrain (7 noise modules)
    //
    // This subgroup generates the final mountainous terrain by combining the
    // high-mountainous-terrain subgroup with the low-mountainous-terrain
    // subgroup.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Scaled-low-mountainous-terrain module]: First, this scale/bias module
    //    scales the output value from the low-mountainous-terrain subgroup to a
    //    very low value and biases it towards -1.0.  This results in the low
    //    mountainous areas becoming more-or-less flat with little variation.
    //    This will also result in the low mountainous areas appearing at the
    //    lowest elevations in this subgroup.
    //module::ScaleBias mountainousTerrain_sb0;
    mountainousTerrain_sb0.SetSourceModule (0, mountainousLow);
    mountainousTerrain_sb0.SetScale (0.03125);
    mountainousTerrain_sb0.SetBias (-0.96875);

    // 2: [Scaled-high-mountainous-terrain module]: Next, this scale/bias module
    //    scales the output value from the high-mountainous-terrain subgroup to
    //    1/4 of its initial value and biases it so that its output value is
    //    usually positive.
    //module::ScaleBias mountainousTerrain_sb1;
    mountainousTerrain_sb1.SetSourceModule (0, mountainousHigh);
    mountainousTerrain_sb1.SetScale (0.25);
    mountainousTerrain_sb1.SetBias (0.25);

    // 3: [Added-high-mountainous-terrain module]: This addition module adds the
    //    output value from the scaled-high-mountainous-terrain module to the
    //    output value from the mountain-base-definition subgroup.  Mountains
    //    now appear all over the terrain.
    //module::Add mountainousTerrain_ad;
    mountainousTerrain_ad.SetSourceModule (0, mountainousTerrain_sb1);
    mountainousTerrain_ad.SetSourceModule (1, mountainBaseDef);

    // 4: [Combined-mountainous-terrain module]: Note that at this point, the
    //    entire terrain is covered in high mountainous terrain, even at the low
    //    elevations.  To make sure the mountains only appear at the higher
    //    elevations, this selector module causes low mountainous terrain to
    //    appear at the low elevations (within the valleys) and the high
    //    mountainous terrain to appear at the high elevations (within the
    //    ridges.)  To do this, this noise module selects the output value from
    //    the added-high-mountainous-terrain module if the output value from the
    //    mountain-base-definition subgroup is higher than a set amount.
    //    Otherwise, this noise module selects the output value from the scaled-
    //    low-mountainous-terrain module.
    //module::Select mountainousTerrain_se;
    mountainousTerrain_se.SetSourceModule (0, mountainousTerrain_sb0);
    mountainousTerrain_se.SetSourceModule (1, mountainousTerrain_ad);
    mountainousTerrain_se.SetControlModule (mountainBaseDef);
    mountainousTerrain_se.SetBounds (-0.5, 999.5);
    mountainousTerrain_se.SetEdgeFalloff (0.5);

    // 5: [Scaled-mountainous-terrain-module]: This scale/bias module slightly
    //    reduces the range of the output value from the combined-mountainous-
    //    terrain module, decreasing the heights of the mountain peaks.
    //module::ScaleBias mountainousTerrain_sb2;
    mountainousTerrain_sb2.SetSourceModule (0, mountainousTerrain_se);
    mountainousTerrain_sb2.SetScale (0.8);
    mountainousTerrain_sb2.SetBias (0.0);

    // 6: [Glaciated-mountainous-terrain-module]: This exponential-curve module
    //    applies an exponential curve to the output value from the scaled-
    //    mountainous-terrain module.  This causes the slope of the mountains to
    //    smoothly increase towards higher elevations, as if a glacier grinded
    //    out those mountains.  This exponential-curve module expects the output
    //    value to range from -1.0 to +1.0.
    //module::Exponent mountainousTerrain_ex;
    mountainousTerrain_ex.SetSourceModule (0, mountainousTerrain_sb2);
    mountainousTerrain_ex.SetExponent (MOUNTAIN_GLACIATION);

    // 7: [Mountainous-terrain group]: Caches the output value from the
    //    glaciated-mountainous-terrain module.  This is the output value for
    //    the entire mountainous-terrain group.
    //module::Cache mountainousTerrain;
    mountainousTerrain.SetSourceModule (0, mountainousTerrain_ex);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: hilly terrain
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: hilly terrain (11 noise modules)
    //
    // This subgroup generates the hilly terrain.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Hills module]: This billow-noise module generates the hills.
    //module::Billow hillyTerrain_bi;
    hillyTerrain_bi.SetSeed (CUR_SEED + 60);
    hillyTerrain_bi.SetFrequency (1663.0);
    hillyTerrain_bi.SetPersistence (0.5);
    hillyTerrain_bi.SetLacunarity (HILLS_LACUNARITY);
    hillyTerrain_bi.SetOctaveCount (6);
    hillyTerrain_bi.SetNoiseQuality (QUALITY_BEST);

    // 2: [Scaled-hills module]: Next, a scale/bias module scales the output
    //    value from the hills module so that its hilltops are not too high.
    //    The reason for this is that these hills are eventually added to the
    //    river valleys (see below.)
    //module::ScaleBias hillyTerrain_sb0;
    hillyTerrain_sb0.SetSourceModule (0, hillyTerrain_bi);
    hillyTerrain_sb0.SetScale (0.5);
    hillyTerrain_sb0.SetBias (0.5);

    // 3: [River-valley module]: This ridged-multifractal-noise module generates
    //    the river valleys.  It has a much lower frequency so that more hills
    //    will appear in between the valleys.  Note that this noise module
    //    generates ridged-multifractal noise using only one octave; this
    //    information will be important in the next step.
    //module::RidgedMulti hillyTerrain_rm;
    hillyTerrain_rm.SetSeed (CUR_SEED + 61);
    hillyTerrain_rm.SetFrequency (367.5);
    hillyTerrain_rm.SetLacunarity (HILLS_LACUNARITY);
    hillyTerrain_rm.SetNoiseQuality (QUALITY_BEST);
    hillyTerrain_rm.SetOctaveCount (1);

    // 4: [Scaled-river-valley module]: Next, a scale/bias module applies a
    //    scaling factor of -2.0 to the output value from the river-valley
    //    module.  This stretches the possible elevation values because one-
    //    octave ridged-multifractal noise has a lower range of output values
    //    than multiple-octave ridged-multifractal noise.  The negative scaling
    //    factor inverts the range of the output value, turning the ridges from
    //    the river-valley module into valleys.
    //module::ScaleBias hillyTerrain_sb1;
    hillyTerrain_sb1.SetSourceModule (0, hillyTerrain_rm);
    hillyTerrain_sb1.SetScale (-2.0);
    hillyTerrain_sb1.SetBias (-0.5);

    // 5: [Low-flat module]: This low constant value is used by step 6.
    //module::Const hillyTerrain_co;
    hillyTerrain_co.SetConstValue (-1.0);

    // 6: [Mountains-and-valleys module]: This blender module merges the
    //    scaled-hills module and the scaled-river-valley module together.  It
    //    causes the low-lying areas of the terrain to become smooth, and causes
    //    the high-lying areas of the terrain to contain hills.  To do this, it
    //    uses the scaled-hills module as the control module, causing the low-
    //    flat module to appear in the lower areas and causing the scaled-river-
    //    valley module to appear in the higher areas.
    //module::Blend hillyTerrain_bl;
    hillyTerrain_bl.SetSourceModule (0, hillyTerrain_co);
    hillyTerrain_bl.SetSourceModule (1, hillyTerrain_sb1);
    hillyTerrain_bl.SetControlModule (hillyTerrain_sb0);

    // 7: [Scaled-hills-and-valleys module]: This scale/bias module slightly
    //    reduces the range of the output value from the hills-and-valleys
    //    module, decreasing the heights of the hilltops.
    //module::ScaleBias hillyTerrain_sb2;
    hillyTerrain_sb2.SetSourceModule (0, hillyTerrain_bl);
    hillyTerrain_sb2.SetScale (0.75);
    hillyTerrain_sb2.SetBias (-0.25);

    // 8: [Increased-slope-hilly-terrain module]: To increase the hill slopes at
    //    higher elevations, this exponential-curve module applies an
    //    exponential curve to the output value the scaled-hills-and-valleys
    //    module.  This exponential-curve module expects the input value to
    //    range from -1.0 to 1.0.
    //module::Exponent hillyTerrain_ex;
    hillyTerrain_ex.SetSourceModule (0, hillyTerrain_sb2);
    hillyTerrain_ex.SetExponent (1.375);

    // 9: [Coarse-turbulence module]: This turbulence module warps the output
    //    value from the increased-slope-hilly-terrain module, adding some
    //    coarse detail to it.
    //module::Turbulence hillyTerrain_tu0;
    hillyTerrain_tu0.SetSourceModule (0, hillyTerrain_ex);
    hillyTerrain_tu0.SetSeed (CUR_SEED + 62);
    hillyTerrain_tu0.SetFrequency (1531.0);
    hillyTerrain_tu0.SetPower (1.0 / 16921.0 * HILLS_TWIST);
    hillyTerrain_tu0.SetRoughness (4);

    // 10: [Warped-hilly-terrain module]: This turbulence module warps the
    //     output value from the coarse-turbulence module.  This turbulence has
    //     a higher frequency, but lower power, than the coarse-turbulence
    //     module, adding some fine detail to it.
    //module::Turbulence hillyTerrain_tu1;
    hillyTerrain_tu1.SetSourceModule (0, hillyTerrain_tu0);
    hillyTerrain_tu1.SetSeed (CUR_SEED + 63);
    hillyTerrain_tu1.SetFrequency (21617.0);
    hillyTerrain_tu1.SetPower (1.0 / 117529.0 * HILLS_TWIST);
    hillyTerrain_tu1.SetRoughness (6);

    // 11: [Hilly-terrain group]: Caches the output value from the warped-hilly-
    //     terrain module.  This is the output value for the entire hilly-
    //     terrain group.
    //module::Cache hillyTerrain;
    hillyTerrain.SetSourceModule (0, hillyTerrain_tu1);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: plains terrain
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: plains terrain (7 noise modules)
    //
    // This subgroup generates the plains terrain.
    //
    // Because this subgroup will eventually be flattened considerably, the
    // types and combinations of noise modules that generate the plains are not
    // really that important; they only need to "look" interesting.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Plains-basis-0 module]: This billow-noise module, along with the
    //    plains-basis-1 module, produces the plains.
    //module::Billow plainsTerrain_bi0;
    plainsTerrain_bi0.SetSeed (CUR_SEED + 70);
    plainsTerrain_bi0.SetFrequency (1097.5);
    plainsTerrain_bi0.SetPersistence (0.5);
    plainsTerrain_bi0.SetLacunarity (PLAINS_LACUNARITY);
    plainsTerrain_bi0.SetOctaveCount (8);
    plainsTerrain_bi0.SetNoiseQuality (QUALITY_BEST);

    // 2: [Positive-plains-basis-0 module]: This scale/bias module makes the
    //    output value from the plains-basis-0 module positive since this output
    //    value will be multiplied together with the positive-plains-basis-1
    //    module.
    //module::ScaleBias plainsTerrain_sb0;
    plainsTerrain_sb0.SetSourceModule (0, plainsTerrain_bi0);
    plainsTerrain_sb0.SetScale (0.5);
    plainsTerrain_sb0.SetBias (0.5);

    // 3: [Plains-basis-1 module]: This billow-noise module, along with the
    //    plains-basis-2 module, produces the plains.
    //module::Billow plainsTerrain_bi1;
    plainsTerrain_bi1.SetSeed (CUR_SEED + 71);
    plainsTerrain_bi1.SetFrequency (1319.5);
    plainsTerrain_bi1.SetPersistence (0.5);
    plainsTerrain_bi1.SetLacunarity (PLAINS_LACUNARITY);
    plainsTerrain_bi1.SetOctaveCount (8);
    plainsTerrain_bi1.SetNoiseQuality (QUALITY_BEST);

    // 4: [Positive-plains-basis-1 module]: This scale/bias module makes the
    //    output value from the plains-basis-1 module positive since this output
    //    value will be multiplied together with the positive-plains-basis-0
    //    module.
    //module::ScaleBias plainsTerrain_sb1;
    plainsTerrain_sb1.SetSourceModule (0, plainsTerrain_bi1);
    plainsTerrain_sb1.SetScale (0.5);
    plainsTerrain_sb1.SetBias (0.5);

    // 5: [Combined-plains-basis module]: This multiplication module combines
    //    the two plains basis modules together.
    //module::Multiply plainsTerrain_mu;
    plainsTerrain_mu.SetSourceModule (0, plainsTerrain_sb0);
    plainsTerrain_mu.SetSourceModule (1, plainsTerrain_sb1);

    // 6: [Rescaled-plains-basis module]: This scale/bias module maps the output
    //    value that ranges from 0.0 to 1.0 back to a value that ranges from
    //    -1.0 to +1.0.
    //module::ScaleBias plainsTerrain_sb2;
    plainsTerrain_sb2.SetSourceModule (0, plainsTerrain_mu);
    plainsTerrain_sb2.SetScale (2.0);
    plainsTerrain_sb2.SetBias (-1.0);

    // 7: [Plains-terrain group]: Caches the output value from the rescaled-
    //    plains-basis module.  This is the output value for the entire plains-
    //    terrain group.
    //module::Cache plainsTerrain;
    plainsTerrain.SetSourceModule (0, plainsTerrain_sb2);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: badlands terrain
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: badlands sand (6 noise modules)
    //
    // This subgroup generates the sandy terrain for the badlands.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Sand-dunes module]: This ridged-multifractal-noise module generates
    //    sand dunes.  This ridged-multifractal noise is generated with a single
    //    octave, which makes very smooth dunes.
    //module::RidgedMulti badlandsSand_rm;
    badlandsSand_rm.SetSeed (CUR_SEED + 80);
    badlandsSand_rm.SetFrequency (6163.5);
    badlandsSand_rm.SetLacunarity (BADLANDS_LACUNARITY);
    badlandsSand_rm.SetNoiseQuality (QUALITY_BEST);
    badlandsSand_rm.SetOctaveCount (1);

    // 2: [Scaled-sand-dunes module]: This scale/bias module shrinks the dune
    //    heights by a small amount.  This is necessary so that the subsequent
    //    noise modules in this subgroup can add some detail to the dunes.
    //module::ScaleBias badlandsSand_sb0;
    badlandsSand_sb0.SetSourceModule (0, badlandsSand_rm);
    badlandsSand_sb0.SetScale (0.875);
    badlandsSand_sb0.SetBias (0.0);

    // 3: [Dune-detail module]: This noise module uses Voronoi polygons to
    //    generate the detail to add to the dunes.  By enabling the distance
    //    algorithm, small polygonal pits are generated; the edges of the pits
    //    are joined to the edges of nearby pits.
    //module::Voronoi badlandsSand_vo;
    badlandsSand_vo.SetSeed (CUR_SEED + 81);
    badlandsSand_vo.SetFrequency (16183.25);
    badlandsSand_vo.SetDisplacement (0.0);
    badlandsSand_vo.EnableDistance ();

    // 4: [Scaled-dune-detail module]: This scale/bias module shrinks the dune
    //    details by a large amount.  This is necessary so that the subsequent
    //    noise modules in this subgroup can add this detail to the sand-dunes
    //    module.
    //module::ScaleBias badlandsSand_sb1;
    badlandsSand_sb1.SetSourceModule (0, badlandsSand_vo);
    badlandsSand_sb1.SetScale (0.25);
    badlandsSand_sb1.SetBias (0.25);

    // 5: [Dunes-with-detail module]: This addition module combines the scaled-
    //    sand-dunes module with the scaled-dune-detail module.
    //module::Add badlandsSand_ad;
    badlandsSand_ad.SetSourceModule (0, badlandsSand_sb0);
    badlandsSand_ad.SetSourceModule (1, badlandsSand_sb1);

    // 6: [Badlands-sand subgroup]: Caches the output value from the dunes-with-
    //    detail module.
    //module::Cache badlandsSand;
    badlandsSand.SetSourceModule (0, badlandsSand_ad);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: badlands cliffs (7 noise modules)
    //
    // This subgroup generates the cliffs for the badlands.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Cliff-basis module]: This Perlin-noise module generates some coherent
    //    noise that will be used to generate the cliffs.
    //module::Perlin badlandsCliffs_pe;
    badlandsCliffs_pe.SetSeed (CUR_SEED + 90);
    badlandsCliffs_pe.SetFrequency (CONTINENT_FREQUENCY * 839.0);
    badlandsCliffs_pe.SetPersistence (0.5);
    badlandsCliffs_pe.SetLacunarity (BADLANDS_LACUNARITY);
    badlandsCliffs_pe.SetOctaveCount (6);
    badlandsCliffs_pe.SetNoiseQuality (QUALITY_STD);

    // 2: [Cliff-shaping module]: Next, this curve module applies a curve to the
    //    output value from the cliff-basis module.  This curve is initially
    //    very shallow, but then its slope increases sharply.  At the highest
    //    elevations, the curve becomes very flat again.  This produces the
    //    stereotypical Utah-style desert cliffs.
    //module::Curve badlandsCliffs_cu;
    badlandsCliffs_cu.SetSourceModule (0, badlandsCliffs_pe);
    badlandsCliffs_cu.AddControlPoint (-2.0000, -2.0000);
    badlandsCliffs_cu.AddControlPoint (-1.0000, -1.2500);
    badlandsCliffs_cu.AddControlPoint (-0.0000, -0.7500);
    badlandsCliffs_cu.AddControlPoint ( 0.5000, -0.2500);
    badlandsCliffs_cu.AddControlPoint ( 0.6250,  0.8750);
    badlandsCliffs_cu.AddControlPoint ( 0.7500,  1.0000);
    badlandsCliffs_cu.AddControlPoint ( 2.0000,  1.2500);

    // 3: [Clamped-cliffs module]: This clamping module makes the tops of the
    //    cliffs very flat by clamping the output value from the cliff-shaping
    //    module so that the tops of the cliffs are very flat.
    //module::Clamp badlandsCliffs_cl;
    badlandsCliffs_cl.SetSourceModule (0, badlandsCliffs_cu);
    badlandsCliffs_cl.SetBounds (-999.125, 0.875);

    // 4: [Terraced-cliffs module]: Next, this terracing module applies some
    //    terraces to the clamped-cliffs module in the lower elevations before
    //    the sharp cliff transition.
    //module::Terrace badlandsCliffs_te;
    badlandsCliffs_te.SetSourceModule (0, badlandsCliffs_cl);
    badlandsCliffs_te.AddControlPoint (-1.0000);
    badlandsCliffs_te.AddControlPoint (-0.8750);
    badlandsCliffs_te.AddControlPoint (-0.7500);
    badlandsCliffs_te.AddControlPoint (-0.5000);
    badlandsCliffs_te.AddControlPoint ( 0.0000);
    badlandsCliffs_te.AddControlPoint ( 1.0000);

    // 5: [Coarse-turbulence module]: This turbulence module warps the output
    //    value from the terraced-cliffs module, adding some coarse detail to
    //    it.
    //module::Turbulence badlandsCliffs_tu0;
    badlandsCliffs_tu0.SetSeed (CUR_SEED + 91);
    badlandsCliffs_tu0.SetSourceModule (0, badlandsCliffs_te);
    badlandsCliffs_tu0.SetFrequency (16111.0);
    badlandsCliffs_tu0.SetPower (1.0 / 141539.0 * BADLANDS_TWIST);
    badlandsCliffs_tu0.SetRoughness (3);

    // 6: [Warped-cliffs module]: This turbulence module warps the output value
    //    from the coarse-turbulence module.  This turbulence has a higher
    //    frequency, but lower power, than the coarse-turbulence module, adding
    //    some fine detail to it.
    //module::Turbulence badlandsCliffs_tu1;
    badlandsCliffs_tu1.SetSeed (CUR_SEED + 92);
    badlandsCliffs_tu1.SetSourceModule (0, badlandsCliffs_tu0);
    badlandsCliffs_tu1.SetFrequency (36107.0);
    badlandsCliffs_tu1.SetPower (1.0 / 211543.0 * BADLANDS_TWIST);
    badlandsCliffs_tu1.SetRoughness (3);

    // 7: [Badlands-cliffs subgroup]: Caches the output value from the warped-
    //    cliffs module.
    //module::Cache badlandsCliffs;
    badlandsCliffs.SetSourceModule (0, badlandsCliffs_tu1);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: badlands terrain (3 noise modules)
    //
    // Generates the final badlands terrain.
    //
    // Using a scale/bias module, the badlands sand is flattened considerably,
    // then the sand elevations are lowered to around -1.0.  The maximum value
    // from the flattened sand module and the cliff module contributes to the
    // final elevation.  This causes sand to appear at the low elevations since
    // the sand is slightly higher than the cliff base.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Scaled-sand-dunes module]: This scale/bias module considerably
    //    flattens the output value from the badlands-sands subgroup and lowers
    //    this value to near -1.0.
    //module::ScaleBias badlandsTerrain_sb;
    badlandsTerrain_sb.SetSourceModule (0, badlandsSand);
    badlandsTerrain_sb.SetScale (0.25);
    badlandsTerrain_sb.SetBias (-0.75);

    // 2: [Dunes-and-cliffs module]: This maximum-value module causes the dunes
    //    to appear in the low areas and the cliffs to appear in the high areas.
    //    It does this by selecting the maximum of the output values from the
    //    scaled-sand-dunes module and the badlands-cliffs subgroup.
    //module::Max badlandsTerrain_ma;
    badlandsTerrain_ma.SetSourceModule (0, badlandsCliffs);
    badlandsTerrain_ma.SetSourceModule (1, badlandsTerrain_sb);

    // 3: [Badlands-terrain group]: Caches the output value from the dunes-and-
    //    cliffs module.  This is the output value for the entire badlands-
    //    terrain group.
    //module::Cache badlandsTerrain;
    badlandsTerrain.SetSourceModule (0, badlandsTerrain_ma);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: river positions
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: river positions (7 noise modules)
    //
    // This subgroup generates the river positions.
    //
    // -1.0 represents the lowest elevations and +1.0 represents the highest
    // elevations.
    //

    // 1: [Large-river-basis module]: This ridged-multifractal-noise module
    //    creates the large, deep rivers.
    //module::RidgedMulti riverPositions_rm0;
    riverPositions_rm0.SetSeed (CUR_SEED + 100);
    riverPositions_rm0.SetFrequency (18.75);
    riverPositions_rm0.SetLacunarity (CONTINENT_LACUNARITY);
    riverPositions_rm0.SetOctaveCount (1);
    riverPositions_rm0.SetNoiseQuality (QUALITY_BEST);

    // 2: [Large-river-curve module]: This curve module applies a curve to the
    //    output value from the large-river-basis module so that the ridges
    //    become inverted.  This creates the rivers.  This curve also compresses
    //    the edge of the rivers, producing a sharp transition from the land to
    //    the river bottom.
    //module::Curve riverPositions_cu0;
    riverPositions_cu0.SetSourceModule (0, riverPositions_rm0);
    riverPositions_cu0.AddControlPoint (-2.000,  2.000);
    riverPositions_cu0.AddControlPoint (-1.000,  1.000);
    riverPositions_cu0.AddControlPoint (-0.125,  0.875);
    riverPositions_cu0.AddControlPoint ( 0.000, -1.000);
    riverPositions_cu0.AddControlPoint ( 1.000, -1.500);
    riverPositions_cu0.AddControlPoint ( 2.000, -2.000);

    /// 3: [Small-river-basis module]: This ridged-multifractal-noise module
    //     creates the small, shallow rivers.
    //module::RidgedMulti riverPositions_rm1;
    riverPositions_rm1.SetSeed (CUR_SEED + 101);
    riverPositions_rm1.SetFrequency (43.25);
    riverPositions_rm1.SetLacunarity (CONTINENT_LACUNARITY);
    riverPositions_rm1.SetOctaveCount (1);
    riverPositions_rm1.SetNoiseQuality (QUALITY_BEST);

    // 4: [Small-river-curve module]: This curve module applies a curve to the
    //    output value from the small-river-basis module so that the ridges
    //    become inverted.  This creates the rivers.  This curve also compresses
    //    the edge of the rivers, producing a sharp transition from the land to
    //    the river bottom.
    //module::Curve riverPositions_cu1;
    riverPositions_cu1.SetSourceModule (0, riverPositions_rm1);
    riverPositions_cu1.AddControlPoint (-2.000,  2.0000);
    riverPositions_cu1.AddControlPoint (-1.000,  1.5000);
    riverPositions_cu1.AddControlPoint (-0.125,  1.4375);
    riverPositions_cu1.AddControlPoint ( 0.000,  0.5000);
    riverPositions_cu1.AddControlPoint ( 1.000,  0.2500);
    riverPositions_cu1.AddControlPoint ( 2.000,  0.0000);

    // 5: [Combined-rivers module]: This minimum-value module causes the small
    //    rivers to cut into the large rivers.  It does this by selecting the
    //    minimum output values from the large-river-curve module and the small-
    //    river-curve module.
    //module::Min riverPositions_mi;
    riverPositions_mi.SetSourceModule (0, riverPositions_cu0);
    riverPositions_mi.SetSourceModule (1, riverPositions_cu1);

    // 6: [Warped-rivers module]: This turbulence module warps the output value
    //    from the combined-rivers module, which twists the rivers.  The high
    //    roughness produces less-smooth rivers.
    //module::Turbulence riverPositions_tu;
    riverPositions_tu.SetSourceModule (0, riverPositions_mi);
    riverPositions_tu.SetSeed (CUR_SEED + 102);
    riverPositions_tu.SetFrequency (9.25);
    riverPositions_tu.SetPower (1.0 / 57.75);
    riverPositions_tu.SetRoughness (6);

    // 7: [River-positions group]: Caches the output value from the warped-
    //    rivers module.  This is the output value for the entire river-
    //    positions group.
    //module::Cache riverPositions;
    riverPositions.SetSourceModule (0, riverPositions_tu);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: scaled mountainous terrain
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: scaled mountainous terrain (6 noise modules)
    //
    // This subgroup scales the output value from the mountainous-terrain group
    // so that it can be added to the elevation defined by the continent-
    // definition group.
    //
    // This subgroup scales the output value such that it is almost always
    // positive.  This is done so that a negative elevation does not get applied
    // to the continent-definition group, preventing parts of that group from
    // having negative terrain features "stamped" into it.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Base-scaled-mountainous-terrain module]: This scale/bias module
    //    scales the output value from the mountainous-terrain group so that the
    //    output value is measured in planetary elevation units.
    //module::ScaleBias scaledMountainousTerrain_sb0;
    scaledMountainousTerrain_sb0.SetSourceModule (0, mountainousTerrain);
    scaledMountainousTerrain_sb0.SetScale (0.125);
    scaledMountainousTerrain_sb0.SetBias (0.125);

    // 2: [Base-peak-modulation module]: At this stage, most mountain peaks have
    //    roughly the same elevation.  This Perlin-noise module generates some
    //    random values that will be used by subsequent noise modules to
    //    randomly change the elevations of the mountain peaks.
    //module::Perlin scaledMountainousTerrain_pe;
    scaledMountainousTerrain_pe.SetSeed (CUR_SEED + 110);
    scaledMountainousTerrain_pe.SetFrequency (14.5);
    scaledMountainousTerrain_pe.SetPersistence (0.5);
    scaledMountainousTerrain_pe.SetLacunarity (MOUNTAIN_LACUNARITY);
    scaledMountainousTerrain_pe.SetOctaveCount (6);
    scaledMountainousTerrain_pe.SetNoiseQuality (QUALITY_STD);

    // 3: [Peak-modulation module]: This exponential-curve module applies an
    //    exponential curve to the output value from the base-peak-modulation
    //    module.  This produces a small number of high values and a much larger
    //    number of low values.  This means there will be a few peaks with much
    //    higher elevations than the majority of the peaks, making the terrain
    //    features more varied.
    //module::Exponent scaledMountainousTerrain_ex;
    scaledMountainousTerrain_ex.SetSourceModule (0, scaledMountainousTerrain_pe);
    scaledMountainousTerrain_ex.SetExponent (1.25);

    // 4: [Scaled-peak-modulation module]: This scale/bias module modifies the
    //    range of the output value from the peak-modulation module so that it
    //    can be used as the modulator for the peak-height-multiplier module.
    //    It is important that this output value is not much lower than 1.0.
    //module::ScaleBias scaledMountainousTerrain_sb1;
    scaledMountainousTerrain_sb1.SetSourceModule (0,
      scaledMountainousTerrain_ex);
    scaledMountainousTerrain_sb1.SetScale (0.25);
    scaledMountainousTerrain_sb1.SetBias (1.0);

    // 5: [Peak-height-multiplier module]: This multiplier module modulates the
    //    heights of the mountain peaks from the base-scaled-mountainous-terrain
    //    module using the output value from the scaled-peak-modulation module.
    //module::Multiply scaledMountainousTerrain_mu;
    scaledMountainousTerrain_mu.SetSourceModule (0,
      scaledMountainousTerrain_sb0);
    scaledMountainousTerrain_mu.SetSourceModule (1,
      scaledMountainousTerrain_sb1);

    // 6: [Scaled-mountainous-terrain group]: Caches the output value from the
    //    peak-height-multiplier module.  This is the output value for the
    //    entire scaled-mountainous-terrain group.
    //module::Cache scaledMountainousTerrain;
    scaledMountainousTerrain.SetSourceModule (0, scaledMountainousTerrain_mu);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: scaled hilly terrain
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: scaled hilly terrain (6 noise modules)
    //
    // This subgroup scales the output value from the hilly-terrain group so
    // that it can be added to the elevation defined by the continent-
    // definition group.  The scaling amount applied to the hills is one half of
    // the scaling amount applied to the scaled-mountainous-terrain group.
    //
    // This subgroup scales the output value such that it is almost always
    // positive.  This is done so that negative elevations are not applied to
    // the continent-definition group, preventing parts of the continent-
    // definition group from having negative terrain features "stamped" into it.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Base-scaled-hilly-terrain module]: This scale/bias module scales the
    //    output value from the hilly-terrain group so that this output value is
    //    measured in planetary elevation units
    //module::ScaleBias scaledHillyTerrain_sb0;
    scaledHillyTerrain_sb0.SetSourceModule (0, hillyTerrain);
    scaledHillyTerrain_sb0.SetScale (0.0625);
    scaledHillyTerrain_sb0.SetBias (0.0625);

    // 2: [Base-hilltop-modulation module]: At this stage, most hilltops have
    //    roughly the same elevation.  This Perlin-noise module generates some
    //    random values that will be used by subsequent noise modules to
    //    randomly change the elevations of the hilltops.
    //module::Perlin scaledHillyTerrain_pe;
    scaledHillyTerrain_pe.SetSeed (CUR_SEED + 120);
    scaledHillyTerrain_pe.SetFrequency (13.5);
    scaledHillyTerrain_pe.SetPersistence (0.5);
    scaledHillyTerrain_pe.SetLacunarity (HILLS_LACUNARITY);
    scaledHillyTerrain_pe.SetOctaveCount (6);
    scaledHillyTerrain_pe.SetNoiseQuality (QUALITY_STD);

    // 3: [Hilltop-modulation module]: This exponential-curve module applies an
    //    exponential curve to the output value from the base-hilltop-modulation
    //    module.  This produces a small number of high values and a much larger
    //    number of low values.  This means there will be a few hilltops with
    //    much higher elevations than the majority of the hilltops, making the
    //    terrain features more varied.
    //module::Exponent scaledHillyTerrain_ex;
    scaledHillyTerrain_ex.SetSourceModule (0, scaledHillyTerrain_pe);
    scaledHillyTerrain_ex.SetExponent (1.25);

    // 4: [Scaled-hilltop-modulation module]: This scale/bias module modifies
    //    the range of the output value from the hilltop-modulation module so
    //    that it can be used as the modulator for the hilltop-height-multiplier
    //    module.  It is important that this output value is not much lower than
    //    1.0.
    //module::ScaleBias scaledHillyTerrain_sb1;
    scaledHillyTerrain_sb1.SetSourceModule (0, scaledHillyTerrain_ex);
    scaledHillyTerrain_sb1.SetScale (0.5);
    scaledHillyTerrain_sb1.SetBias (1.5);

    // 5: [Hilltop-height-multiplier module]: This multiplier module modulates
    //    the heights of the hilltops from the base-scaled-hilly-terrain module
    //    using the output value from the scaled-hilltop-modulation module.
    //module::Multiply scaledHillyTerrain_mu;
    scaledHillyTerrain_mu.SetSourceModule (0, scaledHillyTerrain_sb0);
    scaledHillyTerrain_mu.SetSourceModule (1, scaledHillyTerrain_sb1);

    // 6: [Scaled-hilly-terrain group]: Caches the output value from the
    //    hilltop-height-multiplier module.  This is the output value for the
    //    entire scaled-hilly-terrain group.
    //module::Cache scaledHillyTerrain;
    scaledHillyTerrain.SetSourceModule (0, scaledHillyTerrain_mu);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: scaled plains terrain
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: scaled plains terrain (2 noise modules)
    //
    // This subgroup scales the output value from the plains-terrain group so
    // that it can be added to the elevations defined by the continent-
    // definition group.
    //
    // This subgroup scales the output value such that it is almost always
    // positive.  This is done so that negative elevations are not applied to
    // the continent-definition group, preventing parts of the continent-
    // definition group from having negative terrain features "stamped" into it.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Scaled-plains-terrain module]: This scale/bias module greatly
    //    flattens the output value from the plains terrain.  This output value
    //    is measured in planetary elevation units
    //module::ScaleBias scaledPlainsTerrain_sb;
    scaledPlainsTerrain_sb.SetSourceModule (0, plainsTerrain);
    scaledPlainsTerrain_sb.SetScale (0.00390625);
    scaledPlainsTerrain_sb.SetBias (0.0078125);

    // 2: [Scaled-plains-terrain group]: Caches the output value from the
    //    scaled-plains-terrain module.  This is the output value for the entire
    //    scaled-plains-terrain group.
    //module::Cache scaledPlainsTerrain;
    scaledPlainsTerrain.SetSourceModule (0, scaledPlainsTerrain_sb);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: scaled badlands terrain
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: scaled badlands terrain (2 noise modules)
    //
    // This subgroup scales the output value from the badlands-terrain group so
    // that it can be added to the elevations defined by the continent-
    // definition group.
    //
    // This subgroup scales the output value such that it is almost always
    // positive.  This is done so that negative elevations are not applied to
    // the continent-definition group, preventing parts of the continent-
    // definition group from having negative terrain features "stamped" into it.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Scaled-badlands-terrain module]: This scale/bias module scales the
    //    output value from the badlands-terrain group so that it is measured
    //    in planetary elevation units
    //module::ScaleBias scaledBadlandsTerrain_sb;
    scaledBadlandsTerrain_sb.SetSourceModule (0, badlandsTerrain);
    scaledBadlandsTerrain_sb.SetScale (0.0625);
    scaledBadlandsTerrain_sb.SetBias (0.0625);

    // 2: [Scaled-badlands-terrain group]: Caches the output value from the
    //    scaled-badlands-terrain module.  This is the output value for the
    //    entire scaled-badlands-terrain group.
    //module::Cache scaledBadlandsTerrain;
    scaledBadlandsTerrain.SetSourceModule (0, scaledBadlandsTerrain_sb);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: final planet
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: continental shelf (6 noise modules)
    //
    // This module subgroup creates the continental shelves.
    //
    // The output value from this module subgroup are measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Shelf-creator module]: This terracing module applies a terracing
    //    curve to the continent-definition group at the specified shelf level.
    //    This terrace becomes the continental shelf.  Note that this terracing
    //    module also places another terrace below the continental shelf near
    //    -1.0.  The bottom of this terrace is defined as the bottom of the
    //    ocean; subsequent noise modules will later add oceanic trenches to the
    //    bottom of the ocean.
    //module::Terrace continentalShelf_te;
    continentalShelf_te.SetSourceModule (0, continentDef);
    continentalShelf_te.AddControlPoint (-1.0);
    continentalShelf_te.AddControlPoint (-0.75);
    continentalShelf_te.AddControlPoint (SHELF_LEVEL);
    continentalShelf_te.AddControlPoint (1.0);

    // 2: [Oceanic-trench-basis module]: This ridged-multifractal-noise module
    //    generates some coherent noise that will be used to generate the
    //    oceanic trenches.  The ridges represent the bottom of the trenches.
    //module::RidgedMulti continentalShelf_rm;
    continentalShelf_rm.SetSeed (CUR_SEED + 130);
    continentalShelf_rm.SetFrequency (CONTINENT_FREQUENCY * 4.375);
    continentalShelf_rm.SetLacunarity (CONTINENT_LACUNARITY);
    continentalShelf_rm.SetOctaveCount (16);
    continentalShelf_rm.SetNoiseQuality (QUALITY_BEST);

    // 3: [Oceanic-trench module]: This scale/bias module inverts the ridges
    //    from the oceanic-trench-basis-module so that the ridges become
    //    trenches.  This noise module also reduces the depth of the trenches so
    //    that their depths are measured in planetary elevation units.
    //module::ScaleBias continentalShelf_sb;
    continentalShelf_sb.SetSourceModule (0, continentalShelf_rm);
    continentalShelf_sb.SetScale (-0.125);
    continentalShelf_sb.SetBias (-0.125);

    // 4: [Clamped-sea-bottom module]: This clamping module clamps the output
    //    value from the shelf-creator module so that its possible range is
    //    from the bottom of the ocean to sea level.  This is done because this
    //    subgroup is only concerned about the oceans.
    //module::Clamp continentalShelf_cl;
    continentalShelf_cl.SetSourceModule (0, continentalShelf_te);
    continentalShelf_cl.SetBounds (-0.75, SEA_LEVEL);

    // 5: [Shelf-and-trenches module]: This addition module adds the oceanic
    //    trenches to the clamped-sea-bottom module.
    //module::Add continentalShelf_ad;
    continentalShelf_ad.SetSourceModule (0, continentalShelf_sb);
    continentalShelf_ad.SetSourceModule (1, continentalShelf_cl);

    // 6: [Continental-shelf subgroup]: Caches the output value from the shelf-
    //    and-trenches module.
    //module::Cache continentalShelf;
    continentalShelf.SetSourceModule (0, continentalShelf_ad);


    ////////////////////////////////////////////////////////////////////////////
    // Module group: base continent elevations (3 noise modules)
    //
    // This subgroup generates the base elevations for the continents, before
    // terrain features are added.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Base-scaled-continent-elevations module]: This scale/bias module
    //    scales the output value from the continent-definition group so that it
    //    is measured in planetary elevation units
    //module::ScaleBias baseContinentElev_sb;
    baseContinentElev_sb.SetSourceModule (0, continentDef);
    baseContinentElev_sb.SetScale (CONTINENT_HEIGHT_SCALE);
    baseContinentElev_sb.SetBias (0.0);

    // 2: [Base-continent-with-oceans module]: This selector module applies the
    //    elevations of the continental shelves to the base elevations of the
    //    continent.  It does this by selecting the output value from the
    //    continental-shelf subgroup if the corresponding output value from the
    //    continent-definition group is below the shelf level.  Otherwise, it
    //    selects the output value from the base-scaled-continent-elevations
    //    module.
    //module::Select baseContinentElev_se;
    baseContinentElev_se.SetSourceModule (0, baseContinentElev_sb);
    baseContinentElev_se.SetSourceModule (1, continentalShelf);
    baseContinentElev_se.SetControlModule (continentDef);
    baseContinentElev_se.SetBounds (SHELF_LEVEL - 1000.0, SHELF_LEVEL);
    baseContinentElev_se.SetEdgeFalloff (0.03125);

    // 3: [Base-continent-elevation subgroup]: Caches the output value from the
    //    base-continent-with-oceans module.
    //module::Cache baseContinentElev;
    baseContinentElev.SetSourceModule (0, baseContinentElev_se);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: continents with plains (2 noise modules)
    //
    // This subgroup applies the scaled-plains-terrain group to the base-
    // continent-elevation subgroup.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Continents-with-plains module]:  This addition module adds the
    //    scaled-plains-terrain group to the base-continent-elevation subgroup.
    //module::Add continentsWithPlains_ad;
    continentsWithPlains_ad.SetSourceModule (0, baseContinentElev);
    continentsWithPlains_ad.SetSourceModule (1, scaledPlainsTerrain);

    // 2: [Continents-with-plains subgroup]: Caches the output value from the
    //    continents-with-plains module.
    //module::Cache continentsWithPlains;
    continentsWithPlains.SetSourceModule (0, continentsWithPlains_ad);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: continents with hills (3 noise modules)
    //
    // This subgroup applies the scaled-hilly-terrain group to the continents-
    // with-plains subgroup.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Continents-with-hills module]:  This addition module adds the scaled-
    //    hilly-terrain group to the base-continent-elevation subgroup.
    //module::Add continentsWithHills_ad;
    continentsWithHills_ad.SetSourceModule (0, baseContinentElev);
    continentsWithHills_ad.SetSourceModule (1, scaledHillyTerrain);

    // 2: [Select-high-elevations module]: This selector module ensures that
    //    the hills only appear at higher elevations.  It does this by selecting
    //    the output value from the continent-with-hills module if the
    //    corresponding output value from the terrain-type-defintion group is
    //    above a certain value. Otherwise, it selects the output value from the
    //    continents-with-plains subgroup.
    //module::Select continentsWithHills_se;
    continentsWithHills_se.SetSourceModule (0, continentsWithPlains);
    continentsWithHills_se.SetSourceModule (1, continentsWithHills_ad);
    continentsWithHills_se.SetControlModule (terrainTypeDef);
    continentsWithHills_se.SetBounds (1.0 - HILLS_AMOUNT, 1001.0 - HILLS_AMOUNT);
    continentsWithHills_se.SetEdgeFalloff (0.25);

    // 3: [Continents-with-hills subgroup]: Caches the output value from the
    //    select-high-elevations module.
    //module::Cache continentsWithHills;
    continentsWithHills.SetSourceModule (0, continentsWithHills_se);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: continents with mountains (5 noise modules)
    //
    // This subgroup applies the scaled-mountainous-terrain group to the
    // continents-with-hills subgroup.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Continents-and-mountains module]:  This addition module adds the
    //    scaled-mountainous-terrain group to the base-continent-elevation
    //    subgroup.
    //module::Add continentsWithMountains_ad0;
    continentsWithMountains_ad0.SetSourceModule (0, baseContinentElev);
    continentsWithMountains_ad0.SetSourceModule (1, scaledMountainousTerrain);

    // 2: [Increase-mountain-heights module]:  This curve module applies a curve
    //    to the output value from the continent-definition group.  This
    //    modified output value is used by a subsequent noise module to add
    //    additional height to the mountains based on the current continent
    //    elevation.  The higher the continent elevation, the higher the
    //    mountains.
    //module::Curve continentsWithMountains_cu;
    continentsWithMountains_cu.SetSourceModule (0, continentDef);
    continentsWithMountains_cu.AddControlPoint (                  -1.0, -0.0625);
    continentsWithMountains_cu.AddControlPoint (                   0.0,  0.0000);
    continentsWithMountains_cu.AddControlPoint (1.0 - MOUNTAINS_AMOUNT,  0.0625);
    continentsWithMountains_cu.AddControlPoint (                   1.0,  0.2500);

    // 3: [Add-increased-mountain-heights module]: This addition module adds
    //    the increased-mountain-heights module to the continents-and-
    //    mountains module.  The highest continent elevations now have the
    //    highest mountains.
    //module::Add continentsWithMountains_ad1;
    continentsWithMountains_ad1.SetSourceModule (0, continentsWithMountains_ad0);
    continentsWithMountains_ad1.SetSourceModule (1, continentsWithMountains_cu);

    // 4: [Select-high-elevations module]: This selector module ensures that
    //    mountains only appear at higher elevations.  It does this by selecting
    //    the output value from the continent-with-mountains module if the
    //    corresponding output value from the terrain-type-defintion group is
    //    above a certain value.  Otherwise, it selects the output value from
    //    the continents-with-hills subgroup.  Note that the continents-with-
    //    hills subgroup also contains the plains terrain.
    //module::Select continentsWithMountains_se;
    continentsWithMountains_se.SetSourceModule (0, continentsWithHills);
    continentsWithMountains_se.SetSourceModule (1, continentsWithMountains_ad1);
    continentsWithMountains_se.SetControlModule (terrainTypeDef);
    continentsWithMountains_se.SetBounds (1.0 - MOUNTAINS_AMOUNT,
      1001.0 - MOUNTAINS_AMOUNT);
    continentsWithMountains_se.SetEdgeFalloff (0.25);

    // 5: [Continents-with-mountains subgroup]: Caches the output value from
    //    the select-high-elevations module.
    //module::Cache continentsWithMountains;
    continentsWithMountains.SetSourceModule (0, continentsWithMountains_se);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: continents with badlands (5 noise modules)
    //
    // This subgroup applies the scaled-badlands-terrain group to the
    // continents-with-mountains subgroup.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Badlands-positions module]: This Perlin-noise module generates some
    //    random noise, which is used by subsequent noise modules to specify the
    //    locations of the badlands.
    //module::Perlin continentsWithBadlands_pe;
    continentsWithBadlands_pe.SetSeed (CUR_SEED + 140);
    continentsWithBadlands_pe.SetFrequency (16.5);
    continentsWithBadlands_pe.SetPersistence (0.5);
    continentsWithBadlands_pe.SetLacunarity (CONTINENT_LACUNARITY);
    continentsWithBadlands_pe.SetOctaveCount (2);
    continentsWithBadlands_pe.SetNoiseQuality (QUALITY_STD);

    // 2: [Continents-and-badlands module]:  This addition module adds the
    //    scaled-badlands-terrain group to the base-continent-elevation
    //    subgroup.
    //module::Add continentsWithBadlands_ad;
    continentsWithBadlands_ad.SetSourceModule (0, baseContinentElev);
    continentsWithBadlands_ad.SetSourceModule (1, scaledBadlandsTerrain);

    // 3: [Select-badlands-positions module]: This selector module places
    //    badlands at random spots on the continents based on the Perlin noise
    //    generated by the badlands-positions module.  To do this, it selects
    //    the output value from the continents-and-badlands module if the
    //    corresponding output value from the badlands-position module is
    //    greater than a specified value.  Otherwise, this selector module
    //    selects the output value from the continents-with-mountains subgroup.
    //    There is also a wide transition between these two noise modules so
    //    that the badlands can blend into the rest of the terrain on the
    //    continents.
    //module::Select continentsWithBadlands_se;
    continentsWithBadlands_se.SetSourceModule (0, continentsWithMountains);
    continentsWithBadlands_se.SetSourceModule (1, continentsWithBadlands_ad);
    continentsWithBadlands_se.SetControlModule (continentsWithBadlands_pe);
    continentsWithBadlands_se.SetBounds (1.0 - BADLANDS_AMOUNT,
      1001.0 - BADLANDS_AMOUNT);
    continentsWithBadlands_se.SetEdgeFalloff (0.25);

    // 4: [Apply-badlands module]: This maximum-value module causes the badlands
    //    to "poke out" from the rest of the terrain.  It does this by ensuring
    //    that only the maximum of the output values from the continents-with-
    //    mountains subgroup and the select-badlands-positions modules
    //    contribute to the output value of this subgroup.  One side effect of
    //    this process is that the badlands will not appear in mountainous
    //    terrain.
    //module::Max continentsWithBadlands_ma;
    continentsWithBadlands_ma.SetSourceModule (0, continentsWithMountains);
    continentsWithBadlands_ma.SetSourceModule (1, continentsWithBadlands_se);

    // 5: [Continents-with-badlands subgroup]: Caches the output value from the
    //    apply-badlands module.
    //module::Cache continentsWithBadlands;
    continentsWithBadlands.SetSourceModule (0, continentsWithBadlands_ma);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: continents with rivers (4 noise modules)
    //
    // This subgroup applies the river-positions group to the continents-with-
    // badlands subgroup.
    //
    // The output value from this module subgroup is measured in planetary
    // elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
    // highest mountain peaks.)
    //

    // 1: [Scaled-rivers module]: This scale/bias module scales the output value
    //    from the river-positions group so that it is measured in planetary
    //    elevation units and is negative; this is required for step 2.
    //module::ScaleBias continentsWithRivers_sb;
    continentsWithRivers_sb.SetSourceModule (0, riverPositions);
    continentsWithRivers_sb.SetScale (RIVER_DEPTH / 2.0);
    continentsWithRivers_sb.SetBias (-RIVER_DEPTH / 2.0);

    // 2: [Add-rivers-to-continents module]: This addition module adds the
    //    rivers to the continents-with-badlands subgroup.  Because the scaled-
    //    rivers module only outputs a negative value, the scaled-rivers module
    //    carves the rivers out of the terrain.
    //module::Add continentsWithRivers_ad;
    continentsWithRivers_ad.SetSourceModule (0, continentsWithBadlands);
    continentsWithRivers_ad.SetSourceModule (1, continentsWithRivers_sb);

    // 3: [Blended-rivers-to-continents module]: This selector module outputs
    //    deep rivers near sea level and shallower rivers in higher terrain.  It
    //    does this by selecting the output value from the continents-with-
    //    badlands subgroup if the corresponding output value from the
    //    continents-with-badlands subgroup is far from sea level.  Otherwise,
    //    this selector module selects the output value from the add-rivers-to-
    //    continents module.
    //module::Select continentsWithRivers_se;
    continentsWithRivers_se.SetSourceModule (0, continentsWithBadlands);
    continentsWithRivers_se.SetSourceModule (1, continentsWithRivers_ad);
    continentsWithRivers_se.SetControlModule (continentsWithBadlands);
    continentsWithRivers_se.SetBounds (SEA_LEVEL,
      CONTINENT_HEIGHT_SCALE + SEA_LEVEL);
    continentsWithRivers_se.SetEdgeFalloff (CONTINENT_HEIGHT_SCALE - SEA_LEVEL);

    // 4: [Continents-with-rivers subgroup]: Caches the output value from the
    //    blended-rivers-to-continents module.
    //module::Cache continentsWithRivers;
    continentsWithRivers.SetSourceModule (0, continentsWithRivers_se);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: unscaled final planet (1 noise module)
    //
    // This subgroup simply caches the output value from the continent-with-
    // rivers subgroup to contribute to the final output value.
    //

    // 1: [Unscaled-final-planet subgroup]: Caches the output value from the
    //    continent-with-rivers subgroup.
    //module::Cache unscaledFinalPlanet;
    unscaledFinalPlanet.SetSourceModule (0, continentsWithRivers);


    ////////////////////////////////////////////////////////////////////////////
    // Module subgroup: final planet (2 noise modules)
    //
    // This subgroup scales the output value from the unscaled-final-planet
    // subgroup so that it represents an elevation in meters.
    //

    // 1: [Final-planet-in-meters module]: This scale/bias module scales the
    //    output value from the unscaled-final-planet subgroup so that its
    //    output value is measured in meters.
    //module::ScaleBias finalPlanet_sb;
    finalPlanet_sb.SetSourceModule (0, unscaledFinalPlanet);
    finalPlanet_sb.SetScale ((MAX_ELEV - MIN_ELEV) / 2.0);
    finalPlanet_sb.SetBias (MIN_ELEV + ((MAX_ELEV - MIN_ELEV) / 2.0));

    // 2: [Final-planet group]: Caches the output value from the final-planet-
    //    in-meters module.  Stick a fork in it, we're done!
    //module::Cache finalPlanet;
    finalPlanet.SetSourceModule (0, finalPlanet_sb);
}

Real SphereSource::PD::dh( const Vector3 & at ) const
{
    const Real d = finalPlanet.GetValue( at.x, at.y, at.z );
    return d;
}



SphereSource::SphereSource()
{
    pd = new PD();
}

SphereSource::~SphereSource()
{
    delete pd;
}

Real SphereSource::dh( const Vector3 & at ) const
{
    const Real d = pd->dh( at );
    return d;
}


}









