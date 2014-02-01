#ifndef MGLINPUT_H
#define MGLINPUT_H

enum mglInputType	{ mglButtonInput, mglMotionInput };
enum mglInputState	{ mglButtonDown, mglButtonUp };
enum mglInputDevice	{ mglKeyboard, mglMouse, mglController1, mglController2, mglController3, mglController4 };

union mglInput
{
	mglInputType		type;
	struct
	{
		mglInputType	type;
		mglInputDevice	device;
		int				button;
		mglInputState	state;
	} button;

	struct
	{
		mglInputType	type;
		mglInputDevice	device;
		int				absX, absY;
		int				relX, relY;
	} motion;
};

#endif // MGLINPUT_H
