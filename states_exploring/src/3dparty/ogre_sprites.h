

/*
    OgreSprites
    
    OgreSprites is made available under the MIT License.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

    Written by Andrew C Lytle, June 2007.

    Developed based on code by H. Hernán Moraldo from Moraldo Games
    www.hernan.moraldo.com.ar/pmenglish/field.php
*/

#ifndef __OGRE_SPRITES_H__
#define __OGRE_SPRITES_H__

#include <Ogre.h>
#include <OgreRenderQueueListener.h>
#include <string>
#include <list>

namespace OgreSprites {

    /** Holds information about a single sprite each frame.
    @remarks
        Used internally.
    */
    struct SpriteElement
    {
        float x1, y1, x2, y2;// sprite coordinates
        float tx1, ty1, tx2, ty2;// texture coordinates
        Ogre::ResourceHandle texHandle;// texture handle

        float alpha;
    };

    /** Holds vertex information. 
    @remarks
        Used internally.
    */
    struct VertexChunk {
        Ogre::ResourceHandle texHandle;
        unsigned int vertexCount;

        float alpha;
    };

    /** Available metrics.
    @remarks
        See SpriteManager notes for details.
    */
    enum OSMetrics {
        OSPRITE_METRIC_PIXELS,
        OSPRITE_METRIC_RELATIVE,
        OSPRITE_METRIC_OGRE
    };

    /** Rectangle class to represent either screen or texture space.
    */
    struct Rect 
    {
        /// Default contstructor
        Rect() 
        {
            x1 = y1 = x2 = y2 = 0.0f;
        }

        /// Copy constructor
        Rect(Rect &r) 
        {
            x1 = r.x1;
            y1 = r.y1;
            x2 = r.x2;
            y2 = r.y2;
        }

        /// Parameter constructor
        Rect(Ogre::Real px1, Ogre::Real py1, Ogre::Real px2, Ogre::Real py2) 
        {
            x1 = px1;
            y1 = py1;
            x2 = px2;
            y2 = py2;
        }

        /// Equality operator
        bool operator==(Rect &r) {
            if( (x1==r.x1) && (x2==r.x2) && (y1==r.y1) && (y2==r.y2) )
                return true;
            else 
                return false;
        }
        
        /// Inequality operator
        bool operator!=(Rect &r) {
            return !(operator==(r));
        }

        /// Left coordinate
        Ogre::Real x1;   

        /// Top coordinate
        Ogre::Real y1;   

        /// Right coordinate
        Ogre::Real x2;   

        /// Bottom coordinate
        Ogre::Real y2;   // bottom
    };

    /// Macro typedef of a Rect representing the entire texture space of a sprite.
    extern OgreSprites::Rect FULL_SPRITE;

    /// Macro typedef of a Rect representing the entire screen space.
    extern OgreSprites::Rect FULL_SCREEN;
    
