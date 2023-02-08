#include <bits/stdc++.h>
using namespace std;

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

int stringParse(std::string &str, int i, char q) {
    int j = i + 1;
    while (true) {
        if (j >= str.size()) break;
        if(str[j] == '\\') {
            j += 2;
            continue;
        }
        if (str[j] == q) break;
        j++;
    }
    if (j >= str.size()) {
        // die("Wrong Argument!\n");
    }
    return j;
}

// Splits an input string on the basis of a delimiter
std::vector<std::string> split(std::string &str, char delim)
{
    std::vector<std::string> tokens;

    std::string tmp = "";

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '"' || str[i] == '\'') {
            int j = stringParse(str, i, str[i]);
            tmp += str.substr(i + 1, j - i - 1);
            i = j;
        }
        else if (str[i] == '\\') {
            tmp.push_back(str[i]);
            if (i < str.size() - 1) {
                tmp.push_back(str[++i]);
            }
        }
        else if (str[i] == delim) {
            tokens.push_back(tmp);
            tmp = "";
        }
        else tmp.push_back(str[i]);
    }

    if (!tmp.empty()) {
        tokens.push_back(tmp);
        tmp = "";
    }

    for (std::string &s : tokens) {
        trim(s);
    }

    return tokens;
}

int main() {
    string s;
    getline(cin, s);
    auto A = split(s, ' ');
    for (auto it : A) {
        cout << it << endl;
    }
}