#include "rasterizer.h"
#include "line.h"

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons)
{}

QImage Rasterizer::RenderScene()
{
    // TODO: Complete the various components of code that make up this function
    // It should return the rasterized image of the current scene

    // Notice that even though it should return a QImage this function compiles.
    // Remember, C++'s compiler is dumb (though it will at least warn you that
    // the function doesn't return anything).
    // BEWARE! If you use a function that is missing its return statement,
    // it will return garbage memory!
    QImage Q = QImage(512, 512, QImage::Format_RGB32);
    Q.fill(qRgb(0,0,0));
    glm::vec4 v1(0.f);
    glm::vec4 v2(0.f);
    glm::vec4 v3(0.f);
    float x1 = 0.f;
    float x2 = 0.f;
    float x3 = 0.f;
    for(int y = 1; y < 513; y++) {
        for(Polygon p : m_polygons) {
            for(Triangle t : p.m_tris) {
                glm::vec4 b = p.bounding_box(t);
                if(b[2] > y && b[3] < y) {
                    v1 = p.m_verts[t.m_indices[0]].m_pos;
                    v2 = p.m_verts[t.m_indices[1]].m_pos;
                    v3 = p.m_verts[t.m_indices[2]].m_pos;
                    Line l1 = Line(v1, v2);
                    Line l2 = Line(v1, v3);
                    Line l3 = Line(v2, v3);
                    if(l1.intersect(y, &x1)) {
                        if(!l2.intersect(y, &x2)) {
                            l3.intersect(y, &x2);
                        } else if(l3.intersect(y, &x3)) {
                            if(x1 == x2) {
                                x2 = x3;
                            }
                        }
                    } else {
                        l2.intersect(y, &x1);
                        l3.intersect(y, &x2);
                    }
                    if(x1 > x2) {
                        x3 = x1;
                        x1 = x2;
                        x2 = x3;
                    }
                    for(int i = x1; i <= x2; i++) {
                        glm::vec3 b = p.barycentric(t, i, y);
                        glm::vec3 c0 = p.m_verts[t.m_indices[0]].m_color;
                        glm::vec3 c1 = p.m_verts[t.m_indices[1]].m_color;
                        glm::vec3 c2 = p.m_verts[t.m_indices[2]].m_color;
                        glm::vec3 c = c0 * b[0] + c1 * b[1] + c2 * b[2];
                        Q.setPixel(i, y, qRgb(c[0],c[1],c[2]));
                    }
                }
            }
        }
    }
    return Q;
}

void Rasterizer::ClearScene()
{
    m_polygons.clear();
}
