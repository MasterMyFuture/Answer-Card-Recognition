#include<iostream>
#include <vector>



class Axis{
    private:
        std::vector<int> headIndex;
        std::vector<int> endIndex;


    public:
        void getShadowIndex(const std::vector<int> & shadowArray);

};
