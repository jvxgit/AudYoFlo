#ifndef HJVXARGS_H__
#define HJVXARGS_H__

void
jvx_appArgs(char**& newArgv, int& newArgc, std::vector<std::string>& lstArgs);

void
jvx_deriveRunArgsJvx(std::vector<std::string>& lst, const std::string& argv0, bool binaryInSubfolder = true);

#endif
