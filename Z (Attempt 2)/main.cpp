#include "main.h"

int main(int argc, const char* args[]) {
	using std::cout;

	cout << IO_NORM;

	const char* const commands[] = {
		"-assemble",
		"-exec",
		"-debug",
		"-nodebug",
		"-profile"
	};

	for (int i = 1; i < argc; i++) {
		switch (stringMatchAt(args[i], commands, ARR_LEN(commands))) {
			case -1:
				cout << IO_YELLOW "Unknown console command: " << args[i] << IO_NORM "\n";
				break;

			case 0: // -assemble
				break;

			case 1: // -exec
				break;

			case 2: // -debug
				break;

			case 3: // -nodebug
				break;

			case 4: // -profile
				break;
		}
	}

}