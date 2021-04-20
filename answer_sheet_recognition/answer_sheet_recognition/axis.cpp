#include "axis.h"

void Axis::getShadowIndex(const std::vector<int> & shadowArray){
    size_t length = shadowArray.size();  //
	int upNums = 0, downNums = 0;
	for(int i = 1; i < length; i++)
	{
		if(shadowArray[i-1] == 0 && shadowArray[i] > 0)
		{
			this->headIndex.push_back(i);
			upNums ++;
		}
		else if(shadowArray[i-1] > 0 && shadowArray[i] == 0)
		{
			this->endIndex.push_back(i);
			downNums ++;
		}
		
	}
	if(shadowArray.back()!= 0){
		this->endIndex.push_back(shadowArray.size());
	}

}