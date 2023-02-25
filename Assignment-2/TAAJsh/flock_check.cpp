#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <iostream>

void lockUnLock() {
  int fd, i;
  char path[] = "test.txt";
  fd = open(path, O_WRONLY | O_CREAT);
  if (fd != -1) {
    std::cout << "open file " << path << std::endl;
    std::cout << "Please input a number to lock the file" << path << std::endl;
    scanf("%d", &i);
    if (flock(fd, LOCK_EX) == 0) {
      std::cout << "The file was locked " << std::endl;
    } else {
      std::cout << "The file was not locked " << std::endl;
    }
    std::cout << "please input a number to unlock the file " << std::endl;
    scanf("%d", &i);
    if (flock(fd, LOCK_UN) == 0) {
      std::cout << "The file was unlocked. " << std::endl;
    } else {
      std::cout << "The file was no unlocked. " << std::endl;
    }

    std::cout << "The file will be closed in 30 seconds."<<std::endl;
    sleep(30);

    close(fd);
  } else {
    std::cout << "Cannot open file " << path << std::endl;
  }
}

int main() {
  lockUnLock();
  return 0;
}

