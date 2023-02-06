#include "./utils.hpp"
#include <sstream>
#include <unistd.h>
#include <termios.h>

// Trims leading and trailing whitespaces of a string
void trim(std::string &str) {
    int i = 0;
    while (i < str.size() && str[i] == ' ') {
        i++;
    }
    str = str.substr(i);
    while (!str.empty() && str.back() == ' ') {
        str.pop_back();
    }
}

// Splits an input string on the basis of a delimiter
std::vector<std::string> split(std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    
    std::string tmp;
    while (getline(ss, tmp, delim)) {
        trim(tmp);
        if(!tmp.empty())
            tokens.push_back(tmp);
    }
    return tokens;
}

// Sends one line in the form of a string
std::string ReadLine() {
    int c = editorReadKey();
    std::string arg;
    while (true) {
        if (c == NEWLINE){
            std::cout<<std::endl;
            break;
        }
        processCtrl(c);
        if (c == BACKSPACE) {
            if (!arg.empty()) {
                std::cout << "\b \b";
                arg.pop_back();
            }
        }
        else if (c == TABLINE){

        }
        else {
            std::cout << (char) c;
            arg.push_back(c);
        }
        fflush(stdout);
        c = editorReadKey();
    }
    return arg;
}

int editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '3': return BACKSPACE;
                        default: return '\x1b';
                    }
                }
            }
            else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    default: return '\x1b';
                }
            }
        }
        return '\x1b';
    }

    if (c == '\t') return TABLINE;
    if (c == '\n') return NEWLINE;

    return c;
}

// Ctrl key press handler
void processCtrl(char c) {
    switch(c) {
        case CTRL_KEY('c'):
            if (pid == 0) exit(0);
            break;
        case CTRL_KEY('z'):
        case CTRL_KEY('d'):
            exit(0);
        case CTRL_KEY('m'):
        default: 
            break;
    }
}

void die(const std::string & s) {
    perror(s.c_str());
    exit(0);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);
    // raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | IEXTEN | ICANON | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void shellPrompt(){
        char uname[256], hname[256], cdir[256];
        getlogin_r(uname, 256);
        gethostname(hname, 256);
        getcwd(cdir, 256);
        
        std::cout << "\r\n" << "\033[1;32m" << uname << "@" << hname << ":" << "\033[1;33m" << cdir << "\033[0m" << "$ ";
        fflush(stdout);
}

//Function to execute commands 
void execute_command(std::vector<std::string> command){
    char * arr[command.size()+1];
    for(int i = 0 ; i < command.size() ; i++){
        arr[i] = (char*)command[i].c_str();
    }
    arr[command.size()]=NULL;
    execvp(arr[0], arr);
    std::cerr<<"Error in executing command";
    exit(0);
}