#pragma once
namespace Core
{
	class IListener
	{
	public:
		virtual ~IListener() = 0;
		virtual void Initialise() = 0;

		//drawing functions
		virtual void notifyBeginFrame() = 0;
		virtual void notifyDisplayFrame() = 0;
		virtual void notifyEndFrame() = 0;
		virtual void notifyReshape(int width, int height, int previous_width, int previous_height) = 0;
		
		//keyboard handling
		virtual void notifyProcessNormalKeys(unsigned char key, int x, int y) = 0;
		virtual void notifyProcessSpecialKeys(int key, int x, int y) = 0;

		//mouse handling
		virtual void notifyProcessMouseState(int button, int state, int x, int y) = 0;
		virtual void notifyProcessMouseActiveMove(int x, int y) = 0;
		virtual void notifyProcessMousePassiveMove(int x, int y) = 0;
		virtual void notifyProcessMouseWindowEntryCallback(int state) = 0;
	};
	inline IListener::~IListener() 
	{
		//implementation of pure virtual destructor
	}
}
