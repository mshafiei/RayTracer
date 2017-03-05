
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
#include <rt/cameras/dofperspective.h>
#include <rt/groups/simplegroup.h>
#include <rt/coordmappers/plane.h>
#include <rt/coordmappers/cylindrical.h>
#include <rt/coordmappers/spherical.h>
#include <rt/coordmappers/tmapper.h>
#include <rt/coordmappers/environmentmapper.h>
#include <rt/materials/flatmaterial.h>
#include <rt/groups/CSGGroup.h>
#include <rt/solids/aabox.h>
#include <rt/loaders/obj.h>
#include <rt/integrators/casting.h>
#include <rt/groups/kdtree.h>
#include <rt/primmod/instance.h>
#include <rt/solids/cylinder.h>
#include <rt/solids/disc.h>
#include <rt/solids/infiniteplane.h>
#include <rt/lights/directional.h>
#include <rt/materials/mirror.h>
#include <rt/materials/combine.h>
using namespace rt;
RGBColor Blue(0.3f,0.4901f,0.6941f);
RGBColor Red(0.7803f, 0.1333f, 0.14900f);
RGBColor Brown(0.5333f, 0.2117f, 0.1254f);
RGBColor Green(0.1843f, 0.4039f, 0.2117f);
RGBColor Gray(0.7098f, 0.6392f, 0.6f);
RGBColor Orange(1, 0.6f, 0.2901f);

enum PrimSide
{
    LEFT = 0, RIGHT, TOP, BOTTOM,FRONT,BEHIND
};

Vector up(0, 1.0f, 0.0f);
Vector fwd(-0.6f, -0.3f, 0.0f);
//Vector fwd(0, -0.3f, 0.6f);

Primitive* tableDisk(float radius,float diskWidth)
{
    float dw = diskWidth / 2.0f;
    AABox *upperBox = new AABox(Point(-radius, -radius - dw, -radius), Point(radius, 0-dw, radius),nullptr,nullptr);
    AABox *lowerBox = new AABox(Point(-radius, 0 + dw, -radius), Point(radius, radius + dw, radius), nullptr, nullptr);
    Sphere *sphere = new Sphere(Point(0, 0, 0), radius, nullptr, nullptr);
    CSGGroup *csg1 = new CSGGroup(sphere, upperBox,rt::SUBTRACTION);
    return new CSGGroup(csg1, lowerBox, rt::SUBTRACTION);
}

Primitive* singleLegoCube(const RGBColor& color, float scale, int n, int m,int k)
{
    float const height = 9.6f * scale;
    float const width = 8.0f * scale;
    float const cylRadius = 4.8f / 2.0f  * scale;
    float const cylHeight = 1.8f  * scale;

    Point position(n * width, m * width, height * k);

    Point TopRight = Point(width / 2, width / 2, height / 2);
    Point BottomLeft = Point(-width / 2, -width / 2, -height / 2);
    Point TopMiddle = Point(0, 0, height / 2);
    LambertianMaterial *mat = new LambertianMaterial(new ConstantTexture(RGBColor(0,0,0)), new ConstantTexture(color));

    AABox *body = new AABox(BottomLeft + position, TopRight + position, nullptr, mat);
    Cylinder *emptyCyl = new Cylinder(cylRadius, 0, cylHeight, 2 * pi, nullptr, mat);
    Disc *cylWrap = new Disc(Point(position.x, position.y, position.z + TopMiddle.z + cylHeight), Vector(0, 0, 1), cylRadius,
        nullptr, mat);

    Instance *inst = new Instance(emptyCyl);
    inst->translate(Vector(position.x, position.y, position.z + TopMiddle.z));
    //KDTree *grp = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    CSGGroup * grp1 = new CSGGroup(cylWrap, inst, rt::UNION);
    CSGGroup * grp2 = new CSGGroup(grp1, body, rt::UNION);
    SimpleGroup* grp = new SimpleGroup();
    grp->add(cylWrap);
    grp->add(inst);
    grp->add(body);
    grp->rebuildIndex();
    Instance *ins = new Instance(grp2);
    ins->rotate(Vector(1, 0, 0), -pi / 2.0f);
    return ins;
}

