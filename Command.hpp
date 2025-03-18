#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

class Command
{
public:
virtual std::string execute() = 0;  // Change to std::string
virtual ~Command() = default;
};

#endif
