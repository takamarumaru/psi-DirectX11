#pragma once

#include "../GameObject.h"

class InputObject :public GameObject
{
public:
	inline bool GetIsPush() { return m_isPush; }

protected:

	bool m_isPush = false;
};