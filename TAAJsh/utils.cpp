#include "./utils.hpp"
#include <sstream>
#include <unistd.h>
#include <glob.h>
#include <readline/readline.h>


// Trims leading and trailing whitespaces of a string
void trim(std::string &str)
{
    int i = 0;
    while (i < str.size() && str[i] == ' ')
    {
        i++;
    }
    str = str.substr(i);
    while (!str.empty() && str.back() == ' ')
    {
        str.pop_back();
    }
}

// Splits an input string on the basis of a delimiter
std::vector<std::string> split(std::string &str, char delim)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);

    std::string tmp;
    while (getline(ss, tmp, delim))
    {
        trim(tmp);
        if (!tmp.empty())
            tokens.push_back(tmp);
    }
    return tokens;
}

int uparrowhandler(int count, int key) {
    rl_replace_line(history.getHistory(UP).c_str(), 0);
    rl_redisplay();
    rl_point = rl_end;
    return 0;
}

int downarrowhandler(int count, int key) {
    rl_replace_line(history.getHistory(DOWN).c_str(), 0);
    rl_redisplay();
    rl_point = rl_end;
    return 0;
}

int ctrlAhandler(int count, int key) {
    rl_point = 0;
    return 0;
}

int ctrlEhandler(int count, int key) {
    rl_point = rl_end;
    return 0;
}

std::string ReadLine() {

    rl_bind_keyseq("\\e[A", uparrowhandler);
    rl_bind_keyseq("\\e[B", downarrowhandler);
    rl_bind_keyseq("\\C-h", ctrlAhandler);
    rl_bind_keyseq("\\C-k", ctrlEhandler);
    
    std::string line = readline(shellPrompt().c_str());


    return line;
}

void die(const std::string &s)
{
    perror(s.c_str());
    exit(0);
}

std::string shellPrompt()
{
    char uname[256], hname[256], cdir[256];
    getlogin_r(uname, 256);
    gethostname(hname, 256);
    getcwd(cdir, 256);
    std::string res = "";
    res = "\r\n\033[1;32m" + std::string(uname) + "@" + std::string(hname) + ":\033[1;33m" + std::string(cdir) + "\033[0m" + "$ ";
    return res;
}

// Function to execute commands
void execute_command(std::vector<std::string> command)
{
    char *arr[command.size() + 1];
    for (int i = 0; i < command.size(); i++)
    {
        arr[i] = (char *)command[i].c_str();
    }
    arr[command.size()] = NULL;
    execvp(arr[0], arr);
    std::cerr << "Error in executing command";
    exit(0);
}

std::vector<std::string> expand_wildcards(const std::vector<std::string> &args)
{
    std::vector<std::string> expanded_args;
    for (const auto &arg : args)
    {
        if (arg.find_first_of("*?") == std::string::npos)
        {
            expanded_args.push_back(arg);
            continue;
        }
        if (arg.front() == '\"' && arg.back() == '\"')
        {
            expanded_args.push_back(arg);
            continue;
        }
        if (arg.front() == '\'' && arg.back() == '\'')
        {
            expanded_args.push_back(arg);
            continue;
        }

        glob_t result;
        glob(arg.c_str(), GLOB_TILDE, NULL, &result);
        for (int i = 0; i < result.gl_pathc; i++)
        {
            expanded_args.push_back(result.gl_pathv[i]);
        }
        globfree(&result);
    }
    return expanded_args;
}

