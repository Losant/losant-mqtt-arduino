#ifndef LOSANT_COMMAND_H_
#define LOSANT_COMMAND_H_

class LosantCommand {
  public:
    const char* name;
    const char* time;
    JsonObject* payload;
};

#endif /* LOSANT_COMMAND_H_ */
