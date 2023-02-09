#include "./utils.hpp"
#include <sstream>
#include <unistd.h>
#include <glob.h>


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

int uparrowhandler(int count, int key)
{
    rl_replace_line(history.getHistory(UP).c_str(), 0);
    rl_redisplay();
    rl_point = rl_end;
    return 0;
}

int downarrowhandler(int count, int key)
{
    rl_replace_line(history.getHistory(DOWN).c_str(), 0);
    rl_redisplay();
    rl_point = rl_end;
    return 0;
}

int ctrlAhandler(int count, int key)
{
    rl_point = 0;
    return 0;
}

int ctrlEhandler(int count, int key)
{
    rl_point = rl_end;
    return 0;
}

std::string ReadLine()
{

    rl_bind_keyseq("\\e[A", uparrowhandler);
    rl_bind_keyseq("\\e[B", downarrowhandler);
    rl_bind_keyseq("\\C-a", ctrlAhandler);
    rl_bind_keyseq("\\C-e", ctrlEhandler);

    char *buff = readline(shellPrompt().c_str());
    if (buff == NULL)
        return "";
    std::string input(buff);
    free(buff);
    return input;
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

void expand_wildcards(const std::string &arg, std::vector<std::string> &expanded_args)
{
    bool is_expandable = false;
    for (int i = 0; i < arg.size(); i++)
    {
        if ((arg[i] == '*' || arg[i] == '?') && (i == 0 || arg[i - 1] != '\\'))
        {
            is_expandable = true;
            break;
        }
    }

    std::string unescaped_arg = "";
    for (int i = 0; i < arg.size(); i++)
    {
        if (arg[i] != '\\')
            unescaped_arg.push_back(arg[i]);
        else if (arg[i] == '\\' && i + 1 < arg.size())
        {
            unescaped_arg.push_back(arg[++i]);
        }
        else
        {
            // error
        }
    }

    if (!is_expandable)
    {
        expanded_args.push_back(unescaped_arg);
        return;
    }

    glob_t result;
    glob(arg.c_str(), GLOB_TILDE, NULL, &result);
    if (result.gl_pathc == 0)
        expanded_args.push_back(unescaped_arg);
    for (int i = 0; i < result.gl_pathc; i++)
    {
        expanded_args.push_back(result.gl_pathv[i]);
    }
    globfree(&result);
}

std::vector<std::string> parseArgs(std::string &str, char delim)
{
    std::vector<std::string> tokens;

    std::string tmp = "";

    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == '\"' || str[i] == '\'')
        {
            int j = i + 1;
            while (true)
            {
                if (j >= str.size())
                    break;
                if (str[j] == '\\')
                {
                    j += 2;
                    continue;
                }
                if (str[j] == str[i])
                    break;
                j++;
            }

            if (j >= str.length())
            {
                tokens.push_back(str.substr(i));
            }
            else
                tokens.push_back(str.substr(i + 1, j - i - 1));
            tmp = "";
            i = j;
        }
        else if (str[i] == '\\')
        {
            tmp.push_back(str[i]);
            if (i < str.size() - 1)
            {
                tmp.push_back(str[++i]);
            }
        }
        else if (str[i] == delim)
        {
            trim(tmp);
            if (!tmp.empty())
            {
                expand_wildcards(tmp, tokens);
            }
            tmp = "";
        }
        else
            tmp.push_back(str[i]);
    }

    trim(tmp);

    if (!tmp.empty())
    {
        expand_wildcards(tmp, tokens);
    }
    return tokens;
}
