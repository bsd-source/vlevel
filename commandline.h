// commandline.h
// declares the CommandLine class

#include <string>
#include <stack>

////////////////////////
// note: ill rewrite this to use a list of strings

class CommandLine {
 public:
  // argv is a pointer to a const pointer to a const char
  CommandLine(int argc, const char * const * argv);
  
  std::string GetProgramName();
  std::string GetOption();
  std::string GetArgument();
  bool End() const;

 private:
  std::string program_name;
  std::stack<std::string> args;
  bool in_short;
  bool cur_is_arg;
  bool no_more_options;
};
