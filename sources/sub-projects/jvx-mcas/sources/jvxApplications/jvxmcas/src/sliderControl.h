
#include "myCentralWidget.h"



class sliderControl
{
public:
	sliderControl();
	~sliderControl();

	void handleItemAdded();
	void handleItemRemoved();

private:

	enum  {STOPPED, PLAYING} state;

};