Primitive* Block(int n, int m,int l, const RGBColor& color)
{
    //KDTree *grp = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    SimpleGroup* grp = new SimpleGroup();
    for (int i = 0; i < n;++i)
        for (int j = 0; j < m; ++j)
            for (int k = 0; k < l; ++k)
                {
                    grp->add(singleLegoCube(color, 0.3f, i, j, k));
                }
    grp->rebuildIndex();
    return grp;
}


Primitive* MoveThis(Primitive* pr,const Point& pos, const Point& Or)
{
    Instance * upperRow = new Instance(pr);
    upperRow->rotate(Vector(1, 0, 0), Or.x * pi / 180.0f);
    upperRow->rotate(Vector(0, 1, 0), Or.y * pi / 180.0f);
    upperRow->rotate(Vector(0, 0, 1), Or.z * pi / 180.0f);

    upperRow->translate(Vector(pos.x,pos.y,pos.z));
    return upperRow;
}

Primitive* MoveThis(Primitive* pr, PrimSide side, int nBlocks, Primitive *central = nullptr)
{
    if (central == nullptr)
        central = Block(1, 1, 1, RGBColor(0, 0, 0));

    BBox tBox = central->getBounds();
    Point Center((tBox.min.x + tBox.max.x) / 2.0f, (tBox.min.y + tBox.max.y) / 2.0f, (tBox.min.z + tBox.max.z) / 2.0f);
    switch (side)
    {
    case LEFT:
        return MoveThis(pr, Point(-(tBox.max - tBox.min).x, 0, 0)*nBlocks, Point(0, 0, 0));
    case RIGHT:
        return MoveThis(pr, Point((tBox.max - tBox.min).x, 0, 0)*nBlocks, Point(0, 0, 0));
    case TOP:
        return MoveThis(pr, Point(0, (tBox.max - tBox.min).y / 1.5f, 0)*nBlocks, Point(0, 0, 0));
    case BOTTOM:
        return MoveThis(pr, Point(0, -(tBox.max - tBox.min).y, 0) / 1.5f*nBlocks, Point(0, 0, 0));
    case BEHIND:
        return MoveThis(pr, Point(0, 0, -(tBox.max - tBox.min).z)*nBlocks, Point(0, 0, 0));
    case FRONT:
        return MoveThis(pr, Point(0, 0, (tBox.max - tBox.min).z)*nBlocks, Point(0, 0, 0));
    default:
        break;
    }
}

Primitive* chair()
{
    Primitive *central = Block(2, 1, 1, RGBColor(0.5f,0,0));
    Primitive *front = MoveThis(Block(2, 1, 1, RGBColor(0.5f, 0, 0)), FRONT, 1);
    Primitive *top = MoveThis(Block(2, 1, 1, RGBColor(0.5f, 0, 0)), TOP, 1);
    //KDTree *grp = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    SimpleGroup* grp = new SimpleGroup();
    grp->add(central);
    grp->add(front);
    grp->add(top);
    grp->rebuildIndex();
    return grp;
}
Primitive* stair()
{
    Primitive* center = Block(2, 1, 1, RGBColor(0, 0, 0.5f));
    return center;
}

Primitive* seatRow(int numColumns, std::vector<Primitive*> &Chars)
{
    SimpleGroup* grp = new SimpleGroup();
    for (int i = 0; i < numColumns; ++i)
    {
        Primitive* emptySpace1 = MoveThis(MoveThis(stair(), BEHIND, 1), LEFT, i * 4);
        Primitive* emptySpace2 = MoveThis(MoveThis(stair(), BEHIND, 1), LEFT, i * 4 + 2);
        Primitive* stair1 = MoveThis(stair(), LEFT, i * 4);
        Primitive *Man = Chars.back();
        Chars.pop_back();
        if (Man != nullptr)
        {
            Man = MoveThis(MoveThis(MoveThis(Man, TOP, 1), FRONT, 1), LEFT, i * 4 * 2 + 2);
            grp->add(Man);
        }
        Primitive* chair1 = MoveThis(chair(), LEFT, i * 4 + 2);
        grp->add(stair1);
        grp->add(chair1);
        grp->add(emptySpace1);
        grp->add(emptySpace2);
    }
    grp->rebuildIndex();
    return grp;
}

