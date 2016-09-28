#ifndef BGE_MOVEABLE_H
#define BGE_MOVEABLE_H

#include <BGE_2DVect.h>
#include <BGE_Object.h>

class BGE_Moveable : public BGE_Object {
  public:
    static const float SPEED_BOTTOM_LIMIT;  // [ps/S]

	BGE_Moveable(Type objectType, Material objMaterial);
	virtual ~BGE_Moveable();

	//Updates position and speed.
	virtual void update(float Dt);

	BGE_2DVect speed;   // [px/S]
	float damping;      // [1/S]
  protected:
    //The following are meant to be used by child classes in their interact() functions
    //and define different type of interactions.
    void wallBounce(BGE_Object* other, BGE_2DVect overlap);
    void elasticBounce(BGE_Moveable* other, BGE_2DVect overlap);
    void stick(BGE_Object* other, BGE_2DVect overlap);
  private:
};

#endif // BGE_MOVEABLE_H
