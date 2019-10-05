#include "Application.h"
#include "tinyfiledialogs.h"

int main(void) {
	VoxVRApplication app;
	try {
		app.run();
	} catch (std::exception & e) {
		std::cerr << "Application Failed With Error: " << e.what() << std::endl;
		tinyfd_messageBox("Error",e.what(),"ok","error",1);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}