#ifndef BGE_OBJECTMATERIAL_H
#define BGE_OBJECTMATERIAL_H

#include <string>

class BGE_ObjectMaterial {
    public:
        static const float LITERS_PER_CUBIC_PX;

        enum Material : int {
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

		enum class PhysicalState : int {
			HARD_SOLID,
			SOFT_SOLID,
			LIQUID,
			GAS,
			TOT
		};

		//Used to hold the values associated with each material.
        struct MaterialData {
            std::string name;
            PhysicalState state;
            float density;      // [kg/px3]
            float nutrition;     // [HP/kg] negative -> toxic.
        };

		Material materialCode;

        BGE_ObjectMaterial( Material _materialCode);
        virtual ~BGE_ObjectMaterial();

        //This function has to be called before using any object of this class.
        static void init();

        std::string getName();
        PhysicalState getState();
        float getDensity();
        float getNutrition();
    protected:
    private:
        static MaterialData dataOf[TOT];
};

#endif // BGE_OBJECTMATERIAL_H
