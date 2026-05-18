#pragma once
#include "BaseEngine.h"

class StarfieldDemo :
	public BaseEngine
{
public:
	StarfieldDemo();
	~StarfieldDemo();

	// Handle key presses - ESC to exit
	virtual void virtKeyDown(int iKeyCode) override;

	// Draw a simple starfield, as the bouncing ball
	virtual void virtSetupBackgroundBuffer() override;

	// This forces a screen redraw every pass through the loop.
	virtual void virtMainLoopDoBeforeUpdate() override;

	// Do something special when we copy the background buffer
	virtual void copyAllBackgroundBuffer() override;

protected:
	int m_iOffset;
};

