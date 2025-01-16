/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

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
****************************************************************************/

// ideas taken from:
//     . The ocean spray in your face [Jeff Lander]
//        http://www.double.co.nz/dust/col0798.pdf
//     . Building an Advanced Particle System [John van der Burg]
//        http://www.gamasutra.com/features/20000623/vanderburg_01.htm
//   . LOVE game engine
//        http://love2d.org/
//
//
// Radius mode support, from 71 squared
//        http://particledesigner.71squared.com/
//
// IMPORTANT: Particle Designer is supported by cocos2d, but
// 'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guaranteed in cocos2d,
//  cocos2d uses a another approach, but the results are almost identical. 
//

#include "2d/CCParticleSystem.h"

#include <string>

#include "2d/CCParticleBatchNode.h"
#include "renderer/CCTextureAtlas.h"
#include "base/base64.h"
#include "base/ZipUtils.h"
#include "base/CCDirector.h"
#include "base/CCProfiling.h"
#include "base/ccUTF8.h"
#include "renderer/CCTextureCache.h"
#include "platform/CCFileUtils.h"

using namespace std;

NS_CC_BEGIN

// ideas taken from:
//     . The ocean spray in your face [Jeff Lander]
//        http://www.double.co.nz/dust/col0798.pdf
//     . Building an Advanced Particle System [John van der Burg]
//        http://www.gamasutra.com/features/20000623/vanderburg_01.htm
//   . LOVE game engine
//        http://love2d.org/
//
//
// Radius mode support, from 71 squared
//        http://particledesigner.71squared.com/
//
// IMPORTANT: Particle Designer is supported by cocos2d, but
// 'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guaranteed in cocos2d,
//  cocos2d uses a another approach, but the results are almost identical. 
//


inline void normalize_point(float x, float y, particle_point* out)
{
    float n = x * x + y * y;
    // Already normalized.
    if (n == 1.0f)
        return;
    
    n = sqrt(n);
    // Too close to zero.
    if (n < MATH_TOLERANCE)
        return;
    
    n = 1.0f / n;
    out->x = x * n;
    out->y = y * n;
}

/**
 A more effect random number getter function, get from ejoy2d.
 */
inline static float RANDOM_M11(unsigned int *seed) {
    *seed = *seed * 134775813 + 1;
    union {
        uint32_t d;                                     
        float f;
    } u;
    u.d = (((uint32_t)(*seed) & 0x7fff) << 8) | 0x40000000;
    return u.f - 3.0f;
}

// life cycle of particles
void particleLifeSystem(flecs::iter& it, size_t i, flecsLife& fl)
{
    fl.life -= it.delta_time();
    if (fl.life <= 0)
    {
        it.entity(i).destruct();
    }
}

// modeA update
void modeAUpdateSystem(flecs::iter& it, size_t i, flecsPosition& fp, flecsAccel& fa, flecsDir& fd, flecsYCoordFlipped& fycf)
{
//    particle_point tmp, radial = {0.0f, 0.0f}, tangential;

    // radial acceleration
    particle_point radial = {0.0f, 0.0f};
    if (fp.position.x || fp.position.y)
    {
        normalize_point(fp.position.x, fp.position.y, &radial);
    }
    particle_point tangential = radial;
    radial.x *= fa.radialAccel;
    radial.y *= fa.radialAccel;

    // tangential acceleration
    std::swap(tangential.x, tangential.y);
    tangential.x *= - fa.tangentialAccel;
    tangential.y *= fa.tangentialAccel;

    // (gravity + radial + tangential) * dt
    particle_point tmp;
    tmp.x = radial.x + tangential.x + fa.gravity.x;
    tmp.y = radial.y + tangential.y + fa.gravity.y;
    tmp.x *= it.delta_time();
    tmp.y *= it.delta_time();

    fd.dirX += tmp.x;
    fd.dirY += tmp.y;

    // this is cocos2d-x v3.0
    // if (_configName.length()>0 && _yCoordFlipped != -1)

    // this is cocos2d-x v3.0
    tmp.x = fd.dirX * it.delta_time() * fycf._yCoordFlipped;
    tmp.y = fd.dirY * it.delta_time() * fycf._yCoordFlipped;
    fp.position.x += tmp.x;
    fp.position.y += tmp.y;
}

// modeB update
void modeBUpdateSystem(flecs::iter& it, size_t i, flecsPosition& fp, flecsAngle& fa, flecsRadius& fr, flecsYCoordFlipped& fycf)
{
    fa.angle += fa.degreesPerSecond * it.delta_time();
    fr.radius += fr.deltaRadius * it.delta_time();
    fp.position.x = - cosf(fa.angle) * fr.radius;
    fp.position.y = - sinf(fa.angle) * fr.radius * fycf._yCoordFlipped;
}

// color, size and rotation update
void otherUpdateSystem(flecs::iter& it, size_t i, flecsColor& fc, flecsSize& fs, flecsRotation& fr)
{
    //color r,g,b,a
    fc.colorR += fc.deltaColorR * it.delta_time();
    fc.colorG += fc.deltaColorG * it.delta_time();
    fc.colorB += fc.deltaColorB * it.delta_time();
    fc.colorA += fc.deltaColorA * it.delta_time();

    //size
    fs.size += (fs.deltaSize * it.delta_time());
    fs.size = MAX(0, fs.size);

    //rotation
    fr.rotation += fr.deltaRotation * it.delta_time();
}

