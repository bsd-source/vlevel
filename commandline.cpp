// commandline.cpp
// defines the CommandLine class

#include <iostream>
#include <string>
#include <stdexcept>

#include <assert.h>

#include "commandline.h"

using namespace std;

// argv is a pointer to a const pointer to a const char
CommandLine::CommandLine(int argc, const char * const * argv)
{
  if(argc < 1) throw out_of_range("CommandLine::CommandLine(): argc < 1");
  for(int i = argc - 1; i > 0; --i) args.push(argv[i]);
  in_short = false;
  cur_is_arg = false;
  no_more_options = false;
  program_name = argv[0];
}

string CommandLine::GetOption()
{  
  if(no_more_options || args.empty()) return "";
  if(cur_is_arg) {
    cerr << "ignoring an argument" << endl;
    args.pop();
    if(args.empty()) return "";
  }
  while(true) {
    if(args.top().empty()) {
      args.pop();
      if(args.empty()) return "";
      in_short = false;
    } else {
      break;
    }
  }
  if(in_short) {
    string retval(args.top().substr(0, 1));
    args.top().erase(0, 1);
    return retval;
  }
  if(args.top() == "--") {
    no_more_options = true;
    args.pop();
    return "";
  }
  if(args.top().substr(0,2) == "--") {
    args.top().erase(0, 2);
    size_t eq_pos = args.top().find("=");
    string retval(args.top().substr(0, eq_pos));
    args.top().erase(0, eq_pos);
    if(args.top().empty()) {
      args.pop();
    } else {
      args.top().erase(0, 1);
      cur_is_arg = true;
    }
    return retval;
  }

  if(args.top() == "-") return "";

  if(args.top().substr(0, 1) == "-") {
    args.top().erase(0, 1);
    in_short = true;
    string retval(args.top().substr(0, 1));
    args.top().erase(0, 1);
    if(args.top().empty()) args.pop();
    return retval;
  }

  return "";
}

string CommandLine::GetArgument()
{
  if(args.empty()) return "";
  string retval = args.top();
  if(!cur_is_arg && retval == "--") {
    args.pop();
    if(args.empty()) return "";
    retval = args.top();
  }
  args.pop();
  cur_is_arg = false;
  in_short = false;
  return retval;
}

string CommandLine::GetProgramName()
{
  return program_name;
}

bool CommandLine::End() const {
  return args.empty();
}
