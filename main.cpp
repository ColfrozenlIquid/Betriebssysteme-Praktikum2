#include <iostream>
#include <unistd.h>
#include <vector>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARG_LENGTH

typedef struct ConsoleArgs {
    char** argument_array;
    size_t arr_size;
    bool background;
} ConsoleArgs;

ConsoleArgs getUserInputArguments();
char** vectorToCharArray(const std::vector<std::string>& string_vector);
void freeCharArray(char** char_array, size_t arr_size);
bool handleLogout();
void printPIDVector(const std::vector<pid_t>& pid_vec);
bool getInputArgs(ConsoleArgs& console_args);

int main() {
    bool running = true;
    int status = 0;
    std::vector<pid_t> child_pid_vec;
    std::vector<pid_t> parent_pid_vec;
    
    while (running) {
        std::cout << "> ";
        ConsoleArgs input_args;
        bool logout = false;
        logout = getInputArgs(input_args);

        if (logout) {
            running = false;
            break;
        }

        // ConsoleArgs user_input_args = getUserInputArguments();
        pid_t pid = fork();

        if (pid == 0) {
            int ret = execvp(input_args.argument_array[0], input_args.argument_array);
            if (ret == -1) {
                int err = errno;
                std::cout << "Error: " << errno << std::endl;
            }
        }

        if (input_args.background) {
            child_pid_vec.push_back(pid);
        }
        else if (!input_args.background) {
            waitpid(pid, &status, WUNTRACED);
            if (WIFEXITED(status)) {
                std::cout << "Process exited normally" << std::endl;
            }
        }
    
        printPIDVector(child_pid_vec);
        freeCharArray(input_args.argument_array, input_args.arr_size); 
    }
    std::cout << "Exiting" << std::endl;
    return 0;
}

bool getInputArgs(ConsoleArgs& console_args) {
    std::string user_input;
    std::string token;
    std::vector<std::string> token_vec;
    bool logout = false;
    console_args.background = false;

    std::getline(std::cin, user_input);

    std::istringstream string_stream(user_input);
    while (string_stream >> token) {
        token_vec.push_back(token);
    }

    if (token_vec[0] == "logout") {
        logout = handleLogout();
    }

    if (logout) {
        return true;
    }

    if (token_vec[token_vec.size()-1] == "&") {
        console_args.background = true;
        token_vec.pop_back();
    }

    console_args.arr_size = token_vec.size();
    console_args.argument_array = vectorToCharArray(token_vec);

    return false;
}

void printPIDVector(const std::vector<pid_t>& pid_vec) {
    std::cout << "List of PIDs: ";
    if (pid_vec.size() == 0) {
        std::cout << "{ }" << std::endl;
        return;
    }
    for (int i = 0 ; pid_t pid : pid_vec) {
        if (pid_vec.size() == 1) {
            std::cout << "{ " << pid << " }" << std::endl;
            break;
        }
        if (i == 0) {
            std::cout << "{ " <<  pid << ", ";
        } else if (i < pid_vec.size()-1) {
            std::cout << pid << ", ";
        } else {
            std::cout << pid << " }" << std::endl;
        }
        i++;
    }
}

char** vectorToCharArray(const std::vector<std::string>& string_vector) {
    char** char_array = new char*[string_vector.size() + 1];
    for (size_t i = 0; i < string_vector.size(); i++) {
        char_array[i] = new char[string_vector[i].size()];
        std::strcpy(char_array[i], string_vector[i].c_str());
    }
    return char_array;
}

void freeCharArray(char** char_array, size_t arr_size) {
    for (size_t i = 0; i < arr_size; i++) {
        delete[] char_array[i];
    }
    delete[] char_array;
}

ConsoleArgs getUserInputArguments() {
    std::string user_input;
    std::string token;
    std::vector<std::string> token_vec;
    ConsoleArgs console_args;
    console_args.background = false;

    std::getline(std::cin, user_input);

    std::istringstream string_stream(user_input);
    while (string_stream >> token) {
        token_vec.push_back(token);
    }

    if (token_vec[0] == "logout") {
        handleLogout();
        return getUserInputArguments();
    }

    if (token_vec[token_vec.size()-1] == "&") {
        console_args.background = true;
        token_vec.pop_back();
    }

    console_args.arr_size = token_vec.size();
    console_args.argument_array = vectorToCharArray(token_vec);

    return console_args;
}

bool handleLogout() {
    std::string user_input;

    std::cout << "Are you sure you want to logout (y/n)" << std::endl;
    std::cout << "> ";

    std::getline(std::cin, user_input);

    if (user_input[0] == 'y' || user_input[0] == 'Y') {
        return true;
    } else if (user_input[0] == 'n' || user_input[0] == 'N') {
        return false;
    } else {
        std::cout << "Invalid input: " << user_input[0] << std::endl;
        handleLogout();
    }
    return false;
}