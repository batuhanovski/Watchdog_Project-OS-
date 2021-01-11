#include <iostream>
#include <string>
#include <unistd.h>
#include <csignal>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <iostream>
#include <fstream>
#include <signal.h>

using namespace std;
void sig_handler(int sig);
/**
 * The path of the process out file.
 */
string process_pout;
/**
 * The ostream object used for writing into process output file.
 */
ofstream p_outfile;
/**
 * Particular string to the process which is like this shape: (P1 3019).
 */
string idandpid;
/**
 * After splitting idandpid, this string used for writing the process' index to the pipe.
 */
string p_index;
/**
 * After splitting idandpid, this string used for writing the process' pid to the pipe.
 */
string p_pid;

    /**
 * The signal coming from the executor captured here and after detecting them,
 * these functions sends it to the signal handler function
     * Using pid and id of the process, after the string operations, corresponding message is written at the process' output text.
     * It sleeps until a signal came thanks to the while loop.
     *
*/
int main(int argc, char *argv[]){

    process_pout = argv[2];

    signal(SIGHUP, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGTRAP, sig_handler);
    signal(SIGFPE, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGXCPU, sig_handler);

    idandpid = argv[1];

    int pid,id;

        stringstream splitmsg(idandpid);
        splitmsg >> p_index >> p_pid; //p0 ve <pid> ayrıldı
        id = stoi(p_index.substr(1)); // p den sonraki basamak alındı
        pid = stoi(p_pid); //pid alındı
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " is waiting for a signal" << endl;
        p_outfile.close();

        while(1) {
            sleep(1);
        }

    return 0;
}
/**
 * The signal handler function, first it makes some string splitting operations,
 * after achieving pid and id of the process, it does proper writing operations corresponding to the type of the signal.
 * Different than other signals, when the sigterm signal is detected, process terminates with the exit() function.
 */
void sig_handler(int sig){
    int pid,id;
    stringstream splitmsg(idandpid);
    splitmsg >> p_index >> p_pid; //p0 ve <pid> ayrıldı
    id = stoi(p_index.substr(1)); // p den sonraki basamak alındı
    pid = stoi(p_pid); //pid alındı

    if(sig==SIGTERM){
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " received signal " << SIGTERM << ", terminating gracefully" << endl;
        p_outfile.close();
        exit(0);

    }
    if(sig==SIGHUP){
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " received signal " << SIGHUP << endl;
        p_outfile.close();
    }
    if(sig==SIGINT){
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " received signal " << SIGINT << endl;
        p_outfile.close();
    }
    if(sig==SIGILL){
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " received signal " << SIGILL << endl;
        p_outfile.close();
    }
    if(sig==SIGTRAP){
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " received signal " << SIGTRAP << endl;
        p_outfile.close();
    }
    if(sig==SIGFPE){
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " received signal " << SIGFPE << endl;
        p_outfile.close();
    }
    if(sig==SIGSEGV){
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " received signal " << SIGSEGV << endl;
        p_outfile.close();
    }
    if(sig==SIGXCPU){
        p_outfile.open(process_pout,ios_base::app);
        p_outfile << "P" << id << " received signal " << SIGXCPU << endl;
        p_outfile.close();
    }
}

