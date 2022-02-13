#pragma once

class IAppBase
{
public:
	virtual void init() = 0;
	virtual void run() = 0;
	virtual void cleanup() = 0;
};