Primitive* seatRowOnPlatform(int numColumns, int numPlatforms, std::vector<Primitive*> &Chars)
{
    SimpleGroup* grp = new SimpleGroup();
    for (int j = 0; j < numPlatforms; ++j)
        for (int i = 0; i < numColumns; ++i)
        {
            Primitive* emptySpace1 = MoveThis(MoveThis(stair(), BEHIND, 1), LEFT, i * 4);
            Primitive* emptySpace2 = MoveThis(MoveThis(stair(), BEHIND, 1), LEFT, i * 4 + 2);
            Primitive* emptySpace3 = MoveThis(MoveThis(stair(), FRONT, 1), LEFT, i * 4);
            Primitive* emptySpace4 = MoveThis(MoveThis(stair(), FRONT, 1), LEFT, i * 4 + 2);
            Primitive* emptySpace5 = MoveThis(stair(), LEFT, i * 4);
            Primitive* emptySpace6 = MoveThis(stair(), LEFT, i * 4 + 2);

            grp->add(MoveThis(emptySpace1, TOP, j));
            grp->add(MoveThis(emptySpace2, TOP, j));
            grp->add(MoveThis(emptySpace3, TOP, j));
            grp->add(MoveThis(emptySpace4, TOP, j));
            grp->add(MoveThis(emptySpace5, TOP, j));
            grp->add(MoveThis(emptySpace6, TOP, j));
        }

    grp->add(MoveThis(seatRow(numColumns, Chars), TOP, numPlatforms));
    grp->rebuildIndex();
    return grp;
}


Primitive* LegoModel()
{
    KDTree* Legos = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    MatLib* matlib = new MatLib();
    loadOBJMat(matlib, "competition/", "lego_poeople_obj.mtl");
    loadOBJ(Legos, "competition/", "legos.obj", matlib);

    Legos->rebuildIndex();
    return Legos;
}

Primitive* Char1Model()
{
    KDTree* Legos = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    MatLib* matlib = new MatLib();
    loadOBJMat(matlib, "competition/", "Character1.mtl");
    loadOBJ(Legos, "competition/", "Character1.obj", matlib);

    Legos->rebuildIndex();
    return Legos;
}

Primitive* Char2Model()
{
    KDTree* Legos = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    MatLib* matlib = new MatLib();
    loadOBJMat(matlib, "competition/", "Character2.mtl");
    loadOBJ(Legos, "competition/", "Character2.obj", matlib);

    Legos->rebuildIndex();
    return Legos;
}

Primitive* Char3Model()
{
    KDTree* Legos = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    MatLib* matlib = new MatLib();
    loadOBJMat(matlib, "competition/", "Character3.mtl");
    loadOBJ(Legos, "competition/", "Character3.obj", matlib);

    Legos->rebuildIndex();
    return Legos;
}

Primitive* Char4Model()
{
    KDTree* Legos = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    MatLib* matlib = new MatLib();
    loadOBJMat(matlib, "competition/", "Character4.mtl");
    loadOBJ(Legos, "competition/", "Character4.obj", matlib);

    Legos->rebuildIndex();
    return Legos;
}

Primitive* TVModel()
{
    KDTree* TV = new KDTree(std::vector<Primitive*>(), 1, 1, 1, 1, 10);
    MatLib* matlib = new MatLib();
    loadOBJMat(matlib, "competition/", "TVFlat.mtl");
    loadOBJ(TV, "competition/", "TVFlat.obj", matlib);
    ImageTexture* screenTex = new ImageTexture("competition/screen.png");
    
    Quad *screen = new Quad(Point(-4.5, -3, -4.3), Vector(0, 8, 0),
        Vector(8.5, 0, 0), new PlaneCoordMapper(Point(-4, -2, -8),Vector(8.5f, 0, 0),
    Vector(0, -8, 0)),
    new LambertianMaterial(new ConstantTexture(RGBColor(0, 0, 0)), screenTex));
    TV->rebuildIndex();
    SimpleGroup *grp = new SimpleGroup();
    grp->add(TV);
    grp->add(screen);
    grp->rebuildIndex();
    Instance* ins = new Instance(grp);
    //ins->rotate(Vector(0, 1, 0), -pi / 3 *2);
    //ins->scale(Vector(3, 3, 3));
    //ins->translate(Vector(0, 1, 0));
    return ins;
}

