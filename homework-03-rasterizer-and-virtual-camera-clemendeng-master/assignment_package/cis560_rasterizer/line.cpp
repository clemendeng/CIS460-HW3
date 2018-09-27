#include "line.h"

float calc(bool v, float n, float d) {
    if(!v) {
        return n / d;
    }
    return 0;
}

Line::Line()
{}

Line::Line(glm::vec4 a, glm::vec4 b) : a(a), b(b), vertical(b[0] - a[0]==0),
    slope(calc(vertical, (b[1] - a[1]), (b[0] - a[0])))
{}

bool Line::intersect(float y, float *x_intersection) {
    if((y < a[1] && y < b[1]) || (y > a[1] && y > b[1])) {
        return false;
    }
    if(vertical == true) {
        *x_intersection = a[0];
        return true;
    } else if(slope == 0){
        return false;
    } else {
        float intercept = a[1] - slope * a[0];
        *x_intersection = (y - intercept) / slope;
        return true;
    }
}
