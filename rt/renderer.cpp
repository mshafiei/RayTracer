#include <rt/renderer.h>
#include <main/a_julia.h>
#include <core/transform.h>
#include <iostream>
#include <rt/cameras/camera.h>
#include <rt/cameras/fisheye.h>
#include <rt/integrators/integrator.h>
//#define USE_OPENCV
#ifdef USE_OPENCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#endif
#include <ctime>
#include <iostream>
#include <core/random.h>
#include <rt/solids/quad.h>

namespace rt
{   
    Renderer::Renderer(Camera* cam, Integrator* integrator) :samples(1)
    {
#ifdef USE_OPENCV
        cv::namedWindow("RenderedImage");
#endif
        this->cam = cam;
        this->integrator = integrator;
    }

    void Renderer::setSamples(uint samples)
    {
        this->samples = samples;
    }

    void Renderer::render(Image& img)
    {
        // Prepare a transformation matrix that will map from image
        // space to camera normalized space
        // a.k.a screen to raster transformation
        float halfWidth = (float)img.width() / 2.f;
        float halfHeight = (float)img.height() / 2.f;
        Transform trans = Translate(Vector(-halfWidth, -halfHeight, 0));
        Transform scale = Scale(1.f/halfWidth, -1.f/ halfHeight, 1);
        trans = scale * trans;
#ifdef USE_OPENCV  
        cv::Mat image(img.height(), img.width(), CV_32FC4, img.GetData());
#endif
        const long double sysTime = time(0);
        const long double sysTimeMS = sysTime * 1000;
#if 0
        int tx = img.width() / 2;
        int ty = img.height() / 2;
        Point ucc = trans(Point(tx, ty, 0));
        img[ty*img.width() + tx] = integrator->getRadiance(
            cam->getPrimaryRay(ucc.x, ucc.y));
#else        
       
#pragma  omp parallel for
        for (int y = 0; y < img.height(); y++)
        {
            float randX;
            float randY;
            Point ucc;
            Ray ray;
            RGBColor pixleColor;
            for (int x = 0; x < img.width(); x++)
            {
                std::cout << "x : " << x << " y : " << y << std::endl;
                pixleColor.r = pixleColor.g = pixleColor.b = 0.0f;

                if (samples > 1)
                {
                    for (int s = 0; s < samples; s++)
                    {
                        randX = x + random();
                        randY = y + random();
                        ucc = trans(Point(randX, randY, 0));
                        ray = cam->getPrimaryRay(ucc.x, ucc.y);
                        pixleColor = pixleColor + integrator->getRadiance(ray);
                    }

                    pixleColor = pixleColor / samples;
                }
                else
                {
                    // compute the uniform camera coordinates for the
                    // pixel
                    ucc = trans(Point(x, y, 0));
                    ray = cam->getPrimaryRay(ucc.x, ucc.y);
                    pixleColor = integrator->getRadiance(ray);
                }

                img[y*img.width() + x] = pixleColor;
                }
#ifdef USE_OPENCV
            cv::imshow("RenderedImage", image);
            cv::waitKey(1);
#endif
            }
#endif
        int tst = time(0) * 1000 - sysTimeMS;

        std::cout << "Took " << tst << " MS." << std::endl;
        
    }

    void Renderer::test_render1(Image& img)
    {
        for(int y=0 ; y<img.height() ; y++)
        {
            for(int x=0 ; x<img.width() ; x++)
            {
                img[y*img.width() + x] = a1computeColor(x, y, img.width(), img.height());
            }
        }
    }

    void Renderer::test_render2(Image& img)
    {
        // Prepare a transformation matrix that will map from image
        // space to camera normalized space
        float halfWidth = (float)img.width() / 2.f;
        float halfHeight = (float)img.height() / 2.f;
        Transform trans = Translate(Vector(-halfWidth, -halfHeight, 0));
        Transform scale = Scale(1.f/halfWidth, -1.f/ halfHeight, 1);
        trans = scale * trans;
        
        for(int y=0 ; y<img.height() ; y++)
        {
            for(int x=0 ; x<img.width() ; x++)
            {
                // compute the uniform camera coordinates for the
                // pixel
                Point ucc = trans(Point(x, y, 0));
                img[y*img.width() + x] = a2computeColor(
                    cam->getPrimaryRay(ucc.x, ucc.y));
            }
        }
    }

