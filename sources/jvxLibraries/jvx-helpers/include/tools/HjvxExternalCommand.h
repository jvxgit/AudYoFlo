#ifndef __HJVXEXTERNALCOMMAND_H__
#define __HJVXEXTERNALCOMMAND_H__

void jvx_run_external_command(IjvxExternalCall* hdl, std::string command);

void jvx_run_external_command_1data(IjvxExternalCall* hdl, std::string command, int numEntries, ...);

#endif