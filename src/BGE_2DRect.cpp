#include "BGE_2DRect.h"

#include <BGE_2DVect.h>

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
