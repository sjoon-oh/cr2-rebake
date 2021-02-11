#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <map>

#include <getopt.h>
#include <iostream>


class CommandLineParser {
public:
  CommandLineParser(int argc, char** argv) {
    this->argc = argc;
    this->argv = argv;
    this->optString = "e:r:s:i:";
  }

  std::map<std::string, std::string> parseArguments() {
    std::map<std::string, std::string> fileNameMap;
    extern char* optarg;
    char c;

    while ((c = getopt(argc, argv, optString.c_str())) != - 1) {
      switch (c) {
        case 's':
          fileNameMap["source"] = std::string(optarg);
          break;
        case 'e':
          fileNameMap["edge"] = std::string(optarg);
          break;
        case 'i':
          fileNameMap["iteration"] = std::string(optarg);
          break;
        case '?':
          std::cout << "unknown argument" << std::endl;
          break;
        default:
          break;
      }
    }

    return fileNameMap;
  }

private:
  int argc;
  char** argv;
  std::string optString;

};


#endif
