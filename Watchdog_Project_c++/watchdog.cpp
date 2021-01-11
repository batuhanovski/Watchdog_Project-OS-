#include <string>
#include <unistd.h>
#include <csignal>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>

using namespace std;

/**
 * The number of processes.
 */
int processNum;
/**
 * The path of the process output file given in the command line.
 */
string process_out_path;
/**
 * The path of the watchdog output file given in the command line.
 */
string watchdog_out_path;
/**
 * The char array used for writing to the pipe.
 */
char temp[30];
/**
 * The ofstream oobject, it used to writing to the watchdog output file.
 */
ofstream wd_outfile;
/**
 * The ofstream oobject, it used to writing to the process output file.
 */
ofstream p_outfile;
/**
 * The pipe which provides the communication between Watchdog and Executor.
 */
int unnamedPipe;

void sig_handler(int sig);

/**
 * @author Batuhan Tongarlak.
 *  It detects the sigterm signal with the signal function.
 *  After some string operations, watchdog writes the temp arrays (P_id <pid>) to the pipe.
 *  It writes the corresponding starting messages of processes with corresponding pids to the watchdog out file.
 *  Then it handles the SIGTERM cases,
 *  if P1 is terminated, then watchdog terminates and forks remaining processes, and it writes the new information to the pipe.
 *  if any process except P1 is terminated, it forks new process with respect to the terminated process' id, and writes it to the pipe.
 */


int main(int argc, char *argv[]) {
    signal(SIGTERM, sig_handler);

    processNum = stoi(argv[1]);
    process_out_path = argv[2];
    watchdog_out_path = argv[3];


    wd_outfile.open(watchdog_out_path);
    p_outfile.open(process_out_path,ios_base::trunc);


    char cwd[PATH_MAX];
    getcwd(cwd,sizeof(cwd));
    strcat(cwd,"/process");


    char *myfifo = (char *) "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    unnamedPipe = open(myfifo, O_WRONLY);
    sprintf(temp, "P0 %d", getpid());
    write(unnamedPipe, temp, 30);
    int pid[processNum];
    pid[0] = getpid();


    for (int i = 1; i <= processNum; i++) {
        if ((pid[i] = fork()) == 0) {
            sleep(i);
            wd_outfile << "P" << i << " is started and it has a pid of " << getpid() << endl;
            sprintf(temp, "P%d %d\n", i, getpid());
            write(unnamedPipe, temp, 30);
            execlp(cwd,"./process",temp,argv[2]);
            break;
        }

    }
        int killedpid=0;
        while ((killedpid = wait(NULL)) > 0) {
            if (pid[1] == killedpid) {
                wd_outfile << "P1 is killed, all processes must be killed" << endl;
                wd_outfile << "Restarting all processes" << endl;

                if ((pid[1]=fork()) == 0) {

                    sprintf(temp, "P%d %d", 1, getpid());
                    write(unnamedPipe, temp, 30);
                    execlp(cwd,"./process",temp,argv[2]);
                }
                for (int i = 2; i <= processNum; ++i) {
                    kill(pid[i], SIGTERM);
                    int afterOne = wait(NULL);
                    if (afterOne > 0) {
                        int thisid;
                        for (int k = 1; k <= processNum; ++k) {
                            if (afterOne == pid[k]) {
                                thisid = k;
                            }
                        }
                        if ((pid[thisid] = fork()) == 0) {

                            sleep(thisid*2);
                            sprintf(temp, "P%d %d", i, getpid());
                            write(unnamedPipe, temp, 30);
                            execlp(cwd, "./process", temp, argv[2]);
                        }

                    }
                }
                for (int i = 1; i <= processNum; ++i) {
                    wd_outfile << "P" << i << " is started and it has a pid of " << pid[i] << endl;
                }
            } else {

                int thisid;
                for (int i = 1; i <= processNum; ++i) {
                    if(killedpid==pid[i]){
                        thisid=i;
                    }
                }
                    wd_outfile << "P" << thisid << " is killed" << endl;
                    wd_outfile << "Restarting P" << thisid << endl;

                if ((pid[thisid]=fork()) == 0) {
                    wd_outfile << "P" << thisid << " is started and it has a pid of " << getpid() << endl;

                    sprintf(temp, "P%d %d", thisid, getpid());
                    write(unnamedPipe, temp, 30);
                    execlp(cwd,"./process",temp,argv[2]);
                }
            }

    }

    return 0;
}
/**
 * The signal handler function for watchdog, when these lines are compiling, that means watchdog is terminating.
 * @param sig Type of signal
 */
void sig_handler(int sig){
    wd_outfile << "Watchdog is terminating gracefully" << endl;
    exit(0);
}



