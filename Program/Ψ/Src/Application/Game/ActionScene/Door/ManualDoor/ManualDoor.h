#pragma once

#include "../../OutputObject.h"

class ManualDoor :public OutputObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	bool m_isOpen = false;

};