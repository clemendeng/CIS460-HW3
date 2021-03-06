#pragma once
#include <polygon.h>
#include <camera.h>
#include <QImage>

class Rasterizer
{
private:
    //This is the set of Polygons loaded from a JSON scene file
    std::vector<Polygon> m_polygons;
public:
    Rasterizer(const std::vector<Polygon>& polygons);
    bool frame;
    Camera camera;
    QImage RenderScene();
    void ClearScene();
};
