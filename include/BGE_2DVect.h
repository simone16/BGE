#ifndef BGE_2DVECT_H
#define BGE_2DVECT_H

#include <cstdint>

class SDL_Point;

class BGE_2DVect {
  public:
	float x;
	float y;

	BGE_2DVect();
	BGE_2DVect(float nx, float ny);
	virtual ~BGE_2DVect();

	void setPolar(float modulus, float angle);
	BGE_2DVect operator + ( const BGE_2DVect ) const;
	BGE_2DVect operator - ( const BGE_2DVect ) const;
	BGE_2DVect& operator - ();
	BGE_2DVect& operator += ( const BGE_2DVect );
	BGE_2DVect& operator -= ( const BGE_2DVect );
	BGE_2DVect operator * ( const double ) const;
	bool operator == ( const BGE_2DVect ) const;
	float modulus() const;
	float angle() const;
	//Quadrant assumes integer values depending on angle
	//    '.1.'
	//	  2 * 0
	//    .'3'.
	uint8_t quadrant() const;

	operator SDL_Point() const;
  protected:
  private:
};

#endif // BGE_2DVECT_H
