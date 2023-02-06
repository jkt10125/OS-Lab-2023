#include <termios.h>
#include <sys/wait.h>
#include <bits/stdc++.h>

using namespace std;

#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(IEXTEN | ICANON | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void die(const string s) {
    perror(s.c_str());
    exit(0);
}

const int MAXCHAR = 100;
const int MAXARGS = 100;

void processCtrl(char c) {
    switch(c) {
        case CTRL_KEY('c'):
            exit(0);
            break;
        default: 
            break;
    }
}

string ReadLine() {
    char c;
    cin >> c;
    string arg;
    while (c != '\n') {
        processCtrl(c);
        arg.push_back(c);
        cin >> c;
    }
    return arg;
}

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
