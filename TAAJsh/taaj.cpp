#include <termios.h>
#include <sys/wait.h>
#include <bits/stdc++.h>

using namespace std;

#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
    TABLINE = 9,
    NEWLINE = 13,
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN
};

void die(const string s) {
    perror(s.c_str());
    exit(0);
}

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | IEXTEN | ICANON | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
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
        case CTRL_KEY('z'):
        case CTRL_KEY('d'):
            exit(0);
        case CTRL_KEY('m'):
        default: 
            break;
    }
}

// Sends one line in the form of a string
string ReadLine() {
    int c = editorReadKey();
    string arg;
    while (true) {
        if (c == NEWLINE) break;
        processCtrl(c);
        if (c == BACKSPACE) {
            if (!arg.empty()) {
                cout << "\b \b";
                arg.pop_back();
            }
        }
        else {
            cout << (char) c;
            arg.push_back(c);
        }
        fflush(stdout);
        c = editorReadKey();
    }
    return arg;
}


//Function to execute commands 
void execute_command(vector<string> command){
    char * arr[command.size()+1];
    for(int i = 0 ; i < command.size() ; i++){
        arr[i] = (char*)command[i].c_str();
    }
    arr[command.size()]=NULL;
    execvp(arr[0], arr); // Executing the command.
    exit(0);
}

const int MAXCHAR = 100;
const int MAXARGS = 100;

// Trims leading and trailing whitespaces of a string
void trim(string &str) {
    int i = 0;
    while (i < str.size() && str[i] == ' ') {
        i++;
    }
    str = str.substr(i);
    while (!str.empty() && str.back() == ' ') {
        str.pop_back();
    }
}

/**
 * @brief Identifies the input and output files from the command
 * 
 * @param str 
 * @return vector<string> 
 */
vector<string> InputOutputIdentifier(string& str) {
    bool output = (str.find('>')!=string::npos);
    bool input = (str.find('<')!=string::npos);
    vector<string> tokens(3);

    if(input && output){
        int pos1 = str.find('<');
        int pos2 = str.find('>');
        tokens[0] = str.substr(0, pos1);
        tokens[1] = str.substr(pos1+1, pos2-pos1-1);
        tokens[2] = str.substr(pos2+1);
    }
    else if(input){
        int pos = str.find('<');
        tokens[0] = str.substr(0, pos);
        tokens[1] = str.substr(pos+1);
        tokens[2] = "";
    }
    else if(output){
        int pos = str.find('>');
        tokens[0] = str.substr(0, pos);
        tokens[1] = "";
        tokens[2] = str.substr(pos+1);
    }
    else{
        tokens[0] = str;
        tokens[1] = "";
        tokens[2] = "";
    }
    return tokens;
}

// Splits an input string on the basis of a delimiter
vector<string> split(string& str, char delim) {
    vector<string> tokens;
    stringstream ss(str);
    
    string tmp;
    while (getline(ss, tmp, delim)) {
        trim(tmp);
        if(!tmp.empty())
            tokens.push_back(tmp);
    }
    return tokens;
}

int main() {
    enableRawMode();

    while (true) {
        char uname[256], hname[256], cdir[256];
        getlogin_r(uname, 256);
        gethostname(hname, 256);
        getcwd(cdir, 256);
        
        cout << "\033[1;32m" << uname << "@" << hname << ":" << "\033[1;33m" << cdir << "\033[0m" << "$ ";
        fflush(stdout);

        char input[MAXCHAR];
        char *args[MAXARGS];

        fgets(input, MAXCHAR, stdin);

        char *token = strtok(input, " \t\n");
        int ctr = 0;
        while (token != NULL) {
            args[ctr++] = token;
            token = strtok(NULL, " ");
        }
        args[ctr] = NULL;

        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            exit(0);
        }
        else if (pid < 0) {
            die("Error in forking!");
        }
        else {
            int status;
            waitpid(pid, &status, 0);
            // if (WIFEXITED(status))
        }
    }

    return 0;
}