// render todo
//void renderSystem(flecsPosition& fp, flecsStartPosition& fsp, flecsColor& fc, flecsSize&, fs, flecsRotation& fr)
//{
//    flecsPositionType *pt = ecs_get(world, ecs_id(flecsPositionType), flecsPositionType);
//    felcsQuad *qd = ecs_get(world, ecs_id(felcsQuad), felcsQuad);
//    if (&(pt.positionType) == PositionType::FREE)
//    {
//        Vec3 p1(fsp.startPosX, fsp.startPosY, 0); // todo: startPos may not equal to currentPosition(_position)?
//        Mat4 worldToNodeTM = getWorldToNodeTransform();
//        worldToNodeTM.transformPoint(&p1);
//        Vec3 p2;
//        Vec2 newPos;
//        float startX = fsp.startPosX;
//        float startY = fsp.startPosY;
//        float x = fp.position.x;
//        float y = fp.position.y;
//        float s = fs.size;
//        float r = fr.rotation;
//    }
//    else if (&(pt.positionType) == PositionType::RELATIVE)
//    {
//    }
//    else
//    {
//    }
//}

flecs::world* FlecsParticleSystem::GetEcsWorld() const { return ECSWorld; }

void FlecsParticleSystem::initFlecs()
{
    // register systems
//	auto system_Emit = GetEcsWorld()->system
//		<flecsEmitterInfo, flecsCount>
//		("Emit System")
//		.each(emitSystem);
//
//	auto system_Spawn = GetEcsWorld()->system
//		<flecsCount, flecsSystemNotPause, flecsSpawnInfo, flecsPosition>
//		("Spawn System")
//		.each(spawnSystem);
//
//	auto system_ParticleSystemLife = GetEcsWorld()->system
//		<flecsSystemLife, flecsEmitterInfo>
//		("Particle System Life System")
//		.each(particleSystemLifeSystem);

	auto system_ParticleLife = GetEcsWorld()->system
		<flecsLife>
		("Particle Life System")
		.each(particleLifeSystem);

	auto system_ModeAUpdate = GetEcsWorld()->system
		<flecsPosition, flecsAccel, flecsDir, flecsYCoordFlipped>
		("Mode A Update System")
		.each(modeAUpdateSystem);

	auto system_ModeBUpdate = GetEcsWorld()->system
		<flecsPosition, flecsAngle, flecsRadius, flecsYCoordFlipped>
		("Mode B Update System")
		.each(modeBUpdateSystem);

	auto system_OtherUpdate = GetEcsWorld()->system
		<flecsColor, flecsSize, flecsRotation>
		("Other Update System")
		.each(otherUpdateSystem);

//	auto system_Render = GetEcsWorld()->system
//		<flecsPosition, flecsStartPosition, flecsColor, flecsSize, flecsRotation>
//		("Render System")
//		.each(renderSystem);

//	UE_LOG(LogTemp, Warning, TEXT("All System initialized!"));
}

bool FlecsParticleSystem::Tick(float DeltaTime) // todo: use Update as name?
{
	if (ECSWorld) ECSWorld->progress(DeltaTime);
	return true;
}

void FlecsParticleSystem::init()
{
//    quadPtr = quad;
//     if (!ECSWorld)
//     {
//         char name[] = { "Particle System Flecs" };
//         char* argv = name;
//         ECSWorld = new flecs::world(1, &argv);
//     }
//    ECS_COMPONENT(ECSWorld, felcsQuad);
//    ECS_COMPONENT(ECSWorld, flecsPositionType);
////    ECS_COMPONENT(ECSWorld, flecsNodePosition);
//    ecs_set(world, ecs_id(felcsQuad), felcsQuad, {&(quad)});
//    ecs_set(world, ecs_id(flecsPositionType), flecsPositionType, {&(positionType)});
////    ecs_set(world, ecs_id(flecsNodePosition), flecsNodePosition, {&(_position)});
    initFlecs();
}

void FlecsParticleSystem::release()
{
    ECSWorld->release();
    ECSWorld = nullptr;
}

FlecsParticleSystem::FlecsParticleSystem() {
        char name[] = { "Particle System Flecs" };
        char* argv = name;
        ECSWorld = new flecs::world(1, &argv);
}

FlecsParticleSystem::~FlecsParticleSystem()
{
    ECSWorld->release();
    ECSWorld = nullptr;
}

// delete ParticleData(), ParticleData::init(), ParticleData::release()

Vector<ParticleSystem*> ParticleSystem::__allInstances;
float ParticleSystem::__totalParticleCountFactor = 1.0f;

