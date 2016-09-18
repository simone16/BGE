#include <BGE_2DVect.h>

#include <SDL2/SDL.h>
#include <cmath>

BGE_2DVect::BGE_2DVect() {
	x = 0;
	y = 0;
}
BGE_2DVect::BGE_2DVect( float nx, float ny ) {
	x = nx;
	y = ny;
}

BGE_2DVect::~BGE_2DVect() {}

void BGE_2DVect::setPolar( float modulus, float angle ) {
	x = std::cos( angle ) * modulus;
	y = std::sin( angle ) * modulus;
}

BGE_2DVect BGE_2DVect::operator+( const BGE_2DVect addendum ) const {
	BGE_2DVect result;
	result.x = x + addendum.x;
	result.y = y + addendum.y;
	return result;
}

BGE_2DVect BGE_2DVect::operator-( const BGE_2DVect addendum ) const {
	BGE_2DVect result;
	result.x = x - addendum.x;
	result.y = y - addendum.y;
	return result;
}

BGE_2DVect& BGE_2DVect::operator-() {
    x = -x;
    y = -y;
    return *this;
}

BGE_2DVect& BGE_2DVect::operator+=( const BGE_2DVect addendum ) {
    x += addendum.x;
    y += addendum.y;
    return *this;
}

BGE_2DVect& BGE_2DVect::operator-=( const BGE_2DVect addendum ) {
    x -= addendum.x;
    y -= addendum.y;
    return *this;
}

BGE_2DVect BGE_2DVect::operator*( const double factor ) const {
	BGE_2DVect result;
	result.x = x * factor;
	result.y = y * factor;
	return result;
}

bool BGE_2DVect::operator==(const BGE_2DVect refvect) const {
    return (x == refvect.x) && (y == refvect.y);
}

float BGE_2DVect::modulus() const {
	return std::sqrt( std::pow( x, 2 ) + std::pow( y, 2 ) );
}

float BGE_2DVect::angle() const {
	return std::atan2( y, x );
}

Uint8 BGE_2DVect::quadrant() const {
    //Quadrant assumes integer values depending on angle
	//    '.1.'
	//	  2 * 0
	//    .'3'.
    if (std::abs(x) >= std::abs(y)) {
        if (x >= 0) {
            return 0;
        }
        else {
            return 2;
        }
    }
    else {
        if (y >= 0) {
            return 3;
        }
        else {
            return 1;
        }
    }
}

BGE_2DVect::operator SDL_Point() const {
    SDL_Point sdl = { int(x), int(y)};
    return sdl;
}