Primitive* TVSet()
{
    SimpleGroup *grp = new SimpleGroup();
    Primitive *base = MoveThis(MoveThis(Block(8, 3, 3, RGBColor(0, 0.5f, 0)), LEFT, 7), FRONT, 1);
    Primitive *TV = MoveThis(MoveThis(TVModel(), TOP, 4), LEFT, 4);
    grp->add(base);
    grp->add(TV);
    grp->rebuildIndex();
    return grp;
}

Primitive* Seats()
{
    SimpleGroup* grp = new SimpleGroup();
    std::vector<Primitive*> Chars;
    Chars.push_back(nullptr);
    Chars.push_back(nullptr);
    Chars.push_back(Char2Model());
    Chars.push_back(Char3Model());
    Chars.push_back(nullptr);
    Chars.push_back(nullptr);
    Chars.push_back(Char4Model());
    Chars.push_back(nullptr);
    grp->add(seatRowOnPlatform(4, 1, Chars));
    grp->add(MoveThis(seatRowOnPlatform(4, 2, Chars), BEHIND, 3));
    
    return grp;
    
}
Primitive* Floor()
{
    MirrorMaterial *floorMirror = new MirrorMaterial(0.2f, 0.2f);
    ImageTexture *floorTex = new ImageTexture("competition/checkerboard.png");
    ConstantTexture *floorConst = new ConstantTexture(RGBColor(1.0f, 0.9921f, 0.8156f));
    LambertianMaterial *floorLamb = new LambertianMaterial(new ConstantTexture(RGBColor(0.0f, 0, 0)), floorConst);
    CombineMaterial *floorCombine = new CombineMaterial();
    floorCombine->add(floorLamb, 0.5f);
    floorCombine->add(floorMirror, 0.5f);
    //Quad *floor1 = new Quad(Point(-100, 0, -100), Vector(100, 0, 0), Vector(0, 0, 100),
    //    new PlaneCoordMapper(Vector(5, 0, 0), Vector(0, 0, 5)), floorCombine);
    InfinitePlane *floor1 = new InfinitePlane(Point(0, 0, 0), Vector(0, 1, 0), nullptr, floorCombine);
    return floor1;

}

Primitive *Theater()
{
    SimpleGroup *grp = new SimpleGroup();
    grp->add(Seats());
    grp->add(MoveThis(TVSet(), FRONT, 15));
    grp->rebuildIndex();
    Instance *ins = new Instance(grp);
    ins->rotate(Vector(0, 1, 0), -pi / 2 - pi / 3);
    ins->translate(Vector(-6,0,6));
    return ins;
}

