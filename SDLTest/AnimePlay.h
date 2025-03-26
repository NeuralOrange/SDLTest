#pragma once
#include <vector>
#include "Spirit.h"
#include"SpiritNode.h"
class AnimePlay
{
public:
	unsigned flashNum = 1;
	std::vector<unsigned> flashList;
	bool isLoop = false;
	unsigned animalCount = 0;
	bool flashStart = false;
	SpiritNode* spirit;
	AnimePlay(SpiritNode* spirit_, unsigned flash_num, bool isLoop_, std::vector<unsigned> flash_list ) 
	{
		spirit = spirit_;
		flashNum = flash_num;
		isLoop = isLoop_;
		flashList = flash_list;
	}

	AnimePlay(SpiritNode* spirit_, unsigned flash_num, bool isLoop_)
	{
		spirit = spirit_;
		flashNum = flash_num;
		isLoop = isLoop_;
		for (unsigned i = 0; i != spirit->spirit->stateNum_; i++)
			flashList.push_back(i);
	}

	bool PlayAnime(unsigned endState = 0) 
	{
		if (!flashStart||!spirit->spirit->rendering)
			return true;
		if (++flashCount == flashNum) 
		{
			flashCount = 0;
			spirit->ChangeState(flashList[++animalCount]);
			if (animalCount == flashList.size()) 
			{
				animalCount = 0;
				if (!isLoop)
				{
					spirit->ChangeState(flashList[endState]);
					flashStart = false;
					return true;
				}
			}
		}
		return false;
	}

	void Reset() 
	{
		spirit->ChangeState(0);
		flashStart = false;
		flashCount = 0;
		animalCount = 0;
	}
private:
	unsigned flashCount = 0;
};

