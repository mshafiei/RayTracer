
#include <core/assert.h>
#include <core/scalar.h>
#include <core/image.h>
#include <rt/world.h>
#include <rt/groups/simplegroup.h>
#include <rt/solids/sphere.h>
#include <rt/solids/infiniteplane.h>
#include <rt/solids/triangle.h>
#include <rt/solids/quad.h>
#include <rt/solids/disc.h>
#include <rt/solids/aabox.h>
#include <rt/cameras/perspective.h>
#include <rt/integrators/casting.h>
#include <rt/integrators/castingdist.h>
#include <rt/renderer.h>
#include <rt/groups/CSGGroup.h>
using namespace rt;

void a_solids() {

    Image img(640, 480);
    
    SimpleGroup* csgscene = new SimpleGroup();

   /* CSGGroup grp(new Sphere(Point(0, 0, 0), 2, nullptr, nullptr), new Sphere(Point(-3, 0, 0), 2, nullptr, nullptr), SUBTRACTION);
    CSGGroup othergrp(&grp, new Sphere(Point(-3, 1, 0), 2, nullptr, nullptr), INTERSECTION);
    csgscene->add(&grp);*/

    //CSGGroup grp1(new Sphere(Point(0, 0, 0), 2, nullptr, nullptr), new AABox(Point(0, 1, 1), Point(3, -3, -3), nullptr, nullptr), INTERSECTION);
    ////CSGGroup othergrp(&grp, new Sphere(Point(-3, 1, 0), 2, nullptr, nullptr), INTERSECTION);
    //csgscene->add(&grp1);

    CSGGroup grp1(new Sphere(Point(0, 0, 0), 2, nullptr, nullptr), new Sphere(Point(2, 0, 1), 2, nullptr, nullptr), SUBTRACTION);
    //CSGGroup othergrp(&grp1, new Sphere(Point(-3, 1, 0), 2, nullptr, nullptr), UNION);
    csgscene->add(&grp1);

    SimpleGroup* scene = new SimpleGroup();
    /*scene->add(new Sphere(Point(-2.f,  1.7f,  0), 2   , nullptr, nullptr));
    scene->add(new Sphere(Point(1,    -1,     1), 2.2f, nullptr, nullptr));
    scene->add(new Sphere(Point(3.f,   0.8f, -2), 2, nullptr, nullptr));
    scene->add(new InfinitePlane(Point(0,- 1, 0), Vector(0, 1, 0), nullptr, nullptr));
    scene->add(new Triangle(Point(-2, 3.7f, 0), Point(1, 2,  1), Point( 3, 2.8f, -2), nullptr, nullptr));
    scene->add(new Triangle(Point(3,  2,    3), Point(3, 2, -3), Point(-3, 2   , -3), nullptr, nullptr));
    scene->add(new Quad(Point(1,  -0.9f,    4.5f), Vector(-2, 0, 0), Vector(0, 0.1f, -2), nullptr, nullptr));
    scene->add(new Disc(Point(-3, -0.75f, 1.5f), Vector(0, 0.5f, 0.5f), 1.5f, nullptr, nullptr));
    scene->add(new AABox(Point(2,1.5f,-0.5f), Point(3,2.5f,2.5f), nullptr, nullptr));
    */
    World world;
    world.scene = scene;
    PerspectiveCamera cam(Point(0, 0, 10), Vector(0, 0, -1), Vector(0, 1, 0), pi/4, pi/3);
    
    RayCastingIntegrator integrator(&world);
    Renderer engine(&cam, &integrator);
    engine.render(img);
    img.writePNG("a2-1.png");

    RayCastingDistIntegrator integratorb(&world, RGBColor(1.0f, 0.2f, 0.0f), 4.0f, RGBColor(0.2f, 1.0f, 0.0f), 12.0f);
    Renderer engineb(&cam, &integratorb);
    engineb.render(img);
    img.writePNG("a2-2.png");

    world.scene = csgscene;
    RayCastingDistIntegrator integratorc(&world, RGBColor(1.0f, 0.2f, 0.0f), 4.0f, RGBColor(0.2f, 1.0f, 0.0f), 12.0f);
    Renderer enginec(&cam, &integratorc);
    enginec.render(img);
    img.writePNG("csg-1.png");
}