Primitive *Background()
{
    RGBColor c1(0.5f, 0, 0);
    RGBColor c2(0.0f, 0.5f, 0);
    RGBColor c3(0.0f, 0, 0.5f);
    RGBColor c4(0.5f, 0.5f, 0);
    RGBColor c5(0.0f, 0.5f, 0.5f);
    RGBColor c6(0.5f, 0, 0.5f);

    SimpleGroup *grp = new SimpleGroup();
    Instance *inst1 = new Instance(Block(2, 2, 1, c1));
    inst1->rotate(Vector(0.5f, 0.5f, 0), pi);
    inst1->translate(Vector(7.0f, 0, 0));
    Instance *inst2 = new Instance(Block(2, 2, 1, c2));
    inst2->rotate(Vector(0, 1, 0), -pi*0.2f);
    inst2->translate(Vector(0.0f, 0, -4.0f));
    Instance *inst3 = new Instance(Block(5, 1, 1, c3));
    inst3->rotate(Vector(0, 1, 0), -pi * 0.4f);
    inst3->translate(Vector(0, 0, 2.0f));
    Instance *inst4 = new Instance(Block(4, 1, 1, c4));
    inst4->rotate(Vector(1, 0, 0), pi * 0.2f);
    inst4->translate(Vector(0.0f, 3, -9.0f));
    Primitive *prim1 = MoveThis(Block(3, 2, 1, Brown), TOP, 1);
    Primitive *prim2 = Block(3, 2, 1, c6);
    Primitive *prim3 = MoveThis(Block(3, 2, 1, Gray), FRONT, 2);
    SimpleGroup *block1 = new SimpleGroup();
    block1->add(prim1);
    block1->add(prim2);
    block1->add(prim3);
    block1->rebuildIndex();
    Instance *inst5 = new Instance(block1);
    inst5->translate(Vector(7.0f, 1.5f, 8.0f));
    Primitive *Single1 = Block(1, 1, 1, Blue);
    Instance *inst6 = new Instance(Single1);
    inst6->translate(Vector(18.5f, 1.5f, 11.0f));
    Primitive *Single2 = Block(1, 1, 1, Blue);
    Instance *inst7 = new Instance(Single2);
    inst7->translate(Vector(17.3f, 1.5f, 13.0f));
    Primitive *Single3 = Block(1, 1, 1, Orange);
    Instance *inst8 = new Instance(Single3);
    inst8->translate(Vector(15.9f, 1.5f, 14.0f));
    Primitive *Single4 = Block(1, 1, 1, Orange);
    Instance *inst9 = new Instance(Single4);
    inst9->translate(Vector(14.4f, 1.5f, 8.0f));
    Primitive *Single5 = Block(1, 1, 1, Orange);
    Instance *inst10 = new Instance(Single5);
    inst10->translate(Vector(17.1f, 1.5f, 7.0f));

    //Instance *inst5 = new Instance(Block(2, 2, 1, c5));
    //Instance *inst6 = new Instance(Block(2, 2, 1, c6));

    grp->add(inst1);
    grp->add(inst2);
    grp->add(inst3);
    grp->add(inst4);
    //grp->add(inst5);
    //grp->add(inst6);
    //grp->add(inst7);
    //grp->add(inst8);
    //grp->add(inst9);
    //grp->add(inst10);
    //grp->add(inst4);
    //grp->add(inst5);
    //grp->add(inst6);

    grp->rebuildIndex();
    Instance *inst = new Instance(grp);
    inst->translate(Vector(-80, 0, 10));
    return inst;
}

Primitive *Scene()
{
    SimpleGroup *grp = new SimpleGroup();
    grp->add(Background());
    grp->add(Theater());
    grp->add(Floor());
    grp->rebuildIndex();
    return grp;
}

Primitive* ManOnChair()
{
    Primitive* Chair = chair();
    Primitive* Man = MoveThis(MoveThis(Char4Model(), TOP, 1),FRONT,1);
    SimpleGroup* grp = new SimpleGroup();
    grp->add(Chair);
    grp->add(Man);
    grp->rebuildIndex();
    return grp;

}

void a_competition()
{
    up = up.normalize();
    fwd = fwd.normalize();
    Image img(320 * 4.0f, 240 * 4.0f);
    //Image img(320 * 1.0f, 240 * 1.0f);
    Point camCenter = Point(120.0, 70.0f, -0.0f);
    //Point camCenter = Point(0.0f, 70.0f, -120.0f);
    //PerspectiveCamera cam1(camCenter, fwd, up, pi / 8, pi / 6);
    DOFPerspectiveCamera cam1(camCenter, fwd, up, pi/8, pi/6, 100.025f, 0.385f);

    DirectionalLight light1(Vector(-2, -3, 3), RGBColor(1.0f, 1.0f, 1.0f)*5);
    DirectionalLight light2(Vector(-2, -3, -3), RGBColor(1.0f, 1.0f, 1.0f)*5);
    World world;
    world.scene = Scene();// Theater();// Block(2, 2);// MoveThis(Block(2, 2), Block(2, 2), LEFT);
    //world.scene = Background();
    world.light.push_back(&light1);
    world.light.push_back(&light2);
    
    RayTracingIntegrator integrator(&world);
    //RayCastingIntegrator integrator(&world);
    Renderer engine1(&cam1, &integrator);
    engine1.setSamples(30);
    //engine1.setSamples(1);
    engine1.render(img);
    img.writePNG("competition.png");

}