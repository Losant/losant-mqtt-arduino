#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#define STRUCTURE_BROKER "broker.getstructure.io"
#define STRUCTURE_PORT 1883
#define STRUCTURE_PORT_SECURE 8883
#define STRUCTURE_TOPIC_PREFIX "structure/"
#define STRUCTURE_TOPIC_STATE "/state"
#define STRUCTURE_TOPIC_COMMAND "/command"

#include "internal/StructureDevice.h"
#include "internal/StructureCommand.h"

#endif /* STRUCTURE_H_ */