ParticleSystem::ParticleSystem()
: _isBlendAdditive(false)
, _isAutoRemoveOnFinish(false)
, _plistFile("")
, _elapsed(0)
, _configName("")
, _emitCounter(0)
, _batchNode(nullptr)
, _atlasIndex(0)
, _transformSystemDirty(false)
, _allocatedParticles(0)
, _isActive(true)
, _particleCount(0)
, _duration(0)
, _life(0)
, _lifeVar(0)
, _angle(0)
, _angleVar(0)
, _emitterMode(Mode::GRAVITY)
, _startSize(0)
, _startSizeVar(0)
, _endSize(0)
, _endSizeVar(0)
, _startSpin(0)
, _startSpinVar(0)
, _endSpin(0)
, _endSpinVar(0)
, _emissionRate(0)
, _totalParticles(0)
, _texture(nullptr)
, _blendFunc(BlendFunc::ALPHA_PREMULTIPLIED)
, _opacityModifyRGB(false)
, _yCoordFlipped(1)
, _positionType(PositionType::FREE)
, _paused(false)
//, _quad(nullptr)
, _sourcePositionCompatible(true) // In the furture this member's default value maybe false or be removed.
{
    modeA.gravity.setZero();
    modeA.speed = 0;
    modeA.speedVar = 0;
    modeA.tangentialAccel = 0;
    modeA.tangentialAccelVar = 0;
    modeA.radialAccel = 0;
    modeA.radialAccelVar = 0;
    modeA.rotationIsDir = false;
    modeB.startRadius = 0;
    modeB.startRadiusVar = 0;
    modeB.endRadius = 0;
    modeB.endRadiusVar = 0;            
    modeB.rotatePerSecond = 0;
    modeB.rotatePerSecondVar = 0;
    flecsParticleSystem = new (std::nothrow) FlecsParticleSystem();
}
// implementation ParticleSystem

ParticleSystem * ParticleSystem::create(const std::string& plistFile)
{
    ParticleSystem *ret = new (std::nothrow) ParticleSystem();
    if (ret && ret->initWithFile(plistFile))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return ret;
}

ParticleSystem* ParticleSystem::createWithTotalParticles(int numberOfParticles)
{
    ParticleSystem *ret = new (std::nothrow) ParticleSystem();
    if (ret && ret->initWithTotalParticles(numberOfParticles))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return ret;
}

// static
Vector<ParticleSystem*>& ParticleSystem::getAllParticleSystems()
{
    return __allInstances;
}

void ParticleSystem::setTotalParticleCountFactor(float factor)
{
    __totalParticleCountFactor = factor;
}

bool ParticleSystem::init()
{
    return initWithTotalParticles(150);
}

bool ParticleSystem::initWithFile(const std::string& plistFile)
{
    bool ret = false;
//    _quad = quad;
    _plistFile = FileUtils::getInstance()->fullPathForFilename(plistFile);
    ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(_plistFile);

    CCASSERT( !dict.empty(), "Particles: file not found");
    
    // FIXME: compute path from a path, should define a function somewhere to do it
    string listFilePath = plistFile;
    if (listFilePath.find('/') != string::npos)
    {
        listFilePath = listFilePath.substr(0, listFilePath.rfind('/') + 1);
        ret = this->initWithDictionary(dict, listFilePath);
    }
    else
    {
        ret = this->initWithDictionary(dict, "");
    }
    
    return ret;
}

bool ParticleSystem::initWithDictionary(ValueMap& dictionary)
{
    return initWithDictionary(dictionary, "");
}

