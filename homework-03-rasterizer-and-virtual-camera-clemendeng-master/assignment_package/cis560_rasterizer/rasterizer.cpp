#include "rasterizer.h"
#include "line.h"

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons), frame(false), camera(Camera())
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
    int width = 512, height = 512;
    QImage Q = QImage(width, height, QImage::Format_RGB32);
    Q.fill(qRgb(0,0,0));
    float z[width * height] = {};
    for(int i = 0; i < width * height; i++) {
        z[i] = 1000;
    }

    //3D camera transformations
    std::vector<Polygon> polygons = m_polygons;
    for(Polygon& p : polygons) {
        for(Vertex& v : p.m_verts) {
            v.m_pos = camera.view() * v.m_pos;
            v.m_pos = camera.proj() * v.m_pos;
            v.m_pos = v.m_pos / v.m_pos[3];
            v.m_pos[0] = v.m_pos[0] * 255 + 256;
            v.m_pos[1] = v.m_pos[1] * 255 + 256;
        }
    }

    //Rendering
    glm::vec4 v1(0.f), v2(0.f), v3(0.f), n0(0.f), n1(0.f), n2(0.f), np(0.f);
    float x1 = 0.f, x2 = 0.f, x3 = 0.f, z0 = 0.f, z1 = 0.f, z2 = 0.f, zp = 0.f, energy = 0.f;
    for(int y = 0; y < 512; y++) {
        for(Polygon p : polygons) {
            for(Triangle t : p.m_tris) {
                glm::vec4 b = p.bounding_box(t);
                b = glm::clamp(b, glm::vec4(0.f), glm::vec4(511.f));
                if(b[2] > y && b[3] < y) {
                    v1 = p.m_verts[t.m_indices[0]].m_pos;
                    v2 = p.m_verts[t.m_indices[1]].m_pos;
                    v3 = p.m_verts[t.m_indices[2]].m_pos;
                    Line l1 = Line(v1, v2);
                    Line l2 = Line(v1, v3);
                    Line l3 = Line(v2, v3);
                    if(frame == false) {
                        //Finding x intersections
                        if(l1.intersect(y, &x1) && x1 >= b[0] && x1 <= b[1]) {
                            if(l2.intersect(y, &x2) && x2 >= b[0] && x2 <= b[1]) {
                                if(l3.intersect(y, &x3)) {
                                    if(glm::abs(x1 - x2) < 0.001) {
                                        x2 = x3;
                                    }
                                }
                            } else {
                                l3.intersect(y, &x2);
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
                        //Rendering
                        x1 = glm::clamp(x1, glm::max(0.f, b[0]), glm::min(511.f, b[1]));
                        x2 = glm::clamp(x2, glm::max(0.f, b[0]), glm::min(511.f, b[1]));
                        if(!(glm::abs(x1 - x2) < 0.001)) {
                            for(int i = glm::ceil(x1); i <= glm::floor(x2); i++) {
                                glm::vec3 bary = p.barycentric(t, i, y);
                                //Z buffering
                                z0 = p.m_verts[t.m_indices[0]].m_pos[2];
                                z1 = p.m_verts[t.m_indices[1]].m_pos[2];
                                z2 = p.m_verts[t.m_indices[2]].m_pos[2];
                                zp = 1.0f / (bary[0] / z0 + bary[1] / z1 + bary[2] / z2);
                                if(z[i + width * y] > zp && zp > 0) {
                                    z[i + width * y] = zp;
                                    //Lambertian reflection
                                    n0 = p.m_verts[t.m_indices[0]].m_normal;
                                    n1 = p.m_verts[t.m_indices[1]].m_normal;
                                    n2 = p.m_verts[t.m_indices[2]].m_normal;
                                    np = glm::normalize(zp * ((bary[0] / z0) * n0 + (bary[1] / z1) * n1 + (bary[2] / z2) * n2));
                                    energy = glm::clamp(glm::dot(np, -glm::normalize(camera.dir())), 0.2f, 1.f);
                                    //Coloring
                                    glm::vec2 p_uv = p.m_verts[t.m_indices[0]].m_uv * bary[0] +
                                            p.m_verts[t.m_indices[1]].m_uv * bary[1] + p.m_verts[t.m_indices[2]].m_uv * bary[2];
                                    p_uv = glm::clamp(p_uv, glm::vec2(0), glm::vec2(1));
                                    glm::vec3 c = GetImageColor(p_uv, p.mp_texture);
                                    c = glm::clamp(energy * c, glm::vec3(0.f), glm::vec3(255.f));
                                    Q.setPixel(i, y, qRgb(c[0], c[1], c[2]));
                                }
                            }
                        }
                    } else {
                        //Wireframe rendering
                        //Finding x intersections
                        l1.intersect(y, &x1);
                        l2.intersect(y, &x2);
                        l3.intersect(y, &x3);
                        //If y overlaps a line, draw the line
                        if(x1 == -1.f || x2 == -1.f || x3 == -1.f) {
                            Line l;
                            if(x1 == -1.f) {
                                l = l1;
                            } else if(x2 == -1.f) {
                                l = l2;
                            } else {
                                l = l3;
                            }
                            if(l.a[0] > l.b[0]) {
                                glm::vec4 temp = l.a;
                                l.a = l.b;
                                l.b = temp;
                            }
                            l.a[0] = glm::clamp(l.a[0], glm::max(0.f, b[0]), glm::min(511.f, b[1]));
                            l.b[0] = glm::clamp(l.b[0], glm::max(0.f, b[0]), glm::min(511.f, b[1]));
                            for(int i = glm::floor(l.a[0]); i <= glm::ceil(l.b[0]); i++) {
                                glm::vec3 bary = p.barycentric2(t, l, i, y);
                                //Z buffering
                                z0 = p.m_verts[t.m_indices[(int) bary[0]]].m_pos[2];
                                z1 = p.m_verts[t.m_indices[(int) bary[1]]].m_pos[2];
                                zp = z0 + (z1 - z0) * bary[2];
                                if(z[i + width * y] > zp && zp > 0) {
                                    z[i + width * y] = zp;
                                    //Coloring
                                    glm::vec2 uv0 = p.m_verts[t.m_indices[(int) bary[0]]].m_uv;
                                    glm::vec2 uv1 = p.m_verts[t.m_indices[(int) bary[1]]].m_uv;
                                    glm::vec2 p_uv = uv0 + (uv1 - uv0) * bary[2];
                                    p_uv = glm::clamp(p_uv, glm::vec2(0), glm::vec2(1));
                                    glm::vec3 c = GetImageColor(p_uv, p.mp_texture);
                                    c = glm::clamp(c, glm::vec3(0.f), glm::vec3(255.f));
                                    Q.setPixel(i, y, qRgb(c[0], c[1], c[2]));
                                }
                            }
                        } else {
                            //Otherwise, draw the points of intersection of the two lines y intersects
                            Line la, lb;
                            if(l1.intersect(y, &x1) && x1 >= b[0] && x1 <= b[1]) {
                                la = la;
                                if(l2.intersect(y, &x2) && x2 >= b[0] && x2 <= b[1]) {
                                    lb = l2;
                                    if(l3.intersect(y, &x3)) {
                                        if(glm::abs(x1 - x2) < 0.001) {
                                            x2 = x3;
                                            lb = l3;
                                        }
                                    }
                                } else {
                                    l3.intersect(y, &x2);
                                    lb = l3;
                                }
                            } else {
                                l2.intersect(y, &x1);
                                l3.intersect(y, &x2);
                                la = l2;
                                lb = l3;
                            }
                            if(x1 > x2) {
                                x3 = x1;
                                x1 = x2;
                                x2 = x3;
                                Line temp = la;
                                la = lb;
                                lb = temp;
                            }
                            //Rendering
                            if(glm::abs(x1 - glm::clamp(x1, glm::max(0.f, b[0]), glm::min(511.f, b[1]))) < 0.001) {
                                glm::vec3 bary = p.barycentric2(t, la, x1, y);
                                //Z buffering
                                z0 = p.m_verts[t.m_indices[(int) bary[0]]].m_pos[2];
                                z1 = p.m_verts[t.m_indices[(int) bary[1]]].m_pos[2];
                                zp = z0 + (z1 - z0) * bary[2];
                                if(z[(int) x1 + width * y] > zp && zp > 0) {
                                    z[(int) x1 + width * y] = zp;
                                    //Coloring
                                    glm::vec2 uv0 = p.m_verts[t.m_indices[(int) bary[0]]].m_uv;
                                    glm::vec2 uv1 = p.m_verts[t.m_indices[(int) bary[1]]].m_uv;
                                    glm::vec2 p_uv = uv0 + (uv1 - uv0) * bary[2];
                                    p_uv = glm::clamp(p_uv, glm::vec2(0), glm::vec2(1));
                                    glm::vec3 c = GetImageColor(p_uv, p.mp_texture);
                                    c = glm::clamp(c, glm::vec3(0.f), glm::vec3(255.f));
                                    Q.setPixel(x1, y, qRgb(c[0], c[1], c[2]));
                                }
                            }
                            if(glm::abs(x2 - glm::clamp(x2, glm::max(0.f, b[0]), glm::min(511.f, b[1]))) < 0.001) {
                                glm::vec3 bary = p.barycentric2(t, la, x2, y);
                                //Z buffering
                                z0 = p.m_verts[t.m_indices[(int) bary[0]]].m_pos[2];
                                z1 = p.m_verts[t.m_indices[(int) bary[1]]].m_pos[2];
                                zp = z0 + (z1 - z0) * bary[2];
                                if(z[(int) x2 + width * y] > zp && zp > 0) {
                                    z[(int) x2 + width * y] = zp;
                                    //Coloring
                                    glm::vec2 uv0 = p.m_verts[t.m_indices[(int) bary[0]]].m_uv;
                                    glm::vec2 uv1 = p.m_verts[t.m_indices[(int) bary[1]]].m_uv;
                                    glm::vec2 p_uv = uv0 + (uv1 - uv0) * bary[2];
                                    p_uv = glm::clamp(p_uv, glm::vec2(0), glm::vec2(1));
                                    glm::vec3 c = GetImageColor(p_uv, p.mp_texture);
                                    c = glm::clamp(c, glm::vec3(0.f), glm::vec3(255.f));
                                    Q.setPixel(x2, y, qRgb(c[0], c[1], c[2]));
                                }
                            }
                        }
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
