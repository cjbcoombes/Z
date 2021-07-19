#include "vm.h"

using std::cout;

int vm::executor::exec(const char* const& path, Flags& execFlags) {
	cout << "Attempting to execute file \"" << path << "\"\n";

	std::fstream file;
	file.open(path, std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		cout << IO_ERR "Could not open file \"" << path << "\"" IO_NORM IO_END;
		return 1;
	}

	return 0;
}