    /** Controls all sprite rendering operations.
    @remarks
        Usage:

        SETUP:

            1) Instantiate the OgreSprites::SpriteHandler object as usual with a standard new,
            and leave it in a place that your program will be able to access the pointer later.
 
                OgreSprites::SpriteHandler* spriteHandler = new OgreSprites::SpriteHandler();

            2) Give the SpriteHandler a resource path, this *must* be done or you won't be able to load
              any sprites. Naturally, change the path to match where you save your sprites.

                spriteHandler->SetSpriteLocation("../sprites");

            3) Load your sprites, manually. Notice we aren't using the standard Ogre::Material concept, 
              we're just loading regular old textures.
                spriteHandler->LoadSprite("sprite.png");
                spriteHandler->LoadSprite("sprite2.png");
                spriteHandler->LoadSprite("folder/sprite3.png");

            4) Initialize the library, giving it your current Ogre scene manager and viewport.

                spriteHandler->Init(mSceneMgr, mWindow->getViewport(0));
        
        METRICS:

        - Decide on your metrics. The SpriteManager can interpret both screen values and sprite locations 
          in three different ways. If I've  made this unreasonably complicated, just stick to the defaults 
          and you'll be fine.

        - OSPRITE_METRIC_RELATIVE: This is the default screen metric. It considers the screen location from
          (0.0, 0.0) -> (1.0, 1.0) going from top left to bottom right of the screen. For sprites, the same 
          locations represent the top left to bottom right of the sprite texture. Using this metric for your 
          screen values ensures that your sprites will always been drawn in the same relative screen 
          position, regardless of the size (or resolution) of your viewport. Using it for your sprites is not 
          recommended, since your sprite data is usually static, it is often better to use
          OSPRITE_METRIC_PIXEL instead for sprites.

        - OSPRITE_METRIC_OGRE: This metric is the default for Ogre's internal drawing system, and although 
          similar to OSPRITE_METRIC_RELATIVE, it goes from (-1, 1) to (1, -1) from top left to bottom right. 
          You can not use this metric for sprites. 

        - OSPRITE_METRIC_PIXELS: This is the default sprite metric. This metric works in actual pixel 
          locations. If you use this for your screen metric, you will always be drawing in the same pixel 
          locations, regardless of screen resolution. Since this is often undesireable, it is recommended you 
          use OSPRITE_METRIC_RELATIVE for screen metrics.

        DRAWING:

        - You can draw your sprites using whichever of the various DrawSprite methods suits your needs. 
          I've forced myself to contain my code bloat and keep them to only three. There's lots of room 
          to expand on them if you want specialized drawing concepts.

        - Method 1: DrawSprite using fixed location and the entire sprite.

            DrawSprite("spriteName", xLocation, yLocation, alphaValue);

        - Method 2: DrawSprite using fixed destination size and partial sprite.

            DrawSprite("spriteName", xLocation, yLocation, OgreSprites::Rect(spriteX1, spriteY1, spriteX2, spriteY2), alphaValue);

        - Method 2: DrawSprite using arbitrary destination size and partial sprite.

            DrawSprite("spriteName", OgreSprites::Rect(destX1, destY1, destX2, destY2), OgreSprites::Rect(spriteX1, spriteY1, spriteX2, spriteY2), alphaValue);

        SHUTDOWN:

        - Call spriteHandler->Shutdown() during program shutdown, and delete the pointer normally. 
          That's it!
    */
    class SpriteHandler : public Ogre::RenderQueueListener
    {

    public: 
        /// Default constructor
        SpriteHandler();

        /// Destructor
        virtual ~SpriteHandler();

        /// Used internally by Ogre
        virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation);

