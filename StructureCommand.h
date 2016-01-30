#ifndef STRUCTURE_COMMAND_H_
#define STRUCTURE_COMMAND_H_

class StructureCommand {
  public:
    const char* name;
    const char* time;
    JsonObject* payload;
};

#endif /* STRUCTURE_COMMAND_H_ */
