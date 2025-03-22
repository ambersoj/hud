#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

class Command
{
public:
virtual std::string execute() = 0;
virtual ~Command() = default;
};

#endif
