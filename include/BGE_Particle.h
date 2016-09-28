#ifndef BGE_PARTICLE_H
#define BGE_PARTICLE_H

#include <BGE_Object.h>
#include <BGE_Moveable.h>


class BGE_Particle : public BGE_Moveable {
  public:
	BGE_Particle( Material material );
	virtual ~BGE_Particle();

	void update( float Dt);
	void interact( BGE_Object *other, BGE_2DVect overlap);

    void render();

  protected:
  private:
    void (BGE_Particle::* interactBehaviour)(BGE_Object*, BGE_2DVect);
};

#endif // BGE_PARTICLE_H