bool ParticleSystem::initWithDictionary(ValueMap& dictionary, const std::string& dirname)
{
    bool ret = false;
    unsigned char *buffer = nullptr;
    unsigned char *deflated = nullptr;
    Image *image = nullptr;
//    _quad = quad;
    do 
    {
        int maxParticles = dictionary["maxParticles"].asInt();
        // self, not super
        if(this->initWithTotalParticles(maxParticles))
        {
            // Emitter name in particle designer 2.0
            _configName = dictionary["configName"].asString();

            // angle
            _angle = dictionary["angle"].asFloat();
            _angleVar = dictionary["angleVariance"].asFloat();

            // duration
            _duration = dictionary["duration"].asFloat();

            // blend function 
            if (!_configName.empty())
            {
                _blendFunc.src = dictionary["blendFuncSource"].asFloat();
            }
            else
            {
                _blendFunc.src = dictionary["blendFuncSource"].asInt();
            }
            _blendFunc.dst = dictionary["blendFuncDestination"].asInt();

            // color
            _startColor.r = dictionary["startColorRed"].asFloat();
            _startColor.g = dictionary["startColorGreen"].asFloat();
            _startColor.b = dictionary["startColorBlue"].asFloat();
            _startColor.a = dictionary["startColorAlpha"].asFloat();

            _startColorVar.r = dictionary["startColorVarianceRed"].asFloat();
            _startColorVar.g = dictionary["startColorVarianceGreen"].asFloat();
            _startColorVar.b = dictionary["startColorVarianceBlue"].asFloat();
            _startColorVar.a = dictionary["startColorVarianceAlpha"].asFloat();

            _endColor.r = dictionary["finishColorRed"].asFloat();
            _endColor.g = dictionary["finishColorGreen"].asFloat();
            _endColor.b = dictionary["finishColorBlue"].asFloat();
            _endColor.a = dictionary["finishColorAlpha"].asFloat();

            _endColorVar.r = dictionary["finishColorVarianceRed"].asFloat();
            _endColorVar.g = dictionary["finishColorVarianceGreen"].asFloat();
            _endColorVar.b = dictionary["finishColorVarianceBlue"].asFloat();
            _endColorVar.a = dictionary["finishColorVarianceAlpha"].asFloat();

            // particle size
            _startSize = dictionary["startParticleSize"].asFloat();
            _startSizeVar = dictionary["startParticleSizeVariance"].asFloat();
            _endSize = dictionary["finishParticleSize"].asFloat();
            _endSizeVar = dictionary["finishParticleSizeVariance"].asFloat();

            // position
            float x = dictionary["sourcePositionx"].asFloat();
            float y = dictionary["sourcePositiony"].asFloat();
	    if(!_sourcePositionCompatible) {
                this->setSourcePosition(Vec2(x, y));
	    }
            else {
		this->setPosition(Vec2(x, y));
	    }
            _posVar.x = dictionary["sourcePositionVariancex"].asFloat();
            _posVar.y = dictionary["sourcePositionVariancey"].asFloat();

            // Spinning
            _startSpin = dictionary["rotationStart"].asFloat();
            _startSpinVar = dictionary["rotationStartVariance"].asFloat();
            _endSpin= dictionary["rotationEnd"].asFloat();
            _endSpinVar= dictionary["rotationEndVariance"].asFloat();

            _emitterMode = (Mode) dictionary["emitterType"].asInt();

            // Mode A: Gravity + tangential accel + radial accel
            if (_emitterMode == Mode::GRAVITY)
            {
                // gravity
                modeA.gravity.x = dictionary["gravityx"].asFloat();
                modeA.gravity.y = dictionary["gravityy"].asFloat();

                // speed
                modeA.speed = dictionary["speed"].asFloat();
                modeA.speedVar = dictionary["speedVariance"].asFloat();

                // radial acceleration
                modeA.radialAccel = dictionary["radialAcceleration"].asFloat();
                modeA.radialAccelVar = dictionary["radialAccelVariance"].asFloat();

                // tangential acceleration
                modeA.tangentialAccel = dictionary["tangentialAcceleration"].asFloat();
                modeA.tangentialAccelVar = dictionary["tangentialAccelVariance"].asFloat();
                
                // rotation is dir
                modeA.rotationIsDir = dictionary["rotationIsDir"].asBool();
            }

            // or Mode B: radius movement
            else if (_emitterMode == Mode::RADIUS)
            {
                if (!_configName.empty())
                {
                    modeB.startRadius = dictionary["maxRadius"].asInt();
                }
                else
                {
                    modeB.startRadius = dictionary["maxRadius"].asFloat();
                }
                modeB.startRadiusVar = dictionary["maxRadiusVariance"].asFloat();
                if (!_configName.empty())
                {
                    modeB.endRadius = dictionary["minRadius"].asInt();
                }
                else
                {
                    modeB.endRadius = dictionary["minRadius"].asFloat();
                }
                
                if (dictionary.find("minRadiusVariance") != dictionary.end())
                {
                    modeB.endRadiusVar = dictionary["minRadiusVariance"].asFloat();
                }
                else
                {
                    modeB.endRadiusVar = 0.0f;
                }
                
                if (!_configName.empty())
                {
                    modeB.rotatePerSecond = dictionary["rotatePerSecond"].asInt();
                }
                else
                {
                    modeB.rotatePerSecond = dictionary["rotatePerSecond"].asFloat();
                }
                modeB.rotatePerSecondVar = dictionary["rotatePerSecondVariance"].asFloat();

            } else {
                CCASSERT( false, "Invalid emitterType in config file");
                CC_BREAK_IF(true);
            }

            // life span
            _life = dictionary["particleLifespan"].asFloat();
            _lifeVar = dictionary["particleLifespanVariance"].asFloat();

            // emission Rate
            _emissionRate = _totalParticles / _life;

            //don't get the internal texture if a batchNode is used
            if (!_batchNode)
            {
                // Set a compatible default for the alpha transfer
                _opacityModifyRGB = false;

                // texture        
                // Try to get the texture from the cache
                std::string textureName = dictionary["textureFileName"].asString();
                
                size_t rPos = textureName.rfind('/');
               
                if (rPos != string::npos)
                {
                    string textureDir = textureName.substr(0, rPos + 1);
                    
                    if (!dirname.empty() && textureDir != dirname)
                    {
                        textureName = textureName.substr(rPos+1);
                        textureName = dirname + textureName;
                    }
                }
                else if (!dirname.empty() && !textureName.empty())
                {
                	textureName = dirname + textureName;
                }
                
                Texture2D *tex = nullptr;
                
                if (!textureName.empty())
                {
                    // set not pop-up message box when load image failed
                    bool notify = FileUtils::getInstance()->isPopupNotify();
                    FileUtils::getInstance()->setPopupNotify(false);
                    tex = Director::getInstance()->getTextureCache()->addImage(textureName);
                    // reset the value of UIImage notify
                    FileUtils::getInstance()->setPopupNotify(notify);
                }
                
                if (tex)
                {
                    setTexture(tex);
                }
                else if( dictionary.find("textureImageData") != dictionary.end() )
                {                        
                    std::string textureData = dictionary.at("textureImageData").asString();
                    CCASSERT(!textureData.empty(), "textureData can't be empty!");
                    
                    auto dataLen = textureData.size();
                    if (dataLen != 0)
                    {
                        // if it fails, try to get it from the base64-gzipped data    
                        int decodeLen = base64Decode((unsigned char*)textureData.c_str(), (unsigned int)dataLen, &buffer);
                        CCASSERT( buffer != nullptr, "CCParticleSystem: error decoding textureImageData");
                        CC_BREAK_IF(!buffer);
                        
                        ssize_t deflatedLen = ZipUtils::inflateMemory(buffer, decodeLen, &deflated);
                        CCASSERT( deflated != nullptr, "CCParticleSystem: error ungzipping textureImageData");
                        CC_BREAK_IF(!deflated);
                        
                        // For android, we should retain it in VolatileTexture::addImage which invoked in Director::getInstance()->getTextureCache()->addUIImage()
                        image = new (std::nothrow) Image();
                        bool isOK = image->initWithImageData(deflated, deflatedLen);
                        CCASSERT(isOK, "CCParticleSystem: error init image with Data");
                        CC_BREAK_IF(!isOK);
                        
                        setTexture(Director::getInstance()->getTextureCache()->addImage(image, _plistFile + textureName));

                        image->release();
                    }
                }
                
                _yCoordFlipped = dictionary.find("yCoordFlipped") == dictionary.end() ? 1 : dictionary.at("yCoordFlipped").asInt();

                if( !this->_texture)
                    CCLOGWARN("cocos2d: Warning: ParticleSystemQuad system without a texture");
            }
            ret = true;
        }
    } while (0);
    free(buffer);
    free(deflated);
    return ret;
}

