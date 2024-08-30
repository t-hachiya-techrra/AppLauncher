#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void read_config(char* program, size_t programSize, char* arguments, size_t argumentsSize) {
	FILE* file = fopen("AppLauncher.ini", "r");
	if (file == NULL) {
		perror("Could not open config file");
		exit(EXIT_FAILURE);
	}

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "program=", 8) == 0) {
			strcpy_s(program, programSize, line + 8);
			program[strcspn(program, "\r\n")] = 0; // Remove newline characters
		}
		else if (strncmp(line, "arguments=", 10) == 0) {
			strcpy_s(arguments, argumentsSize, line + 10);
			arguments[strcspn(arguments, "\r\n")] = 0; // Remove newline characters
		}
	}

	fclose(file);
}

int main() {
	char program[256];
	char arguments[256];

	read_config(program, sizeof(program), arguments, sizeof(arguments));

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	// Command line string should include program and arguments
	char cmdline[512];
	snprintf(cmdline, sizeof(cmdline), "\"%s\" %s", program, arguments);
	wchar_t commandLine[512];
	size_t ret;
	mbstowcs_s(&ret, commandLine, 512, cmdline, _TRUNCATE);

	// Create the process
	if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 1;
	}

	// Close the process and thread handles, do not wait for the process to finish
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}
