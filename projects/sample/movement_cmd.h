#ifndef MOVEMENT_CMD_H
#define MOVEMENT_CMD_H

#include <oak/core/command.h>

namespace sample
{
  struct MovementCMD : oak::Command
  {
    MovementCMD();
    void execute() override;
  };
}

#endif