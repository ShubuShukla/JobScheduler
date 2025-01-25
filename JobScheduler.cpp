#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>

struct Job {
    int jobId, arrivalDay, arrivalHour, memReq, cpuReq, exeTime;
};

struct WorkerNode {
    int id;
    int availableCores = 24;
    int availableMemory = 64;
};

bool compareByShortestDuration(const Job& a, const Job& b) {
    return a.exeTime < b.exeTime;
}

bool compareBySmallestJob(const Job& a, const Job& b) {
    return (a.exeTime * a.cpuReq * a.memReq) < (b.exeTime * b.cpuReq * b.memReq);
}

class JobScheduler {
    std::vector<Job> jobQueue;
    std::vector<WorkerNode> workerNodes;
    std::vector<std::vector<int>> cpuUtilization;
    std::vector<std::vector<int>> memUtilization;
    
public:
    JobScheduler(int numWorkers) {
        for (int i = 0; i < numWorkers; i++) {
            workerNodes.push_back({i});
        }
        cpuUtilization.resize(7, std::vector<int>(24, 0));
        memUtilization.resize(7, std::vector<int>(24, 0));
    }
    
    void loadJobs(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file!" << std::endl;
            return;
        }
        Job job;
        while (file >> job.jobId >> job.arrivalDay >> job.arrivalHour >> job.memReq >> job.cpuReq >> job.exeTime) {
            jobQueue.push_back(job);
        }
        file.close();
    }
    
    void applyQueuePolicy(const std::string& policy) {
        if (policy == "smallest_job") {
            std::sort(jobQueue.begin(), jobQueue.end(), compareBySmallestJob);
        } else if (policy == "shortest_duration") {
            std::sort(jobQueue.begin(), jobQueue.end(), compareByShortestDuration);
        }
    }
    
    void scheduleJobs() {
        for (auto& job : jobQueue) {
            for (auto& node : workerNodes) {
                if (node.availableCores >= job.cpuReq && node.availableMemory >= job.memReq) {
                    node.availableCores -= job.cpuReq;
                    node.availableMemory -= job.memReq;
                    cpuUtilization[job.arrivalDay][job.arrivalHour] += job.cpuReq;
                    memUtilization[job.arrivalDay][job.arrivalHour] += job.memReq;
                    std::cout << "Job " << job.jobId << " assigned to Worker " << node.id << "\n";
                    break;
                }
            }
        }
    }
    
    void saveUtilizationToCSV(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file!" << std::endl;
            return;
        }
        file << "Day,Hour,CPU Utilization,Memory Utilization\n";
        for (int day = 0; day < 7; day++) {
            for (int hour = 0; hour < 24; hour++) {
                file << day << "," << hour << "," << cpuUtilization[day][hour] << "," << memUtilization[day][hour] << "\n";
            }
        }
        file.close();
    }
};

int main() {
    JobScheduler scheduler(128);
    scheduler.loadJobs("JobArrival.txt");
    scheduler.applyQueuePolicy("smallest_job"); // Change to "FCFS" or "shortest_duration" as needed
    scheduler.scheduleJobs();
    scheduler.saveUtilizationToCSV("utilization.csv");
    return 0;
}