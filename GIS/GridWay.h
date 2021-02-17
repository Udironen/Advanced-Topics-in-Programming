#pragma once

#include "BaseGrid.h"

#define DEFAULT_WAY_NAME ""

class GridWay: public BaseGrid {


public:
	using BaseGrid::insert;
	void insert(Way&				way,
				const Coordinates&	from,
				const Coordinates&	to)
	{
		Way new_way = way;
		new_way.setName("");
		BaseGrid::insert(new_way, from, to);
	}

	~GridWay() = default;
};