#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QXmlStreamReader>
#include <QFileDialog>
#include <raytracing/samplers/randompixelsampler.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <raytracing/samplers/uniformpixelsampler.h>
#include <tbb/tbb.h>
#include <QTime>


using namespace tbb;

MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent)
{
    setFocusPolicy(Qt::ClickFocus);
}

MyGL::~MyGL()
{
    makeCurrent();

    vao.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    vao.create();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat-color shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    vao.bind();

    //Test scene data initialization
    scene.CreateTestScene();
    integrator.scene = &scene;
    integrator.intersection_engine = &intersection_engine;
    intersection_engine.scene = &scene;
    ResizeToSceneCamera();

}

void MyGL::resizeGL(int w, int h)
{
    gl_camera = Camera(w, h);

    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the projection matrix
    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function updateGL is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the viewproj matrix
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());
    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    GLDrawScene();
}

void MyGL::GLDrawScene()
{
    for(Geometry *g : scene.objects)
    {
        if(g->drawMode() == GL_TRIANGLES)
        {
            prog_lambert.setModelMatrix(g->transform.T());
            prog_lambert.draw(*this, *g);
        }
        else if(g->drawMode() == GL_LINES)
        {
            prog_flat.setModelMatrix(g->transform.T());
            prog_flat.draw(*this, *g);
        }
        DrawBBX_mesh(g->GetRoot());
    }
    for(Geometry *l : scene.lights)
    {
        prog_flat.setModelMatrix(l->transform.T());
        prog_flat.draw(*this, *l);
    }
    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(*this, scene.camera);
    DrawBBX(&Root);
}

void MyGL::DrawBBX(BVHNode *start_pt)
{
    if (start_pt == NULL) return;

    prog_flat.draw(*this , start_pt->BBX);

    if(start_pt->left_child != NULL)
    {
        DrawBBX(start_pt->left_child);
        //prog_flat.draw(*this, start_pt->left_child->BBX);
    }

    if(start_pt->right_child != NULL)
    {
        DrawBBX(start_pt->right_child);
        //prog_flat.draw(*this, start_pt->right_child->BBX);
    }

}

void MyGL::DrawBBX_mesh(BVHNode *start_pt)
{
    if (start_pt == NULL) return;

    prog_flat.draw(*this , start_pt->BBX_world);

    if(start_pt->left_child != NULL)
    {
        DrawBBX_mesh(start_pt->left_child);
        //prog_flat.draw(*this, start_pt->left_child->BBX);
    }

    if(start_pt->right_child != NULL)
    {
        DrawBBX_mesh(start_pt->right_child);
        //prog_flat.draw(*this, start_pt->right_child->BBX);
    }

}

void MyGL::ResizeToSceneCamera()
{
    this->setFixedWidth(scene.camera.width);
    this->setFixedHeight(scene.camera.height);
//    if(scene.camera.aspect <= 618/432)
//    {
//        this->setFixedWidth(432 / scene.camera.aspect);
//        this->setFixedHeight(432);
//    }
//    else
//    {
//        this->setFixedWidth(618);
//        this->setFixedHeight(618 * scene.camera.aspect);
//    }
    gl_camera = Camera(scene.camera);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_F) {
        gl_camera.CopyAttributes(scene.camera);
    } else if (e->key() == Qt::Key_R) {
        scene.camera = Camera(gl_camera);
        scene.camera.recreate();
    }
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::SceneLoadDialog()
{
    QString filepath = QFileDialog::getOpenFileName(0, QString("Load Scene"), QString("../scene_files"), tr("*.xml"));
    if(filepath.length() == 0)
    {
        return;
    }

    QFile file(filepath);
    int i = filepath.length() - 1;
    while(QString::compare(filepath.at(i), QChar('/')) != 0)
    {
        i--;
    }
    QStringRef local_path = filepath.leftRef(i+1);
    //Reset all of our objects
    scene.Clear();
    integrator = Integrator();
    intersection_engine = IntersectionEngine();
    //Load new objects based on the XML file chosen.
    xml_reader.LoadSceneFromFile(file, local_path, scene, integrator);
    integrator.scene = &scene;
    integrator.intersection_engine = &intersection_engine;
    intersection_engine.scene = &scene;

    Root.Clear();
    Root = BVHNode(scene.objects, 0);
    Root.BBX.create();
    intersection_engine.root = &Root;

}


void MyGL::RaytraceScene()
{
    QString filepath = QFileDialog::getSaveFileName(0, QString("Save Image"), QString("../rendered_images"), tr("*.bmp"));
    int num_samples = 2;
    RandomPixelSampler sampler(num_samples);
    QList<glm::vec2> samples;
    glm::vec3 color(0, 0, 0);
    //coordinates for super sampling
    float x_super;
    float y_super;


    if(filepath.length() == 0)
    {
        return;
    }
    //#define TBB //Uncomment this line out to render your scene with multiple threads.
    //This is useful when debugging your raytracer with breakpoints.

    #ifdef TBB
        parallel_for(0, (int)scene.camera.width, 1, [=](unsigned int i)
        {
            for(unsigned int i = 0; i < scene.camera.width; i++)
            {
                for(unsigned int j = 0; j < scene.camera.height; j++)
                {

                    Ray r(gl_camera.Raycast(i, j));
                    //Intersection XX(intersection_engine.GetIntersection(r));
                    glm::vec3 color(integrator.TraceRay(r, 0));
                    scene.film.pixels[i][j] = color;

                    if (XX.object_hit == NULL)
                    {
                        scene.film.pixels[i][j] = glm::vec3(0, 0, 0);
                    }
                    else
                    {
                        scene.film.pixels[i][j] = glm::abs(XX.rgb);
                    }

                }
            }
        });

    #else
        //timer1.start(1);
        QTime timer2;
        timer2.start();
        for(unsigned int i = 0; i < scene.camera.width; i++)
        {
            for(unsigned int j = 0; j < scene.camera.height; j++)
            {
                samples = sampler.GetSamples(i, j); //super sample
                color = glm::vec3(0, 0, 0);

                for (int k = 0; k < samples.count(); k++)
                {
                    x_super = samples[k].x;
                    y_super = samples[k].y;
                    if(i == 32 && j == 220)
                    {
                        int r = 0;
                    }
                    Ray r(scene.camera.Raycast(x_super, y_super));
                    //Intersection XX(intersection_engine.GetIntersection(r));
                    color = color + integrator.TraceRay(r, 0)/static_cast<float>(num_samples)/static_cast<float>(num_samples);

                    /*
                    if (XX.object_hit == NULL)
                    {
                        scene.film.pixels[i][j] = glm::vec3(0, 0, 0);
                    }
                    else
                    {
                        scene.film.pixels[i][j] = glm::abs(XX.rgb);
                    } */
                }

                scene.film.pixels[i][j] = color;
            }

        }
        qDebug() << "Render took" << timer2.elapsed() << "ms";
        //timer1.stop();
    #endif
    scene.film.WriteImage(filepath);
}
