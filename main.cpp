#include "engine\RuneEngine.h"

int main(int argc, char *argv[])
{
	srand(time(NULL));

	RuneEngine mine;
	ENGINE_STATUS ret = mine.init();
	if (ret != Success)
		return ret;
	ret = mine.load();
	if (ret != Success)
		return ret;
	mine.exec();

	return 0;
}