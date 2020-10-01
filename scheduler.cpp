/*
 * 457 Assignment 4: scheduler simulation
 * L1 T04
 * Daniel Artuso
 * 30029826

Compile with g++ scheduler.cpp

Command line arguments
Your program will accept 2 or 3 command-line arguments, depending on the scheduling
algorithm:
1. The name of the configuration file. Your program will read the configuration file to
obtain the description of processes.
2. The name of the scheduling algorithm: ‘RR’ for round-robin or ‘SJF’ for shortest job first.
3. The time quantum for the RR scheduling algorithm. If 2 nd argument is SJF, the 3 rd
argument will not be specified.
For example, the command line below should invoke your simulator on file config.txt using
RR scheduler and time slice of 3:
./scheduler config.txt RR 3
To run the simulation on the same file using SJF schedule, you would start your simulator like
this:
$ ./scheduler config.txt SJF
If the user does not provide correct arguments, you should print out an informative error message
and abort the program. For example, if the user specifies time-slice for SJF, you should report
this as an error and abort the program. You must support the uppercase strings "RR" and "SJF",
but if you wish you can also upport lower case versions.
Use FCFS to break ties. A tie could occur if a new process arrives exactly at the same time as a
currently executing process exceeds its time slice. Since both processes must be inserted into the
ready, we need to decide which one to insert first. By using FCFS to break ties, the already
executing job (the older of the two) will be inserted into the ready queue first, and the newly
arrived job second.
 * */

#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <climits>
#include <queue>
#include <iomanip>

/**
 * Event class for each process information
 */
class Event {
protected:
	int processID;
	int arrivalTime;
	int startTime = -1;
	int finishTime = -1;
	int burstTime;
	int timeRemaining;
public:
	Event(int processId, int arrivalTime, int burstTime) : processID(processId), arrivalTime(arrivalTime),
	                                                       burstTime(burstTime), timeRemaining(burstTime) {};

	bool added = false;

	bool doJob(){
		timeRemaining--;
		return timeRemaining >= 0;
	}
	void setStartTime(int startTime) {
		Event::startTime = startTime;
	}

	void setFinishTime(int finishTime) {
		Event::finishTime = finishTime;
	}

	int getTimeRemaining() const {
		return timeRemaining;
	}

	int getArrivalTime() const {
		return arrivalTime;
	}

	int getStartTime() const {
		return startTime;
	}

	int getFinishTime() const {
		return finishTime;
	}

	int getBurstTime() const {
		return burstTime;
	}

	int getProcessId() const {
		return processID;
	}

	int getTurnaroundTime() const{
		return finishTime - arrivalTime;
	}
	int getWaitingTime() const{
		return getTurnaroundTime() - burstTime;
	}
};

/**
 * Generic scheduler class for common scheduler needs
 */
class Scheduler {
public:
	std::map<int,Event> events;
protected:
	int eventToComplete = 0;
	int time = 0;

	void printHeader() {
		std::cout << "Time ";
		for (auto &event : events){
			std::cout << "P" << event.first;
			std::cout << " ";
		}
		std::cout << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;
	}

	void printState(int curr){
		std::cout << std::right <<std::setprecision(4) << std::setw(4) << time << " ";
		if (curr > -1){
			for (auto &event : events) {
				std::cout << "  ";
				if (event.second.getArrivalTime() <= time) {
					if (event.first == curr) {
						std::cout << "#";
					}else if (event.second.getFinishTime() == -1){
						std::cout << ".";
					}
				}
			}
		}
		std::cout << std::endl;
	}

	void printSummary() {
		std::cout << "------------------------------------------------------------" << std::endl;
		for (auto &event : events){
			std::cout << "P" << event.first << " waited " << (double) event.second.getWaitingTime() << ".000 sec." << std::endl;
		}
		std::cout << "Average waiting time = " << getAverageWaitTime() << "sec." << std::endl;
	}
	double getAverageWaitTime() const{
		double sum = 0;
		for (auto &event : events){
			sum += event.second.getWaitingTime();
		}
		return sum / (double) events.size();
	}

};

