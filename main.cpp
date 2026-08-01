#include <iostream>
#include <string>
#include <vector>

#include <filesystem>
#include <unistd.h>

#ifdef _WIN32
  constexpr char PATH_SEPARATOR = ';';
#else
  constexpr char PATH_SEPARATOR = ':';
#endif

constexpr char ECHO[] = "echo";
constexpr char EXIT[] = "exit";
constexpr char TYPE[] = "type";

void extract_words(const std::string& userInput, std::vector<std::string>& words) {
  std::string word = "";

  for(const char& ch : userInput) {
    if(ch == ' ') {
      if(word != "") {
        words.push_back(word);
      }

      word = "";
    }
    else {
      word += ch;
    }
  }

  if(word != "") {
    words.push_back(word);
  }
}

void echo_command(const std::vector<std::string>& string) {
  for(size_t i = 1; i < string.size(); ++i) {
    std::cout << string[i] << ' ';
  }

  std::cout << std::endl;
}

void type_command(const std::vector<std::string>& words) {
  if(words.size() == 1) {
    return;
  }

  std::string command = words.at(1);

  if(command == ECHO ||
     command == EXIT ||
     command == TYPE) {
    std::cout << command << " is a shell builtin" << std::endl;
  }
  else {
    // Creatina a string stream in order to read the paths separately easier
    std::istringstream pathStream(std::getenv("PATH"));
    std::string currPath;

    // Parsing the PATH variable and stopping each time we encounter a PATH_SEPARATOR
    while(std::getline(pathStream, currPath, PATH_SEPARATOR)) {
      // Constructing the wanted file path
      std::string wantedFilePath = currPath + '/' + command;

      // Checking if the file exists and execute access
      if(access(wantedFilePath.c_str(), X_OK) == 0) {
        std::cout << command << " is " << wantedFilePath << std::endl;
        return;
      }
    }

    std::cout << command << ": not found" << std::endl;
  }
}

void start_exec(const std::vector<std::string>& words) {
  const char *argv_list[words.size()] = {};
  size_t argv_len = 0;

  for(const std::string& word : words) {
    argv_list[argv_len++] = word.c_str();
  }

  argv_list[argv_len] = NULL;

  // Creatina a string stream in order to read the paths separately easier
  std::istringstream pathStream(std::getenv("PATH"));
  std::string command = words[0];
  std::string currPath;

  // Parsing the PATH variable and stopping each time we encounter a PATH_SEPARATOR
  while(std::getline(pathStream, currPath, PATH_SEPARATOR)) {
    // Constructing the wanted file path
    std::string wantedFilePath = currPath + '/' + command;

    // Checking if the file exists and execute access
    if(access(wantedFilePath.c_str(), X_OK) == 0) {
      execv(wantedFilePath.c_str(), const_cast<char* const*>(argv_list));
      return;
    }
  }

  std::cout << command << ": command not found" << std::endl;
  return;
}

void process_command(const std::vector<std::string>& words) {
  std::string command = words.at(0);

  if(command == EXIT) {
    exit(0);
  }
  else if(command == ECHO) {
    echo_command(words);
  }
  else if(command == TYPE) {
    type_command(words);
  }
  else {
    start_exec(words);
  }
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while(true) {
    std::cout << "$ ";
    std::string userInput;
    getline(std::cin, userInput);

    std::vector<std::string> words;
    extract_words(userInput, words);

    process_command(words);
  }

  return 0;
}
