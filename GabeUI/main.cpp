#include "App.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

int main(int argc, char** argv){
	App GabeUI;

	int ret = GabeUI.Main(argc, argv);
	return ret;
}