bool ParticleSystem::initWithTotalParticles(int numberOfParticles)
{
    _totalParticles = numberOfParticles;
//    _quad = quad;
    
//    _particleData.release();
    flecsParticleSystem->init();
    flecsParticleSystem->setMaxCount(numberOfParticles);
    flecs::world* ECSWorld = flecsParticleSystem->GetEcsWorld();
    if (ECSWorld == nullptr)
    {
        CCLOG("Particle system: ECSWorld init failed");
        return false;
    }
//    if( !_particleData.init() )
//    {
//        CCLOG("Particle system: not enough memory");
//        this->release();
//        return false;
//    }
    _allocatedParticles = numberOfParticles;

    if (_batchNode) // todo: batchNode
    {
        for (int i = 0; i < _totalParticles; i++)
        {
//            _particleData.atlasIndex[i] = i;
        }
    }
    // default, active
    _isActive = true;

    // default blend function
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

    // default movement type;
    _positionType = PositionType::FREE;

    // by default be in mode A:
    _emitterMode = Mode::GRAVITY;

    // default: modulate
    // FIXME:: not used
    //    colorModulate = YES;

    _isAutoRemoveOnFinish = false;

    // Optimization: compile updateParticle method
    //updateParticleSel = @selector(updateQuadWithParticle:newPosition:);
    //updateParticleImp = (CC_UPDATE_PARTICLE_IMP) [self methodForSelector:updateParticleSel];
    //for batchNode
    _transformSystemDirty = false;

    return true;
}

ParticleSystem::~ParticleSystem()
{
    // Since the scheduler retains the "target (in this case the ParticleSystem)
	// it is not needed to call "unscheduleUpdate" here. In fact, it will be called in "cleanup"
    //unscheduleUpdate();
//    _particleData.release();
    CC_SAFE_RELEASE(_texture);
}

