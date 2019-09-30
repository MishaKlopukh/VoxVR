#include "Application.h"

int main(void) {
	VoxVRApplication app;
	try {
		app.run();
	} catch (std::exception & e) {
		std::cerr << "Application Failed With Error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}