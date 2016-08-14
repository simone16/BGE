#include "BGE_2DRect.h"

#include <BGE_2DVect.h>

#include <cmath>

#include <stdio.h>

BGE_2DRect::BGE_2DRect() {
    x=0;
    y=0;
    w=0;
    h=0;
}

BGE_2DRect::BGE_2DRect( float _x, float _y, float _w, float _h) {
    x=_x;
    y=_y;
    w=_w;
    h=_h;
}

BGE_2DRect::~BGE_2DRect() {}

void BGE_2DRect::operator += ( const BGE_2DVect &offset) {
    x += offset.x;
    y += offset.y;
}

void BGE_2DRect::operator -= ( const BGE_2DVect &offset) {
    x -= offset.x;
    y -= offset.y;
}

bool BGE_2DRect::overlaps( BGE_2DRect &otherRect) {
    return !( x+w < otherRect.x ||
        y+h < otherRect.y ||
        otherRect.x+otherRect.w < x ||
        otherRect.y+otherRect.h < y );
}

bool BGE_2DRect::contains( BGE_2DRect &otherRect) {
    return !( otherRect.x < x ||
        otherRect.y < y ||
        x+w < otherRect.x+otherRect.w ||
        y+h < otherRect.y+otherRect.h );
}

BGE_2DRect BGE_2DRect::intersection( BGE_2DRect &otherRect ) {
    BGE_2DRect intersection = otherRect;
    if (otherRect.x < x) {
        intersection.w -= x-otherRect.x;
        intersection.x = x;
    }
    if (otherRect.x+otherRect.w > x+w) {
        intersection.w = x+w-intersection.x;
    }
    if (otherRect.y < y) {
        intersection.h -= y-otherRect.y;
        intersection.y = y;
    }
    if (otherRect.y+otherRect.h > y+h) {
        intersection.h = y+h-intersection.y;
    }
    return intersection;
}

bool BGE_2DRect::intersection(const BGE_2DVect& A, const BGE_2DVect& B, BGE_2DVect& intersectionA, BGE_2DVect& intersectionB ) {
    //Check rectangle is inside segment.
    if (A.x <= B.x) {
        if (x+w < A.x || x > B.x) {
            return false;
        }
    }
    else {
        if (x+w < B.x || x > A.x) {
            return false;
        }
    }
    if (A.y <= B.y) {
        if (y+h < A.y || y > B.y) {
            return false;
        }
    }
    else {
        if (y+h < B.y || y > A.y) {
            return false;
        }
    }
    //Compute intersection points.
    bool foundOne = false;
    //Segment is in the form y=mx+q;
    float m = (B.y-A.y)/(B.x-A.x);
    float q = B.y-m*B.x;
    if ((m*x+q) <= (y+h) && (m*x+q) >= y) {
        //Intersects left side.
        intersectionA.x = x;
        intersectionA.y = (m*x+q);
        foundOne = true;
    }
    if ((m*(x+w)+q) <= (y+h) && (m*(x+w)+q) >= y) {
        //Intersects right side.
        if (foundOne) {
            intersectionB.x = x+w;
            intersectionB.y = (m*(x+w)+q);
            return true;
        }
        else {
            intersectionA.x = x+w;
            intersectionA.y = (m*(x+w)+q);
            foundOne = true;
        }
    }
    if ((y-q)/m <= x+w && (y-q)/m >= x) {
        //Intersects top side.
        if (foundOne) {
            intersectionB.x = (y-q)/m;
            intersectionB.y = y;
            return true;
        }
        else {
            intersectionA.x = (y-q)/m;
            intersectionA.y = y;
            foundOne = true;
        }
    }
    if ((y+h-q)/m <= x+w && (y+h-q)/m >= x) {
        //Intersects bottom side.
        if (foundOne) {
            intersectionB.x = (y+h-q)/m;
            intersectionB.y = y+h;
            return true;
        }
        else {
            printf("Problem!! \n");
        }
    }
    return false;
}

float BGE_2DRect::diagonal() {
    return std::sqrt(w*w+h*h);
}
