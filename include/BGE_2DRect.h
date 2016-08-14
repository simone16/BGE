#ifndef BGE_2DRECT_H
#define BGE_2DRECT_H

class BGE_2DVect;

class BGE_2DRect {
    public:
        //Upper left corner coordinate
        float x;
        float y;
        //Width and height
        float w;
        float h;

        BGE_2DRect();
        BGE_2DRect( float x, float y, float w, float h );
        virtual ~BGE_2DRect();

        void operator += (const BGE_2DVect &offset);
        void operator -= (const BGE_2DVect &offset);

        //Tells whether two rectangles overlap (intersect or include).
        bool overlaps( BGE_2DRect &otherRect );
        //Tells whether other is completely inside this.
        bool contains( BGE_2DRect &otherRect );
        //Returns intersection rectangle.
        BGE_2DRect intersection( BGE_2DRect &otherRect );
        //Returns intersection segment from segment AB~ (false if no intersection is found).
        bool intersection(const BGE_2DVect &A, const BGE_2DVect &B,  BGE_2DVect &intersectionA, BGE_2DVect &intersectionB );
        //Returns diagonal.
        float diagonal();
    protected:
    private:
};

#endif // BGE_2DRECT_H
