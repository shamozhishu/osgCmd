#pragma once

#include "Entity.h"

class BlueArmy : public Entity
{
	friend class Scene;
	BlueArmy();
	~BlueArmy();
public:
	void init();
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);
};