    void Renderer::test_render3(Image& img)
    {
        float halfWidth = (float)img.width() / 2.f;
        float halfHeight = (float)img.height() / 2.f;
        Transform trans = Translate(Vector(-halfWidth, -halfHeight, 0));
        Transform scale = Scale(1.f/halfWidth, -1.f/ halfHeight, 1);
        trans = scale * trans;
        
        for(int y=0 ; y<img.height() ; y++)
        {
            for(int x=0 ; x<img.width() ; x++)
            {
                // compute the uniform camera coordinates for the
                // pixel
                Point ucc = trans(Point(x, y, 0));
                Ray ray = cam->getPrimaryRay(ucc.x, ucc.y);
                if(ray.d.length() == 0)
                {
                    img[y*img.width() + x] = RGBColor(0, 0, 0);
                }
                else
                {
                    img[y*img.width() + x] = a2computeColor(ray);
                }
            }
        }
    }

    void Renderer::test_render4(Image& img)
    {
        for(int y=0 ; y<img.height() ; y++)
        {
            for(int x=0 ; x<img.width() ; x++)
            {
                // compute NDC coordinates for the
                // pixel
                float ndcX = (float)x / (float)img.width();
                float ndcY = (float)y / (float)img.height();
                img[y*img.width() + x] = a2computeColor(
                    cam->getPrimaryRay(ndcX, ndcY));
            }
        }
    }
    
    void Renderer::test_sampling(Image& img)
    {
        Quad testQuad(Point(0, 0, 0), Vector(img.width(), 0, 0),
                      Vector(0, img.height(), 0),
                      nullptr, nullptr);

        for(int s=0 ; s<10000 ; s++)
        {
            Point p = testQuad.sample();
            img[(int)p.x*img.width() + (int)p.y] = RGBColor(1, 1, 1);
        }
    }
    void Renderer::test_motionblur(Image& img)
    {
        // Prepare a transformation matrix that will map from image
        // space to camera normalized space
        // a.k.a screen to raster transformation
        float halfWidth = (float)img.width() / 2.f;
        float halfHeight = (float)img.height() / 2.f;
        Transform trans = Translate(Vector(-halfWidth, -halfHeight, 0));
        Transform scale = Scale(1.f / halfWidth, -1.f / halfHeight, 1);
        trans = scale * trans;
#ifdef USE_OPENCV  
        cv::Mat image(img.height(), img.width(), CV_32FC4, img.GetData());
#endif
        const long double sysTime = time(0);
        const long double sysTimeMS = sysTime * 1000;
#if 0
        int tx = img.width() / 2;
        int ty = img.height() / 2;
        Point ucc = trans(Point(tx, ty, 0));
        img[ty*img.width() + tx] = integrator->getRadiance(
            cam->getPrimaryRay(ucc.x, ucc.y));
#else        
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {

                for (int k = 0; k < 30; ++k)
                {
                    float t = random(0, 1);
                    RGBColor pixleColor;

                    if (samples > 1)
                    {
                        for (int s = 0; s < samples; s++)
                        {
                            float randX = x + random();
                            float randY = y + random();
                            Point ucc = trans(Point(randX, randY, 0));
                            Ray ray = cam->getPrimaryRay(ucc.x, ucc.y);
                            ray.setT(t);
                            pixleColor = pixleColor + integrator->getRadiance(ray);
                        }

                        pixleColor = pixleColor / samples;
                    }
                    else
                    {
                        // compute the uniform camera coordinates for the
                        // pixel
                        Point ucc = trans(Point(x, y, 0));
                        Ray ray = cam->getPrimaryRay(ucc.x, ucc.y);
                        ray.setT(t);
                        pixleColor = integrator->getRadiance(ray);
                    }

                    img[y*img.width() + x] = img[y*img.width() + x] + pixleColor;
                }
                img[y*img.width() + x] = img[y*img.width() + x] / 30;
            }
#ifdef USE_OPENCV
            cv::imshow("RenderedImage", image);
            cv::waitKey(1);
#endif
        }
#endif
        int tst = time(0) * 1000 - sysTimeMS;

        std::cout << "Took " << tst << " MS." << std::endl;

    }
}
