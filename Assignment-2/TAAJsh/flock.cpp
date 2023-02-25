#include "./flock.hpp"
#include "./utils.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>

void log_pids(const std::string &__fname, std::vector<pid_t> &pids)
{
    DIR *proc_dir, *proc_fd_dir;
    dirent *ent, *fd_ent;
    std::string __proc_fd_dir_name, link;

    char buf[MAXLINKLEN];

    if ((proc_dir = opendir("/proc")) == NULL)
    {
        perror("opendir \'/proc\'");
        return;
    }

    while ((ent = readdir(proc_dir)) != NULL)
    {
        if (ent->d_name[0] < '0' || ent->d_name[0] > '9')
            continue;

        pid_t proc_id = static_cast<pid_t>(atoll(ent->d_name));

        __proc_fd_dir_name = "/proc/" + std::to_string(proc_id) + "/fd";

        if ((proc_fd_dir = opendir(__proc_fd_dir_name.c_str())) == NULL)
            continue;

        while ((fd_ent = readdir(proc_fd_dir)) != NULL)
        {
            if (fd_ent->d_name[0] != '.')
            {
                link = __proc_fd_dir_name + "/" + fd_ent->d_name;

                ssize_t len = readlink(link.c_str(), buf, MAXLINKLEN);

                if (len == -1)
                    continue;

                buf[len] = 0;

                if (buf == __fname)
                {
                    pids.push_back(proc_id);
                }
            }
        }
        closedir(proc_fd_dir);
    }
    closedir(proc_dir);
}

void delep(std::string __fname)
{
    char *temp = realpath(__fname.c_str(), NULL);
    if (temp == NULL)
    {
        perror("error");
        return;
    }
    __fname = temp;
    free(temp);

    int fd[2], pid;

    if (pipe(fd) < 0 || (pid = fork()) < 0)
    {
        perror("Error executing command");
        return;
    }
    if (pid == 0)
    {
        close(fd[0]);

        std::vector<pid_t> pids;
        log_pids(__fname, pids);
        for (int i = 0; i < pids.size(); i++)
        {
            write(fd[1], &pids[i], sizeof(pids[i]));
        }
        close(fd[1]);
        exit(0);
    }

    else
    {
        close(fd[1]);

        std::vector<pid_t> pids;

        int n;
        pid_t pid;
        while ((n = read(fd[0], &pid, sizeof(pid))) > 0)
        {
            pids.push_back(pid);
        }

        close(fd[0]);

        if (pids.empty())
        {
            std::cout << "No processes holding a lock or have the file open\n";
            return;
        }

        std::cout << "Processes holding a lock or have the file open:\n";
        for (int i = 0; i < pids.size(); i++)
        {
            std::cout << pids[i] << std::endl;
        }

        std::cout << "Kill these processes and delete the file (yes/no)? ";

        std::string flag;
        std::cin >> flag;
        // trim(flag);
        if (flag == "yes")
        {
            for (pid_t pid : pids)
            {
                if (kill(pid, SIGKILL) < 0)
                {
                    std::string msg = "kill " + std::to_string(pid);
                    perror(msg.c_str());
                }
            }
            if (unlink(__fname.c_str()) < 0)
            {
                std::string msg = "unlink " + __fname;
                perror(msg.c_str());
            }
            remove(__fname.c_str());
        }
    }
}
