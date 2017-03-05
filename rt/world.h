#ifndef CG1RAYTRACER_WORLD_HEADER
#define CG1RAYTRACER_WORLD_HEADER

#include <rt/primitive.h>
#include <rt/Fog.h>

namespace rt {
class Light;
class World {
public:
    World(Fog *fog = nullptr) :fog(fog){}
    Primitive* scene;
	typedef std::vector<Light*> LightVector;
    LightVector light;
    Fog *fog;

};
}

#endif
