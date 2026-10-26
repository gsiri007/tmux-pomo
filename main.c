#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define POMO_PID "/tmp/pomo"

int main(int argc, char **argv)
{
    if (argc == 1)
    {
      printf("Usage: pomo <command> [time | stop]\n");
      return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "stop") == 0)
    {
      FILE *f = fopen(POMO_PID, "r");
      pid_t pid;
      if (fscanf(f, "%d", &pid) == 1)
      {
        kill(pid, SIGKILL);
        remove(POMO_PID);
      }

      system("tmux set-option -g status-right \"\"");
      return EXIT_SUCCESS;
    }

    FILE *fpid= fopen(POMO_PID, "r");
    if (fpid)
    {
      printf("pomo running...\n");
      return EXIT_SUCCESS;
    }

    pid_t pid = fork();
    if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    setsid();

    FILE *f = fopen(POMO_PID, "w");
    if (f)
    {
      fprintf(f, "%d", getpid());
      fclose(f);
    }

    int time = strtol(argv[1], NULL, 10);

    char cmd[512];
    int countdown = time;
    while (countdown > 0)
    {
      snprintf(cmd, sizeof(cmd), "tmux set-option -g status-right \"\U0001F345 %d\"", countdown);
      system(cmd);
      countdown -= 1;
      sleep(60);
    }

    system("tmux set-option -g status-right \"\"");
    remove(POMO_PID);
    return EXIT_SUCCESS;
}