// oop method
void ParticleSystem::addParticles(int count)
{
    if (_paused)
        return;
    uint32_t RANDSEED = rand();

//    int start = _particleCount;
    _particleCount += count;

    for (int i = 0; i < count ; ++i)
    {
        // life
        float theLife = _life + _lifeVar * RANDOM_M11(&RANDSEED);
        theLife = MAX(0, theLife);

        // position
        Vec2 position;
        position.x = _sourcePosition.x + _posVar.x * RANDOM_M11(&RANDSEED);
        position.y = _sourcePosition.y + _posVar.y * RANDOM_M11(&RANDSEED);

        // color
        float colorR = clampf( _startColor.r + _startColorVar.r * RANDOM_M11(&RANDSEED) , 0 , 1 ); // todo: check function clampf
        float colorG = clampf( _startColor.g + _startColorVar.g * RANDOM_M11(&RANDSEED) , 0 , 1 );
        float colorB = clampf( _startColor.b + _startColorVar.b * RANDOM_M11(&RANDSEED) , 0 , 1 );
        float colorA = clampf( _startColor.a + _startColorVar.a * RANDOM_M11(&RANDSEED) , 0 , 1 );
        float deltaColorR = (clampf( _endColor.r + _endColorVar.r * RANDOM_M11(&RANDSEED) , 0 , 1 ) - colorR) / theLife;
        float deltaColorG = (clampf( _endColor.g + _endColorVar.g * RANDOM_M11(&RANDSEED) , 0 , 1 ) - colorG) / theLife;
        float deltaColorB = (clampf( _endColor.b + _endColorVar.b * RANDOM_M11(&RANDSEED) , 0 , 1 ) - colorB) / theLife;
        float deltaColorA = (clampf( _endColor.a + _endColorVar.a * RANDOM_M11(&RANDSEED) , 0 , 1 ) - colorA) / theLife;

        // size
        float startSize = _startSize + _startSizeVar * RANDOM_M11(&RANDSEED);
        startSize = MAX(0, startSize);
        float deltaSize = 0.0f;
        if (_startSize != _endSize)
        {
            float endSize = _endSize + _endSizeVar * RANDOM_M11(&RANDSEED);
            endSize = MAX(0, endSize);
            deltaSize = (endSize - startSize) / theLife;
        }

        // rotation
        float rotation = _startSpin + _startSpinVar * RANDOM_M11(&RANDSEED);
        float endRot = _endSpin + _endSpinVar * RANDOM_M11(&RANDSEED);
        float deltaRotation = (endRot - rotation) / theLife;

        // position
        Vec2 pos;
        if (_positionType == PositionType::FREE)
        {
            pos = this->convertToWorldSpace(Vec2::ZERO);
        }
        else if (_positionType == PositionType::RELATIVE)
        {
            pos = _position;
        }
        float startPosX = pos.x;
        float startPosY = pos.y; // todo: check whether there is any influence of changing type from pointer to value

        // ModeA: Gravity
        float radialAccel, tangentialAccel, dirX, dirY;
        float radius, deltaRadius, angle, degreesPerSecond;
        if (_emitterMode == Mode::GRAVITY)
        {
            // radial accel
            radialAccel = modeA.radialAccel + modeA.radialAccelVar * RANDOM_M11(&RANDSEED);
            // tangential accel
            tangentialAccel = modeA.tangentialAccel + modeA.tangentialAccelVar * RANDOM_M11(&RANDSEED);
            // dir
            if (modeA.rotationIsDir)
            {
                float a = CC_DEGREES_TO_RADIANS( _angle + _angleVar * RANDOM_M11(&RANDSEED) );
                Vec2 v(cosf( a ), sinf( a )); //
                float s = modeA.speed + modeA.speedVar * RANDOM_M11(&RANDSEED);
                Vec2 dir = v * s;
                dirX = dir.x;//v * s ;
                dirY = dir.y;
                rotation = -CC_RADIANS_TO_DEGREES(dir.getAngle()); // todo: check function getAngle()
            }
            else
            {
                float a = CC_DEGREES_TO_RADIANS( _angle + _angleVar * RANDOM_M11(&RANDSEED) );
                Vec2 v(cosf( a ), sinf( a ));
                float s = modeA.speed + modeA.speedVar * RANDOM_M11(&RANDSEED);
                Vec2 dir = v * s;
                dirX = dir.x;//v * s ;
                dirY = dir.y;
            }
        }
        else
        {// ModeB: Circular
            // radius
            radius = modeB.startRadius + modeB.startRadiusVar * RANDOM_M11(&RANDSEED);
            deltaRadius = 0.0f;
            if (modeB.startRadius != modeB.endRadius)
            {
                float endRadius = modeB.endRadius + modeB.endRadiusVar * RANDOM_M11(&RANDSEED);
                deltaRadius = (endRadius - radius) / theLife;
            }
            // angle
            angle = CC_DEGREES_TO_RADIANS( _angle + _angleVar * RANDOM_M11(&RANDSEED)); // todo: check function CC_DEGREES_TO_RADIANS
            degreesPerSecond = CC_DEGREES_TO_RADIANS(modeB.rotatePerSecond + modeB.rotatePerSecondVar * RANDOM_M11(&RANDSEED));
        }

        // add entity
//        auto parent = it.entity(i);
        if (_emitterMode == Mode::GRAVITY) // modeA
        {
            auto entity = flecsParticleSystem->GetEcsWorld()->entity()
                .set<flecsLife>({ theLife })
                .set<flecsPosition>({ position })
                .set<flecsStartPosition>({ startPosX, startPosY })
                .set<flecsColor>({ colorR, colorG, colorB, colorA, deltaColorR, deltaColorG, deltaColorB, deltaColorA })
                .set<flecsSize>({ startSize, deltaSize })
                .set<flecsRotation>({ rotation, deltaRotation })
                .set<flecsYCoordFlipped>({ _yCoordFlipped })
                .set<flecsAccel>({ radialAccel, tangentialAccel, modeA.gravity })
                .set<flecsDir>({ dirX, dirY });
        }
        else // modeB
        {
            auto entity = flecsParticleSystem->GetEcsWorld()->entity()
			    .set<flecsLife>({ theLife })
			    .set<flecsPosition>({ position })
			    .set<flecsStartPosition>({ startPosX, startPosY })
			    .set<flecsColor>({ colorR, colorG, colorB, colorA, deltaColorR, deltaColorG, deltaColorB, deltaColorA })
			    .set<flecsSize>({ startSize, deltaSize })
			    .set<flecsRotation>({ rotation, deltaRotation })
			    .set<flecsYCoordFlipped>({ _yCoordFlipped })
			    .set<flecsRadius>({ radius, deltaRadius })
			    .set<flecsAngle>({ angle, degreesPerSecond });
        }
    }
}

