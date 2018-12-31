

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

#include "OgreSprites.h"
#include <Ogre.h>
#include <OgreMesh.h>
#include <OgreHardwareBuffer.h>

#define OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE 120

namespace OgreSprites {

    OgreSprites::Rect FULL_SPRITE(-1000.0f, -1000.0f, -1000.0f, -1000.0f);
    OgreSprites::Rect FULL_SCREEN(-1000.0f, -1000.0f, -1000.0f, -1000.0f);

    //-----------------------------------------------------------------------
    SpriteHandler::SpriteHandler() : 
        _metricScreen(OSPRITE_METRIC_RELATIVE),
        _metricSprite(OSPRITE_METRIC_PIXELS)
    {
    }
    //-----------------------------------------------------------------------
    SpriteHandler::~SpriteHandler()
    {
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation)
    {
       if (!afterQueue && queueGroupId==targetQueue)
          renderBuffer();
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation)
    {
       if (afterQueue && queueGroupId==targetQueue)
          renderBuffer();
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::Init(Ogre::SceneManager* sceneMan, Ogre::Viewport* viewPort, Ogre::uint8 targetQueue, bool afterQueue)
    {
        // Save scene manager data
        SpriteHandler::sceneMan = sceneMan;
        SpriteHandler::afterQueue = afterQueue;
        SpriteHandler::targetQueue = targetQueue;

        // Ensure our hardware buffer is set to zero
        hardwareBuffer.setNull();

        // Set this object as a render queue listener with Ogre
        sceneMan->addRenderQueueListener(this);
    
        // Gather viewport info
        _vpWidth = viewPort->getActualWidth();
        _vpHeight = viewPort->getActualHeight();
        _vpHalfWidth = (int)(_vpWidth / 2);
        _vpHalfHeight = (int)(_vpHeight / 2);
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::Shutdown()
    {
        // Destroy the hardware buffer    
        if (!hardwareBuffer.isNull())
            destroyHardwareBuffer();

        // Delist our renderqueuelistener
        sceneMan->removeRenderQueueListener(this);
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::SetScreenMetric(OSMetrics metric)
    {
        _metricScreen = metric;
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::SetSpriteMetric(OSMetrics metric)
    {
        if(metric == OSPRITE_METRIC_RELATIVE || metric == OSPRITE_METRIC_PIXELS)
            _metricSprite = metric;
        else
            _metricSprite = OSPRITE_METRIC_RELATIVE;
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::SetSpriteLocation(const std::string& pathName)
    {
        spriteLocation = pathName;

        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(pathName, "FileSystem", "OgreSprites");
        Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("OgreSprites");
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::LoadSprite(const std::string& spriteName)
    {
        Ogre::TextureManager::getSingleton().load(spriteName, "OgreSprites"); 
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::DrawSprite(const std::string& spriteName, float x, float y, float alpha)
    {
        // Retrieve pointer to texture resource
        Ogre::TexturePtr texturePtr = Ogre::TextureManager::getSingleton().getByName(spriteName);

        // This is the size of the original image data (pixels)
        int iSpriteWidth = (int)texturePtr->getWidth();
        int iSpriteHeight = (int)texturePtr->getHeight();

        // Get texture handle from texture resource
        SpriteElement spriteElement;
        spriteElement.texHandle = texturePtr->getHandle();

        // Convert destination start to Pixels
        float fPixelStartX = 0;
        float fPixelStartY = 0;
        convertScreenMetrics(_metricScreen, x, y, OSPRITE_METRIC_PIXELS, fPixelStartX, fPixelStartY);
        int iPixelEndX = (int)fPixelStartX + (int)iSpriteWidth;
        int iPixelEndY = (int)fPixelStartY + (int)iSpriteHeight;

        // Convert from pixels to Ogre
        convertScreenMetrics(OSPRITE_METRIC_PIXELS, fPixelStartX, fPixelStartY, OSPRITE_METRIC_OGRE, spriteElement.x1, spriteElement.y1);
        convertScreenMetrics(OSPRITE_METRIC_PIXELS, iPixelEndX, iPixelEndY, OSPRITE_METRIC_OGRE, spriteElement.x2, spriteElement.y2);

        // We want to draw the entire sprite
        spriteElement.tx1 = 0.0f;
        spriteElement.ty1 = 0.0f;
        spriteElement.tx2 = 1.0f;
        spriteElement.ty2 = 1.0f;

        // save alpha value
        spriteElement.alpha = alpha;

        // Add this sprite to our render list
        sprites.push_back(spriteElement);
    } 
    //-----------------------------------------------------------------------
    void SpriteHandler::DrawSprite(const std::string& spriteName, float x, float y, OgreSprites::Rect& spriteRect, float alpha)
    {
        // Retrieve pointer to texture resource
        Ogre::TexturePtr texturePtr = Ogre::TextureManager::getSingleton().getByName(spriteName);

        // This is the size of the original image data (pixels)
        int iSpriteWidth = (int)texturePtr->getWidth();
        int iSpriteHeight = (int)texturePtr->getHeight();

        // Get texture handle from texture resource
        SpriteElement spriteElement;
        spriteElement.texHandle = texturePtr->getHandle();

        // Drawing size
        int iDrawingWidth = (spriteRect.x2 - spriteRect.x1);
        int iDrawingHeight = (spriteRect.y2 - spriteRect.y1);

        // Convert destination start to Pixels
        float fPixelStartX = 0;
        float fPixelStartY = 0;
        convertScreenMetrics(_metricScreen, x, y, OSPRITE_METRIC_PIXELS, fPixelStartX, fPixelStartY);
        int iPixelEndX = (int)fPixelStartX + (int)iDrawingWidth;
        int iPixelEndY = (int)fPixelStartY + (int)iDrawingHeight;

        // Convert from pixels to Ogre
        convertScreenMetrics(OSPRITE_METRIC_PIXELS, fPixelStartX, fPixelStartY, OSPRITE_METRIC_OGRE, spriteElement.x1, spriteElement.y1);
        convertScreenMetrics(OSPRITE_METRIC_PIXELS, iPixelEndX, iPixelEndY, OSPRITE_METRIC_OGRE, spriteElement.x2, spriteElement.y2);

        // We want to draw only a portion of the sprite
        spriteElement.tx1 = spriteElement.ty1 = 0.0f;
        spriteElement.tx2 = spriteElement.ty2 = 1.0f;

        if(spriteRect != FULL_SPRITE) {
            if(_metricSprite == OSPRITE_METRIC_RELATIVE) {
                spriteElement.tx1 = spriteRect.x1;
                spriteElement.ty1 = spriteRect.y1;
                spriteElement.tx2 = spriteRect.x2;
                spriteElement.ty2 = spriteRect.y2;
            }
            else if(_metricSprite == OSPRITE_METRIC_PIXELS) {

                spriteElement.tx1 = (float)(spriteRect.x1 / iSpriteWidth);
                spriteElement.ty1 = (float)(spriteRect.y1 / iSpriteHeight);
                spriteElement.tx2 = (float)(spriteRect.x2 / iSpriteWidth);
                spriteElement.ty2 = (float)(spriteRect.y2 / iSpriteHeight);
            }
        }

        // save alpha value
        spriteElement.alpha = alpha;

        // Add this sprite to our render list
        sprites.push_back(spriteElement);
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::DrawSprite(const std::string& spriteName, OgreSprites::Rect& destRect, OgreSprites::Rect& spriteRect, float alpha)
    {
        // Retrieve pointer to texture resource
        Ogre::TexturePtr texturePtr = Ogre::TextureManager::getSingleton().getByName(spriteName);

        // Get texture handle from texture resource
        SpriteElement spriteElement;
        spriteElement.texHandle = texturePtr->getHandle();

        // This is the size of the original image data (pixels)
        int iSpriteWidth = (int)texturePtr->getWidth();
        int iSpriteHeight = (int)texturePtr->getHeight();

        if(destRect != FULL_SCREEN) {
            // Convert destination start to Pixels
            float fPixelStartX = 0;
            float fPixelStartY = 0;
            convertScreenMetrics(_metricScreen, destRect.x1, destRect.y1, OSPRITE_METRIC_PIXELS, fPixelStartX, fPixelStartY);

            // Convert size to pixels
            float fPixelEndX = 0; 
            float fPixelEndY = 0;
            convertScreenMetrics(_metricScreen, destRect.x2, destRect.y2, OSPRITE_METRIC_PIXELS, fPixelEndX, fPixelEndY);

            // Convert from pixels to Ogre
            convertScreenMetrics(OSPRITE_METRIC_PIXELS, fPixelStartX, fPixelStartY, OSPRITE_METRIC_OGRE, spriteElement.x1, spriteElement.y1);
            convertScreenMetrics(OSPRITE_METRIC_PIXELS, fPixelEndX, fPixelEndY, OSPRITE_METRIC_OGRE, spriteElement.x2, spriteElement.y2);
        }
        else {
            spriteElement.x1 = -1;
            spriteElement.x2 = 1;
            spriteElement.y1 = 1;
            spriteElement.y2 = -1;
        }

        // We want to draw only a portion of the sprite
        spriteElement.tx1 = spriteElement.ty1 = 0.0f;
        spriteElement.tx2 = spriteElement.ty2 = 1.0f;

        if(spriteRect != FULL_SPRITE) {
            if(_metricSprite == OSPRITE_METRIC_RELATIVE) {
                spriteElement.tx1 = spriteRect.x1;
                spriteElement.ty1 = spriteRect.y1;
                spriteElement.tx2 = spriteRect.x2;
                spriteElement.ty2 = spriteRect.y2;
            }
            else if(_metricSprite == OSPRITE_METRIC_PIXELS) {
                spriteElement.tx1 = (float)(spriteRect.x1 / iSpriteWidth);
                spriteElement.ty1 = (float)(spriteRect.y1 / iSpriteHeight);
                spriteElement.tx2 = (float)(spriteRect.x2 / iSpriteWidth);
                spriteElement.ty2 = (float)(spriteRect.y2 / iSpriteHeight);
            }
        }

        // save alpha value
        spriteElement.alpha = alpha;

        // Add this sprite to our render list
        sprites.push_back(spriteElement);
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::renderBuffer()
    {
       Ogre::RenderSystem* rs=Ogre::Root::getSingleton().getRenderSystem();
       std::list<SpriteElement>::iterator currSpr, endSpr;

       VertexChunk thisChunk;
       std::list<VertexChunk> chunks;

       unsigned int newSize;

       newSize = (int)(sprites.size())*6;
       if (newSize<OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE)
          newSize=OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE;

       // grow hardware buffer if needed
       if (hardwareBuffer.isNull() || hardwareBuffer->getNumVertices()<newSize)
       {
          if (!hardwareBuffer.isNull())
             destroyHardwareBuffer();

          createHardwareBuffer(newSize);
       }

       // If we have no sprites this frame, bail here
       if (sprites.empty()) return;

       // write quads to the hardware buffer, and remember chunks
       float* buffer;
       float z=-1;

       buffer=(float*)hardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

       endSpr=sprites.end();
       currSpr=sprites.begin();
       thisChunk.texHandle=currSpr->texHandle;
       thisChunk.vertexCount=0;
       while (currSpr!=endSpr)
       {
           thisChunk.alpha = currSpr->alpha;

          // 1st point (left bottom)
          *buffer=currSpr->x1; buffer++;
          *buffer=currSpr->y2; buffer++;
          *buffer=z; buffer++;
          *buffer=currSpr->tx1; buffer++;
          *buffer=currSpr->ty2; buffer++;
          // 2st point (right top)
          *buffer=currSpr->x2; buffer++;
          *buffer=currSpr->y1; buffer++;
          *buffer=z; buffer++;
          *buffer=currSpr->tx2; buffer++;
          *buffer=currSpr->ty1; buffer++;
          // 3rd point (left top)
          *buffer=currSpr->x1; buffer++;
          *buffer=currSpr->y1; buffer++;
          *buffer=z; buffer++;
          *buffer=currSpr->tx1; buffer++;
          *buffer=currSpr->ty1; buffer++;

          // 4th point (left bottom)
          *buffer=currSpr->x1; buffer++;
          *buffer=currSpr->y2; buffer++;
          *buffer=z; buffer++;
          *buffer=currSpr->tx1; buffer++;
          *buffer=currSpr->ty2; buffer++;
          // 5th point (right bottom)
          *buffer=currSpr->x2; buffer++;
          *buffer=currSpr->y1; buffer++;
          *buffer=z; buffer++;
          *buffer=currSpr->tx2; buffer++;
          *buffer=currSpr->ty1; buffer++;
          // 6th point (right top)
          *buffer=currSpr->x2; buffer++;
          *buffer=currSpr->y2; buffer++;
          *buffer=z; buffer++;
          *buffer=currSpr->tx2; buffer++;
          *buffer=currSpr->ty2; buffer++;

          // remember this chunk
          thisChunk.vertexCount+=6;
          currSpr++;
          if (currSpr==endSpr || thisChunk.texHandle!=currSpr->texHandle || thisChunk.alpha != currSpr->alpha)
          {
             chunks.push_back(thisChunk);
             if (currSpr!=endSpr)
             {
                thisChunk.texHandle=currSpr->texHandle;
                thisChunk.vertexCount=0;
             }
          }
       }

       hardwareBuffer->unlock();

       // set up...
       prepareForRender();

       // do the real render!
       Ogre::TexturePtr tp;
       std::list<VertexChunk>::iterator currChunk, endChunk;

       endChunk=chunks.end();
       renderOp.vertexData->vertexStart=0;
       for (currChunk=chunks.begin(); currChunk!=endChunk; currChunk++)
       {
          renderOp.vertexData->vertexCount=currChunk->vertexCount;
          tp=Ogre::TextureManager::getSingleton().getByHandle(currChunk->texHandle);
          rs->_setTexture(0, true, tp->getName());

          Ogre::LayerBlendModeEx alphaBlendMode;
          
          alphaBlendMode.blendType=Ogre::LBT_ALPHA;
          alphaBlendMode.source1=Ogre::LBS_TEXTURE;
          alphaBlendMode.operation=Ogre::LBX_BLEND_MANUAL;
          alphaBlendMode.factor = currChunk->alpha;
          rs->_setTextureBlendMode(0, alphaBlendMode);

          rs->_render(renderOp);
          
          renderOp.vertexData->vertexStart+=currChunk->vertexCount;
       }

       // sprites go home!
       sprites.clear();
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::prepareForRender()
    {
       Ogre::LayerBlendModeEx colorBlendMode;
       Ogre::LayerBlendModeEx alphaBlendMode;
       Ogre::TextureUnitState::UVWAddressingMode uvwAddressMode;

       Ogre::RenderSystem* rs=Ogre::Root::getSingleton().getRenderSystem();

       colorBlendMode.blendType=Ogre::LBT_COLOUR;
       colorBlendMode.source1=Ogre::LBS_TEXTURE;
       colorBlendMode.operation=Ogre::LBX_SOURCE1;

       alphaBlendMode.blendType=Ogre::LBT_ALPHA;
       alphaBlendMode.source1=Ogre::LBS_TEXTURE;
       alphaBlendMode.operation=Ogre::LBX_SOURCE1;
     
       uvwAddressMode.u=Ogre::TextureUnitState::TAM_CLAMP;
       uvwAddressMode.v=Ogre::TextureUnitState::TAM_CLAMP;
       uvwAddressMode.w=Ogre::TextureUnitState::TAM_CLAMP;

       rs->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
       rs->_setViewMatrix(Ogre::Matrix4::IDENTITY);
       rs->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);
       rs->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
       rs->_setTextureCoordSet(0, 0);
       rs->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
       rs->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
       rs->_setTextureBlendMode(0, colorBlendMode);
       rs->_setTextureBlendMode(0, alphaBlendMode);
       rs->_setTextureAddressingMode(0, uvwAddressMode);
       rs->_disableTextureUnitsFrom(1);
       rs->setLightingEnabled(false);
       rs->_setFog(Ogre::FOG_NONE);
       rs->_setCullingMode(Ogre::CULL_NONE);
       rs->_setDepthBufferParams(false, false);
       rs->_setColourBufferWriteEnabled(true, true, true, false);
       rs->setShadingType(Ogre::SO_GOURAUD);
       rs->_setPolygonMode(Ogre::PM_SOLID);
       rs->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
       rs->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
       rs->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
       rs->_setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS, 0);
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::createHardwareBuffer(unsigned int size)
    {
       Ogre::VertexDeclaration* vd;

       renderOp.vertexData=new Ogre::VertexData;
       renderOp.vertexData->vertexStart=0;

       vd=renderOp.vertexData->vertexDeclaration;
       vd->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
       vd->addElement(0, Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
          Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

       hardwareBuffer=Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
          vd->getVertexSize(0),
          size,// buffer size
          Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
          false);// use shadow buffer? no

       renderOp.vertexData->vertexBufferBinding->setBinding(0, hardwareBuffer);

       renderOp.operationType=Ogre::RenderOperation::OT_TRIANGLE_LIST;
       renderOp.useIndexes=false;

    }
    //-----------------------------------------------------------------------
    void SpriteHandler::destroyHardwareBuffer()
    {
       delete renderOp.vertexData;
       renderOp.vertexData=0;
       hardwareBuffer.setNull();
    }
    //-----------------------------------------------------------------------
    void SpriteHandler::convertScreenMetrics(OSMetrics metricFrom, const float sx, const float sy, OSMetrics metricTo, float& dx, float& dy)
    {
        // trivial case
        if(metricFrom == metricTo) {
            dx = sx;
            dy = sy;
            return;
        }

        // Convert from pixels ..
        if(metricFrom == OSPRITE_METRIC_PIXELS) {
            // .. to Ogre.
            if(metricTo == OSPRITE_METRIC_OGRE) {
                dx = (sx / _vpHalfWidth) - 1;
                dy = 1 - (sy / _vpHalfHeight);
            }
            // .. to relative.
            else if(metricTo == OSPRITE_METRIC_RELATIVE) {
                dx = (sx / (float)_vpWidth);
                dy = (sy / (float)_vpHeight);
            }
        }
        // Convert from relative ..
        else if(metricFrom == OSPRITE_METRIC_RELATIVE) {
            // .. to Ogre.
            if(metricTo == OSPRITE_METRIC_OGRE) {
                dx = (sx * 2) - 1;
                dy = (sy * -2) + 1;
                return;
            }
            // .. to pixels.
            else if(metricTo == OSPRITE_METRIC_PIXELS)  {
                dx = (sx * _vpWidth);
                dy = (sy * _vpHeight);
                return;
            }
        }
        // Convert from ogre ..
        else if(metricFrom == OSPRITE_METRIC_OGRE) {
            // .. to pixels.
            if(metricTo == OSPRITE_METRIC_PIXELS) {
                float relx = (sx + 1) / 2;
                float rely = (sy - 1) / (-2);
                
                dx = (relx * _vpWidth);
                dy = (rely * _vpHeight);
                return;
            }
            // .. to relative.
            else if(metricTo == OSPRITE_METRIC_RELATIVE) {
                dx = (sx + 1) / 2;
                dy = (sy - 1) / (-2);
                return;
            }
        }

    }
    //-----------------------------------------------------------------------
}




