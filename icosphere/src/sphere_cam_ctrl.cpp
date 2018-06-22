
#include "sphere_cam_ctrl.h"
#include "manual_sphere.h"


namespace IcoHeightmap
{

class SphereCamCtrl::PD
{
public:
    Real r;
    ManualSphere * sphere;
    SceneNode    * sphereNode,
                 * camNode;

    bool fwd, bwd, left, right, up, down, fast;
    Real maxSpeed;
    Real acc;
    Real w;

    Vector3 velocity;

    PD( SceneNode * sphereNode, ManualSphere * s, Real r );
    ~PD();
};

SphereCamCtrl::PD::PD( SceneNode * sphereNode, ManualSphere * s, Real r )
{
    this->camNode = 0;
    this->sphereNode = sphereNode;
    this->sphere = s;
    this->r      = r;
    maxSpeed     = 150.0;
    acc          = 20.0;
    w            = 0.0015;

    velocity     = Vector3::ZERO;
}

SphereCamCtrl::PD::~PD()
{

}

SphereCamCtrl::SphereCamCtrl( SceneNode * sphereNode, ManualSphere * sphere, Real r )
{
    pd = new PD( sphereNode, sphere, r );
}

SphereCamCtrl::~SphereCamCtrl()
{
    delete pd;
}

void SphereCamCtrl::setSphere( ManualSphere * sphere )
{
    pd->sphere = sphere;
}

void SphereCamCtrl::setCameraNode( SceneNode * camNode )
{
    pd->camNode = camNode;
}

void SphereCamCtrl::frameRendered( const Ogre::FrameEvent & evt )
{
    if ( !pd->camNode )
        return;

    Ogre::Vector3 accel = Ogre::Vector3::ZERO;
    Ogre::Matrix3 axes = pd->camNode->getLocalAxes();
    if (pd->fwd)
        accel -= axes.GetColumn(2);
    if (pd->bwd)
        accel += axes.GetColumn(2);
    if (pd->right)
        accel += axes.GetColumn(0);
    if (pd->left)
        accel -= axes.GetColumn(0);
    if (pd->up)
        accel += axes.GetColumn(1);
    if (pd->down)
        accel -= axes.GetColumn(1);

    // if accelerating, try to reach top speed in a certain time
    Ogre::Real topSpeed = pd->fast ? pd->maxSpeed * 20 : pd->maxSpeed;
    if (accel.squaredLength() != 0)
    {
        accel.normalise();
        pd->velocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
    }
    // if not accelerating, try to stop in a certain time
    else
        pd->velocity -= pd->velocity * evt.timeSinceLastFrame * 10;

    Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

    // keep camera velocity below top speed and above epsilon
    if (pd->velocity.squaredLength() > topSpeed * topSpeed)
    {
        pd->velocity.normalise();
        pd->velocity *= pd->maxSpeed;
    }
    else if (pd->velocity.squaredLength() < tooSmall * tooSmall)
        pd->velocity = Ogre::Vector3::ZERO;

    if (pd->velocity != Ogre::Vector3::ZERO)
        pd->camNode->translate( pd->velocity * evt.timeSinceLastFrame );

    /*Vector3 at = pd->camNode->getPosition();
    // Should be normalized.
    at.normalise();
    const Vector3 localUp = at;
    const Real localHeight = pd->sphere->localHeight( at ) + 2.0;
    at *= localHeight;
    pd->camNode->setPosition( at );

    const Vector3 currentBack = axes.GetColumn( 2 );
    Vector3 currentRight = localUp.crossProduct( currentBack );
    const Real    l2 = currentRight.squaredLength();
    if ( l2 > tooSmall )
    {
        currentRight.normalise();
        Vector3 currentUp = currentBack.crossProduct( currentRight );
        axes.SetColumn( 0, currentRight );
        axes.SetColumn( 1, currentUp );
        axes.SetColumn( 2, currentUp );
        Quaternion q;
        q.FromRotationMatrix( axes );
        pd->camNode->setOrientation( q );
    }*/
}

bool SphereCamCtrl::keyPressed(const KeyboardEvent& evt)
{
    Keycode key = evt.keysym.sym;
    if (key == 'w' || key == SDLK_UP) pd->fwd = true;
    else if (key == 's' || key == SDLK_DOWN) pd->bwd = true;
    else if (key == 'a' || key == SDLK_LEFT) pd->left = true;
    else if (key == 'd' || key == SDLK_RIGHT) pd->right = true;
    else if (key == SDLK_PAGEUP) pd->up = true;
    else if (key == SDLK_PAGEDOWN) pd->down = true;
    else if (key == SDLK_LSHIFT) pd->fast = true;

    return true;
}

bool SphereCamCtrl::keyReleased(const KeyboardEvent& evt)
{
    Keycode key = evt.keysym.sym;
    if (key == 'w' || key == SDLK_UP) pd->fwd = false;
    else if (key == 's' || key == SDLK_DOWN) pd->bwd = false;
    else if (key == 'a' || key == SDLK_LEFT) pd->left = false;
    else if (key == 'd' || key == SDLK_RIGHT) pd->right = false;
    else if (key == SDLK_PAGEUP) pd->up = false;
    else if (key == SDLK_PAGEDOWN) pd->down = false;
    else if (key == SDLK_LSHIFT) pd->fast = false;

    return true;
}

bool SphereCamCtrl::mouseMoved(const MouseMotionEvent& evt)
{
    if ( !pd->camNode )
        return true;

    Matrix3 axes = pd->camNode->getLocalAxes();
    Vector3 up = pd->camNode->getPosition();
    up.normalise();
    const Real ay = - static_cast<Real>( evt.xrel ) * pd->w;
    const Real ax = - static_cast<Real>( evt.yrel ) * pd->w;
    Quaternion qy;
    qy.FromAngleAxis( Radian(ay), up );
    Quaternion qx;
    qx.FromAngleAxis( Radian(ax), axes.GetColumn( 0 ) );
    Quaternion q = qy * qx;
    Matrix3 R;
    q.ToRotationMatrix( R );
    axes = R * axes;
    q.FromRotationMatrix( axes );
    pd->camNode->setOrientation( q );

    return true;
}

bool SphereCamCtrl::mouseWheelRolled(const MouseWheelEvent& evt)
{
    return true;
}

bool SphereCamCtrl::mousePressed(const MouseButtonEvent& evt)
{
    return true;
}

bool SphereCamCtrl::mouseReleased(const MouseButtonEvent& evt)
{
    return true;
}




}