/**
 * Round Robin implementation
 */
class RoundRobin : public Scheduler{
protected:
	int timeQuantum;
	std::map<int,Event>::iterator it ;

public:
	explicit RoundRobin(int timeQuantum) : timeQuantum(timeQuantum) {};
	std::queue<Event> pQueue;

	void addJobstoQueue() {
		for (auto &event : events){
			if (event.second.getArrivalTime() <= time && !event.second.added){
				event.second.added = true;
				pQueue.push(event.second);
			}
		}
	}

	void doScheduling()  {
		printHeader();
		eventToComplete = events.size();
		it = events.begin();
		while (eventToComplete){
			addJobstoQueue();
			if (pQueue.empty()){
				printState(-1);
				time++;
			} else {
				doJob();
			}
		}
		printSummary();
	}


	void doJob() {
		auto curr = pQueue.front();
		auto currIt = events.find(curr.getProcessId());
		pQueue.pop();
		if(currIt->second.getStartTime() == -1){
			currIt->second.setStartTime(time);
		}
		for (int i = 0; i < timeQuantum; ++i) {
			if (currIt->second.doJob()){
				printState(curr.getProcessId());
				time++;
				if(currIt->second.getTimeRemaining() == 0){
					currIt->second.setFinishTime(time);
					eventToComplete--;
					break;
				}
			} else {
				currIt->second.setFinishTime(time);
				eventToComplete--;
				break;
			}
		}

		if(it != events.end()) {
			addJobstoQueue();
		}

		if (currIt->second.getFinishTime() == -1){
			pQueue.push(currIt->second);
		}
	}
};


/**
 * SJF implementation
 */
class ShortestJobFirst : public Scheduler{
public:
	void doScheduling()  {
		printHeader();
		eventToComplete = events.size();
		while (eventToComplete){
			int id = selectJob();
			if (id == -1){
				printState(id);
				time++;
			} else {
				doJob(id);
			}
		}
		printSummary();
	}

	int selectJob() {
		int currId = -1;
		int currBurst = INT_MAX;
		for(auto &event : events){
			if (event.second.getArrivalTime() <= time && event.second.getStartTime() == -1){
				if(event.second.getBurstTime() < currBurst)
					currId = event.first;
			}
		}
		return currId;
	}

	void doJob(int id) {
		auto it = events.find(id);
		if(it->second.getStartTime() == -1){
			it->second.setStartTime(time);
		}
		while (it->second.doJob()){
			printState(it->first);
			time++;
		}
		it->second.setFinishTime(time);
		eventToComplete--;
	}
};




int main (int argc, char *argv[]){
	std::ifstream configFile(argv[1]);
	if(!configFile.is_open()){
		std::cerr << "Error opening file\n";
		exit(1);
	}


	std::string algorithm(argv[2]);
	if (algorithm == "RR"){
		int timeQuant = 0;
		if (argv[3]){
			std::string time(argv[3]);
			timeQuant = stoi(time);
		} else {
			std::cerr << "RR requires a time quantum\n";
			exit(2);
		}
		RoundRobin myRR(timeQuant);
		int counter = 0;
		while (!configFile.eof()){
			int arrival;
			int burst;
			configFile >> arrival;
			configFile >> burst;
			Event event(counter,arrival,burst);
			myRR.events.insert(std::make_pair(counter,event));
			counter++;
		}
		myRR.doScheduling();
	} else if (algorithm == "SJF"){
		if (argv[3]){
			std::cerr << "No time slice for SJF\n";
			exit(3);
		}
		ShortestJobFirst mySJF;
		int counter = 0;
		while (!configFile.eof()){
			int arrival;
			int burst;
			configFile >> arrival;
			configFile >> burst;
			Event event(counter,arrival,burst);
			mySJF.events.insert(std::make_pair(counter,event));
			counter++;
		}
		mySJF.doScheduling();
	} else {
		std::cerr << "Invalid scheduling algorithm\n Enter ‘RR’ for round-robin or ‘SJF’ for shortest job first\n";
		exit(4);
	}

	return 0;
}