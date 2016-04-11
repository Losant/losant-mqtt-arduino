#ifndef LOSANT_H_
#define LOSANT_H_

#define LOSANT_BROKER "broker.losant.com"
#define LOSANT_PORT 1883
#define LOSANT_PORT_SECURE 8883
#define LOSANT_TOPIC_PREFIX "losant/"
#define LOSANT_TOPIC_STATE "/state"
#define LOSANT_TOPIC_COMMAND "/command"

#include "internal/LosantDevice.h"
#include "internal/LosantCommand.h"

#endif /* LOSANT_H_ */
