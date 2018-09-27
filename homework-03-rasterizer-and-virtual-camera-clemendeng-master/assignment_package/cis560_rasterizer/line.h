#ifndef LINE_H
#define LINE_H

#include <glm/glm.hpp>

class Line
{
private:
    //vec4 so can easily use with Vertex class
    bool vertical;
    float slope;
public:
    glm::vec4 a, b;
    Line();
    Line(glm::vec4 a, glm::vec4 b);

    bool intersect(float y, float* x_intersection);
};

#endif // LINE_H
