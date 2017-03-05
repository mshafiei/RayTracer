
#include <core/assert.h>
#include <core/scalar.h>
#include <core/image.h>
#include <rt/world.h>
#include <rt/renderer.h>
#include <cmath>

#include <rt/cameras/perspective.h>

#include <rt/materials/lambertian.h>

#include <rt/textures/constant.h>
#include <rt/textures/imagetex.h>
#include <rt/textures/checkerboard.h>
#include <rt/textures/perlin.h>

#include <rt/lights/pointlight.h>

#include <rt/solids/quad.h>
#include <rt/solids/triangle.h>
#include <rt/solids/sphere.h>
#include <rt/integrators/raytrace.h>
#include <rt/solids/specialsolid.h>

#include <rt/groups/simplegroup.h>
#include <rt/coordmappers/plane.h>
#include <rt/coordmappers/cylindrical.h>
#include <rt/coordmappers/spherical.h>
#include <rt/coordmappers/tmapper.h>
#include <rt/coordmappers/environmentmapper.h>
#include <rt/materials/flatmaterial.h>
#include <rt/integrators/homvolintegrator.h>
#include <rt/lights/Ambient.h>
using namespace rt;

void trymapper3(const char* filename, CoordMapper* spheremapper1, CoordMapper* spheremapper2) {
    static const float scale = 0.001f;
    Image img(400, 400);
    World world(new Fog(5, RGBColor(1, 1, 1)));
    SimpleGroup scene;
    world.scene = &scene;

    PerspectiveCamera cam(Point(278 * scale, 273 * scale, -800 * scale), Vector(0, 0, 1), Vector(0, 1, 0), 0.686f, 0.686f);

    CheckerboardTexture* redtex = new CheckerboardTexture(RGBColor(.7f, 0.1f, 0.1f), RGBColor(0.3f, 0.1f, 0.1f));
    PerlinTexture* greentex = new PerlinTexture(RGBColor(0.f, .7f, 0.f), RGBColor(0.0f, 0.2f, 0.4f));
    greentex->addOctave(1.0f, 3.0f);
    greentex->addOctave(0.5f, 6.0f);
    greentex->addOctave(0.25f, 12.0f);
    ConstantTexture* blacktex = new ConstantTexture(RGBColor::rep(0.0f));
    ConstantTexture* lightsrctex = new ConstantTexture(RGBColor::rep(1.0f));

    ImageTexture* whitetex = new ImageTexture("models/stones_diffuse.png");
    ImageTexture* clamptex = new ImageTexture("models/stones_diffuse.png", ImageTexture::CLAMP);
    ImageTexture* mirrtex = new ImageTexture("models/stones_diffuse.png", ImageTexture::MIRROR);

    LambertianMaterial white(blacktex, whitetex);
    LambertianMaterial clamp(blacktex, clamptex);
    LambertianMaterial mirror(blacktex, mirrtex);
    LambertianMaterial green(blacktex, greentex);
    LambertianMaterial red(blacktex, redtex);

    //point light
    world.light.push_back(new PointLight(Point((178)*scale, 429.99f*scale, (279.5f)*scale), RGBColor::rep(100000.0f*scale*scale)));
    world.light.push_back(new PointLight(Point(478 * scale, 229.99f*scale, (-59.5f)*scale), RGBColor::rep(150000.0f*scale*scale)));

    world.light.push_back(new PointLight(Point(490 * scale, 159.99f*scale, 279.5f*scale), RGBColor(40000.0f*scale*scale, 0, 0)));
    world.light.push_back(new PointLight(Point(40 * scale, 159.99f*scale, 249.5f*scale), RGBColor(5000.0f*scale*scale, 30000.0f*scale*scale, 5000.0f*scale*scale)));
    world.light.push_back(new AmbientLight(RGBColor(1, 1, 1)));

    TriangleMapper* bottomleft = new TriangleMapper(Point(0, 0, 0), Point(3, 0, 0), Point(0, 3, 0));
    TriangleMapper* topright = new TriangleMapper(Point(3, 3, 0), Point(3, 0, 0), Point(0, 3, 0));
    //floor
    scene.add(new Triangle(Point(000.f, 000.f, 000.f)*scale, Point(550.f, 000.f, 000.f)*scale, Point(000.f, 000.f, 560.f)*scale, bottomleft, &clamp));
    scene.add(new Triangle(Point(550.f, 000.f, 560.f)*scale, Point(550.f, 000.f, 000.f)*scale, Point(000.f, 000.f, 560.f)*scale, topright, &clamp));

    //ceiling
    scene.add(new Triangle(Point(000.f, 550.f, 000.f)*scale, Point(550.f, 550.f, 000.f)*scale, Point(000.f, 550.f, 560.f)*scale, bottomleft, &mirror));
    scene.add(new Triangle(Point(550.f, 550.f, 560.f)*scale, Point(550.f, 550.f, 000.f)*scale, Point(000.f, 550.f, 560.f)*scale, topright, &mirror));

    //back wall
    scene.add(new Triangle(Point(000.f, 000.f, 560.f)*scale, Point(550.f, 000.f, 560.f)*scale, Point(000.f, 550.f, 560.f)*scale, bottomleft, &white));
    scene.add(new Triangle(Point(550.f, 550.f, 560.f)*scale, Point(550.f, 000.f, 560.f)*scale, Point(000.f, 550.f, 560.f)*scale, topright, &white));

    //right wall
    scene.add(new Triangle(Point(000.f, 000.f, 000.f)*scale, Point(000.f, 000.f, 560.f)*scale, Point(000.f, 550.f, 000.f)*scale, bottomleft, &green));
    scene.add(new Triangle(Point(000.f, 550.f, 560.f)*scale, Point(000.f, 000.f, 560.f)*scale, Point(000.f, 550.f, 000.f)*scale, topright, &green));

    //left wall
    scene.add(new Triangle(Point(550.f, 000.f, 000.f)*scale, Point(550.f, 000.f, 560.f)*scale, Point(550.f, 550.f, 000.f)*scale, bottomleft, &red));
    scene.add(new Triangle(Point(550.f, 550.f, 560.f)*scale, Point(550.f, 000.f, 560.f)*scale, Point(550.f, 550.f, 000.f)*scale, topright, &red));

    //sphere
    scene.add(new Sphere(Point(400.f, 450.f, 300.f)*scale, 150.f*scale, spheremapper1, &white));
    scene.add(new Sphere(Point(200.f, 100.f, 300.f)*scale, 150.f*scale, spheremapper2, &white));

    HomogenousVolumeIntegrator integrator(&world);

    Renderer engine(&cam, &integrator);
    engine.render(img);
    img.writePNG(filename);
}

void a_volumetric() {
    float hsq2 = 0.5f / std::sqrt(2.0f);
    trymapper3("vol-1.png",
        new SphericalCoordMapper(Point(.4f, .45f, .3f), Vector(0, hsq2, hsq2), Vector(0.5f, 0.0f, 0.0f)),
        new SphericalCoordMapper(Point(.3f, .1f, .3f), Vector(0, hsq2, -hsq2), Vector(0.5f, 0.0f, 0.0f))
        );
    
}