void ParticleSystem::onEnter()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter))
            return;
    }
#endif
    
    Node::onEnter();
    
    // update after action in run!
    this->scheduleUpdateWithPriority(1);

    __allInstances.pushBack(this);
}

void ParticleSystem::onExit()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExit))
            return;
    }
#endif
    
    this->unscheduleUpdate();
    Node::onExit();

    auto iter = std::find(std::begin(__allInstances), std::end(__allInstances), this);
    if (iter != std::end(__allInstances))
    {
        __allInstances.erase(iter);
    }
}

void ParticleSystem::stopSystem()
{
    _isActive = false;
    _elapsed = _duration;
    _emitCounter = 0;
}

void ParticleSystem::resetSystem()
{
    _isActive = true;
    _elapsed = 0;
//    for (int i = 0; i < _particleCount; ++i)
//    {
//        _particleData.timeToLive[i] = 0.0f;
//    }
    // manually iterate entities and reset particles' life
    auto ECSWorld = flecsParticleSystem->GetEcsWorld();
    flecs::query<flecsLife> q = ECSWorld->query<flecsLife>();
    q.each([](flecs::entity e, flecsLife& fl) {
        fl.life = 0.0f;
    });
}

bool ParticleSystem::isFull()
{
    return (_particleCount == _totalParticles);
}

// ParticleSystem - MainLoop
void ParticleSystem::update(float dt)
{
    CC_PROFILER_START_CATEGORY(kProfilerCategoryParticles , "CCParticleSystem - update");

    if (_isActive && _emissionRate)
    {
        // count particles
        auto ECSWorld = flecsParticleSystem->GetEcsWorld();
        _particleCount = ECSWorld->count<flecsPosition>();
        float rate = 1.0f / _emissionRate;
        int totalParticles = static_cast<int>(_totalParticles * __totalParticleCountFactor);

        //issue #1201, prevent bursts of particles, due to too high emitCounter
        if (_particleCount < totalParticles)
        {
            _emitCounter += dt;
            if (_emitCounter < 0.f)
                _emitCounter = 0.f;
        }

        int emitCount = MIN(totalParticles - _particleCount, _emitCounter / rate);
        addParticles(emitCount);
        _emitCounter -= rate * emitCount;

        _elapsed += dt;
        if (_elapsed < 0.f)
            _elapsed = 0.f;
        if (_duration != DURATION_INFINITY && _duration < _elapsed)
        {
            this->stopSystem();
        }
    }

    // particle update with ECS
    if (!flecsParticleSystem->Tick(dt))
    {
        CCLOG("Particle system: ECS Tick failed");
    }

    // set quad for render
    updateParticleQuads();
    _transformSystemDirty = false;

    // only update gl buffer when visible
    if (_visible && ! _batchNode)
    {
        postStep();
    }

    CC_PROFILER_STOP_CATEGORY(kProfilerCategoryParticles , "CCParticleSystem - update");
}

// oop render system

void ParticleSystem::updateWithNoTime(void)
{
    this->update(0.0f);
}

void ParticleSystem::updateParticleQuads()
{
    //should be overridden
}

void ParticleSystem::postStep()
{
    // should be overridden
}

// ParticleSystem - Texture protocol
void ParticleSystem::setTexture(Texture2D* var)
{
    if (_texture != var)
    {
        CC_SAFE_RETAIN(var);
        CC_SAFE_RELEASE(_texture);
        _texture = var;
        updateBlendFunc();
    }
}

void ParticleSystem::updateBlendFunc()
{
    CCASSERT(! _batchNode, "Can't change blending functions when the particle is being batched");

    if(_texture)
    {
        bool premultiplied = _texture->hasPremultipliedAlpha();
        
        _opacityModifyRGB = false;
        
        if( _texture && ( _blendFunc.src == CC_BLEND_SRC && _blendFunc.dst == CC_BLEND_DST ) )
        {
            if( premultiplied )
            {
                _opacityModifyRGB = true;
            }
            else
            {
                _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
            }
        }
    }
}

Texture2D * ParticleSystem::getTexture() const
{
    return _texture;
}

// ParticleSystem - Additive Blending
void ParticleSystem::setBlendAdditive(bool additive)
{
    if( additive )
    {
        _blendFunc = BlendFunc::ADDITIVE;
    }
    else
    {
        if( _texture && ! _texture->hasPremultipliedAlpha() )
            _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
        else 
            _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    }
}

bool ParticleSystem::isBlendAdditive() const
{
    return( _blendFunc.src == GL_SRC_ALPHA && _blendFunc.dst == GL_ONE);
}