        /// Used internally by Ogre
        virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation);

        /** Initialize the sprite system, and register it with Ogre.
        @remarks
            This method setups up the sprite system, and must be called only after the Ogre
            Scene Manager has been created.
            @param 
                sceneMan A pointer to the current scene manager.
            @param
                viewPort The Ogre viewport we should be rendering to.
            @param
                targetQueue The render queue that we are inserting this render operation into.
            @param
                afterQueue Should we render after this Render Queue? If not, we'll do it before.
        */
        void Init(Ogre::SceneManager* sceneMan, Ogre::Viewport* viewPort, Ogre::uint8 targetQueue = Ogre::RENDER_QUEUE_OVERLAY, bool afterQueue = true);
        
        /** Shutdown the sprite system. 
        @remarks
            This will be done automatically by deleting the object, but can be done manually
            should you desire it. 
        */
        void Shutdown(void);

        /** Control screen metrics. 
        @remarks
            The three available metrics for the screen are OSPRITE_METRIC_RELATIVE, OSPRITE_METRIC_PIXELS,
            and OSPRITE_METRIC_OGRE. For the screen, RELATIVE means top-left origin, with bottom right as
            (1,1). OGRE means (-1,1) to (1,-1).  And PIXELS means (0,0) to (screenWidth, screenHeight).
            The default value is OSPRITE_METRIC_RELATIVE. 
            @param 
                metric Which metric to use
        */
        void SetScreenMetric(OSMetrics metric);

        /** Control sprite metrics. 
        @remarks
            The available metrics for the screen are OSPRITE_METRIC_RELATIVE, and OSPRITE_METRIC_PIXELS.
            The function similar to screen metrics, with (0,0) always as the top-left of the texture.
            The default value is OSPRITE_METRIC_PIXELS. 
            @param 
                metric Which metric to use
        */
        void SetSpriteMetric(OSMetrics metric);
    
        /** Set the folder location of the sprite data.
        @remarks
            This MUST be done before any sprites can be loaded. Failure to do so will cause
            LoadSprite to fail with an exception.
            @param 
                pathName The relative or absolute path to the sprite texture files.
        */
        void SetSpriteLocation(const std::string& pathName);

        /** Load a sprite into memory.
        @remarks
            This method will load a sprite from a texture file. You must do this with
            each sprite you intend to draw later. This texture file must be found in the
            path given by SetSpriteLocation previously.
            @param 
                pathName The texture file name (with extension)
        */
        void LoadSprite(const std::string& spriteName);

        /** Render a sprite.
        @remarks
            This method will draw a sprite at a given location, with a given alpha. The entire
            texture will be used to render the sprite.
            @param 
                spriteName The file name that was loaded with LoadSprite (with extension).
            @param
                x The x coordinate to begin drawing the sprite at (relative or pixel, based on metrics)
            @param
                y The y coordinate to begin drawing the sprite at (relative or pixel, based on metrics)
            @param
                alpha The alpha value to used when drawing. 0.0 is totally transparent, 1.0 is completely solid.
        */
        void DrawSprite(const std::string& spriteName, float x, float y, float alpha);

        /** Render a sprite.
        @remarks
            This method will draw a portion of the sprite at a given location, with a given alpha. 
            The area specified in spriteRect will be be used to render the sprite.
            @param 
                spriteName The file name that was loaded with LoadSprite (with extension).
            @param
                x The x coordinate to begin drawing the sprite at (relative or pixel, based on metrics)
            @param
                y The y coordinate to begin drawing the sprite at (relative or pixel, based on metrics)
            @param
                spriteRect A rectangle representing a 2D location in the texture (relative or pixel, based on metrics)
            @param
                alpha The alpha value to used when drawing. 0.0 is totally transparent, 1.0 is completely solid.
        */
        void DrawSprite(const std::string& spriteName, float x, float y, OgreSprites::Rect& spriteRect, float alpha);

        /** Render a sprite.
        @remarks
            This method will draw a portion of the sprite at a given location and given ending point, 
            with a given alpha. The area specified in spriteRect will be be used to render the sprite.
            The area specified in destRect will be used to determine location and size of the final
            drawing operation.
            @param 
                spriteName The file name that was loaded with LoadSprite (with extension).
            @param
                x The x coordinate to begin drawing the sprite at (relative or pixel, based on metrics)
            @param
                y The y coordinate to begin drawing the sprite at (relative or pixel, based on metrics)
            @param
                spriteRect A rectangle representing a 2D location in the texture (relative or pixel, based on metrics)
            @param
                spriteRect A rectangle representing a 2D location in the texture (relative or pixel, based on metrics)
            @param
                alpha The alpha value to used when drawing. 0.0 is totally transparent, 1.0 is completely solid.
        */
        void DrawSprite(const std::string& spriteName, OgreSprites::Rect& destRect, OgreSprites::Rect& spriteRect, float alpha);

    private:

        /// Render all the 2d data stored in the hardware buffers.
        void renderBuffer();

        /// Create a new hardware buffer
        void createHardwareBuffer(unsigned int size);

        /// Destroy the hardware buffer
        void destroyHardwareBuffer();

        /// Set Ogre for rendering
        void prepareForRender();

        /// Convert metrics
        void convertScreenMetrics(OSMetrics metricFrom, const float sx, const float sy, OSMetrics metricTo, float& dx, float& dy);

        /// Ogre Specific: render operation handler
        Ogre::RenderOperation renderOp;

        /// Ogre Specific: hardware buffer
        Ogre::HardwareVertexBufferSharedPtr hardwareBuffer;

        /// Sprite Buffer
        std::list<SpriteElement> sprites;

        /// Save our sprite texture path
        std::string    spriteLocation;

        /// Scene manager reference pointer
        Ogre::SceneManager* sceneMan;

        /// Which queue we're rendering on
        Ogre::uint8 targetQueue;

        /// Render after or before this queue
        bool afterQueue;

        /// Viewport width
        int _vpWidth;

        /// Viewport height
        int _vpHalfWidth;

        /// Half viewport width, save time calculating later
        int _vpHeight;

        /// Half viewport height, save time calculating later
        int _vpHalfHeight;

        /// Current screen metrics
        OSMetrics _metricScreen;

        /// Current sprite metrics
        OSMetrics _metricSprite;

    };

}

#endif // __OGRE_SPRITES_H__




