#ifndef BGE_OBJECTMATERIAL_H
#define BGE_OBJECTMATERIAL_H

#include <string>

class BGE_ObjectMaterial {
    public:
        enum class PhysicalState : int {
			HARD_SOLID,
			SOFT_SOLID,
			LIQUID,
			GAS,
			PLASMA,
			TOT
		};

        enum class Material : int {
			FLESH,
			BONE,
			IRON,
			PINEWOOD,
			MARBLE,
			FIBERGLASS,
			DRUG,
			WATER,
			FABRIC,
			TOT
		};

		Material material;
        PhysicalState state;

        BGE_ObjectMaterial();
        virtual ~BGE_ObjectMaterial();

        Material getMaterial();
        PhysicalState getState();
        float getDensity();
        float getToxicity();
        std::string getName();
        std::string getMaterialName();
        std::string getStateName();
    protected:
    private:
};

#endif // BGE_OBJECTMATERIAL_H