// ParticleSystem - Properties of Gravity Mode 
void ParticleSystem::setTangentialAccel(float t)
{
    CCASSERT( _emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.tangentialAccel = t;
}

float ParticleSystem::getTangentialAccel() const
{
    CCASSERT( _emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.tangentialAccel;
}

void ParticleSystem::setTangentialAccelVar(float t)
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.tangentialAccelVar = t;
}

float ParticleSystem::getTangentialAccelVar() const
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.tangentialAccelVar;
}    

void ParticleSystem::setRadialAccel(float t)
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.radialAccel = t;
}

float ParticleSystem::getRadialAccel() const
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.radialAccel;
}

void ParticleSystem::setRadialAccelVar(float t)
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.radialAccelVar = t;
}

float ParticleSystem::getRadialAccelVar() const
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.radialAccelVar;
}

void ParticleSystem::setRotationIsDir(bool t)
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.rotationIsDir = t;
}

bool ParticleSystem::getRotationIsDir() const
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.rotationIsDir;
}

void ParticleSystem::setGravity(const Vec2& g)
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.gravity = g;
}

const Vec2& ParticleSystem::getGravity()
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.gravity;
}

void ParticleSystem::setSpeed(float speed)
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.speed = speed;
}

float ParticleSystem::getSpeed() const
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.speed;
}

void ParticleSystem::setSpeedVar(float speedVar)
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.speedVar = speedVar;
}

float ParticleSystem::getSpeedVar() const
{
    CCASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.speedVar;
}

// ParticleSystem - Properties of Radius Mode
void ParticleSystem::setStartRadius(float startRadius)
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.startRadius = startRadius;
}

float ParticleSystem::getStartRadius() const
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.startRadius;
}

void ParticleSystem::setStartRadiusVar(float startRadiusVar)
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.startRadiusVar = startRadiusVar;
}

float ParticleSystem::getStartRadiusVar() const
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.startRadiusVar;
}

void ParticleSystem::setEndRadius(float endRadius)
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.endRadius = endRadius;
}

float ParticleSystem::getEndRadius() const
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.endRadius;
}

void ParticleSystem::setEndRadiusVar(float endRadiusVar)
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.endRadiusVar = endRadiusVar;
}

float ParticleSystem::getEndRadiusVar() const
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.endRadiusVar;
}

void ParticleSystem::setRotatePerSecond(float degrees)
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.rotatePerSecond = degrees;
}

float ParticleSystem::getRotatePerSecond() const
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.rotatePerSecond;
}

void ParticleSystem::setRotatePerSecondVar(float degrees)
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.rotatePerSecondVar = degrees;
}

float ParticleSystem::getRotatePerSecondVar() const
{
    CCASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.rotatePerSecondVar;
}

bool ParticleSystem::isActive() const
{
    return _isActive;
}

int ParticleSystem::getTotalParticles() const
{
    return _totalParticles;
}

void ParticleSystem::setTotalParticles(int var)
{
    CCASSERT( var <= _allocatedParticles, "Particle: resizing particle array only supported for quads");
    _totalParticles = var;
}

const BlendFunc& ParticleSystem::getBlendFunc() const
{
    return _blendFunc;
}

void ParticleSystem::setBlendFunc(const BlendFunc &blendFunc)
{
    if( _blendFunc.src != blendFunc.src || _blendFunc.dst != blendFunc.dst ) {
        _blendFunc = blendFunc;
        this->updateBlendFunc();
    }
}

bool ParticleSystem::isAutoRemoveOnFinish() const
{
    return _isAutoRemoveOnFinish;
}

void ParticleSystem::setAutoRemoveOnFinish(bool var)
{
    _isAutoRemoveOnFinish = var;
}


// ParticleSystem - methods for batchNode rendering

ParticleBatchNode* ParticleSystem::getBatchNode(void) const
{
    return _batchNode;
}

void ParticleSystem::setBatchNode(ParticleBatchNode* batchNode)
{
    if( _batchNode != batchNode ) {

//        _batchNode = batchNode; // weak reference
        _batchNode = nullptr; // todo
        if( batchNode ) {
            //each particle needs a unique index
            for (int i = 0; i < _totalParticles; i++)
            {
//                _particleData.atlasIndex[i] = i;
            }
        }
    }
}

//don't use a transform matrix, this is faster
void ParticleSystem::setScale(float s)
{
    _transformSystemDirty = true;
    Node::setScale(s);
}

void ParticleSystem::setRotation(float newRotation)
{
    _transformSystemDirty = true;
    Node::setRotation(newRotation);
}

void ParticleSystem::setScaleX(float newScaleX)
{
    _transformSystemDirty = true;
    Node::setScaleX(newScaleX);
}

void ParticleSystem::setScaleY(float newScaleY)
{
    _transformSystemDirty = true;
    Node::setScaleY(newScaleY);
}

void ParticleSystem::start()
{
    resetSystem();
}

void ParticleSystem::stop()
{
    stopSystem();
}

bool ParticleSystem::isPaused() const
{
    return _paused;
}

void ParticleSystem::pauseEmissions()
{
    _paused = true;
}

void ParticleSystem::resumeEmissions()
{
    _paused = false;
}



NS_CC_END
