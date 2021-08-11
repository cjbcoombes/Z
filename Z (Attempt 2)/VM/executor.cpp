#include "vm.h"

using std::cout;

int vm::executor::exec(const char* const& path, ExecutorSettings& execSettings) {
	cout << "Attempting to execute file \"" << path << "\"\n";

	std::fstream file;
	file.open(path, std::ios::in | std::ios::binary);

	try {
		return vm::executor::exec_(file, execSettings, std::cout);
	} catch (ExecutorException& e) {
		cout << IO_ERR "Error during execution : " << e.what() << IO_NORM IO_END;
	} catch (std::exception& e) {
		cout << IO_ERR "An unknown error ocurred during execution. This error is most likely an issue with the c++ executor code, not your code. Sorry. The provided error message is as follows:\n" << e.what() << IO_NORM IO_END;
	}

	return 1;
}

int vm::executor::exec_(std::iostream& file, ExecutorSettings& execSettings, std::ostream& stream) {
	return 0;
}