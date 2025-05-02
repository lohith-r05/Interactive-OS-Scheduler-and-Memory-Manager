#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "libs/json.hpp"

using namespace std;
using json = nlohmann::json;

class MemoryBlock {
public:
    bool isFree;
    int size;
    string label;
    bool isReserved;

    MemoryBlock(int size, bool isFree, bool isReserved = false)
        : size(size), isFree(isFree), isReserved(isReserved) {
        label = to_string(size);
    }
};

struct GanttEntry {
    int processID;
    int startTime;
    int endTime;
};

void drawMemoryBlocks(sf::RenderWindow& window, const vector<MemoryBlock>& memoryBlocks, float yPosition, bool isInitial) {
    float current = 50;

    for (const auto& block : memoryBlocks) {
        sf::RectangleShape rectangle;

        if (isInitial) {
            rectangle.setFillColor(block.isFree ? sf::Color::White : sf::Color(169, 169, 169));
        } else {
            if (block.isReserved) rectangle.setFillColor(sf::Color(169, 169, 169));
            else if (!block.isFree) rectangle.setFillColor(sf::Color::Green);
            else rectangle.setFillColor(sf::Color::White);
        }

        float scaledWidth = block.size / 3.3f;
        rectangle.setSize(sf::Vector2f(scaledWidth, 50));
        rectangle.setPosition(current, yPosition);
        window.draw(rectangle);

        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            cerr << "Error loading font!" << endl;
            return;
        }

        if (!block.isReserved) {
            sf::Text text;
            text.setFont(font);
            text.setString(block.label);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::Black);

            float textX = current + (scaledWidth - text.getLocalBounds().width) / 2;
            float textY = yPosition + (50 - text.getLocalBounds().height) / 2;
            text.setPosition(textX, textY);
            window.draw(text);
        }

        current += scaledWidth;
    }
}

vector<GanttEntry> simulateScheduling(const string& algo, const vector<int>& burst, int quantum = 4) {
    int n = burst.size();
    vector<int> wt(n, 0), tat(n, 0);
    vector<GanttEntry> gantt;

    cout << "\nScheduling Results (" << algo << "):\n";

    if (algo == "FCFS") {
        int time = 0;
        for (int i = 0; i < n; ++i) {
            wt[i] = time;
            gantt.push_back({i, time, time + burst[i]});
            time += burst[i];
            tat[i] = wt[i] + burst[i];
        }
    }
    else if (algo == "SJF") {
        vector<int> idx(n);
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(), [&](int a, int b) { return burst[a] < burst[b]; });

        int time = 0;
        for (int i : idx) {
            wt[i] = time;
            gantt.push_back({i, time, time + burst[i]});
            time += burst[i];
            tat[i] = wt[i] + burst[i];
        }
    }
    else if (algo == "RR") {
        vector<int> rem = burst;
        vector<int> arrival(n, 0); // all arrive at time 0
        int t = 0;
        queue<int> q;
        for (int i = 0; i < n; ++i) q.push(i);

        while (!q.empty()) {
            int i = q.front(); q.pop();
            if (rem[i] > 0) {
                int exec = min(quantum, rem[i]);
                gantt.push_back({i, t, t + exec});
                t += exec;
                rem[i] -= exec;
                if (rem[i] > 0) {
                    q.push(i);
                } else {
                    tat[i] = t;
                    wt[i] = tat[i] - burst[i];
                }
            }
        }
    }

    for (int i = 0; i < n; ++i)
        cout << "Process " << i + 1 << " - WT: " << wt[i] << ", TAT: " << tat[i] << '\n';

    float avgWT = accumulate(wt.begin(), wt.end(), 0.0f) / n;
    float avgTAT = accumulate(tat.begin(), tat.end(), 0.0f) / n;

    cout << fixed << setprecision(2);
    cout << "\nAverage Waiting Time: " << avgWT << "\n";
    cout << "Average Turnaround Time: " << avgTAT << "\n";

    cout << "\nGantt Chart Timings:\n";
    for (const auto& entry : gantt) {
        cout << entry.startTime << " to " << entry.endTime << " = P" << entry.processID + 1 << '\n';
    }

    return gantt;
}

void drawGanttChart(sf::RenderWindow& window, const vector<GanttEntry>& gantt) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Error loading font!" << endl;
        return;
    }

    float xPosition = 50.0f;
    float yPosition = 400.0f;

    for (const auto& entry : gantt) {
        float width = (entry.endTime - entry.startTime) * 15.0f;
        sf::RectangleShape block(sf::Vector2f(width, 50));
        block.setPosition(xPosition, yPosition);
        block.setFillColor(sf::Color::Green);
        window.draw(block);

        sf::Text text;
        text.setFont(font);
        text.setString("P" + to_string(entry.processID + 1));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setPosition(xPosition + width / 2 - text.getLocalBounds().width / 2, yPosition + 15);
        window.draw(text);

        sf::Text timeText;
        timeText.setFont(font);
        timeText.setCharacterSize(15);
        timeText.setFillColor(sf::Color::Black);
        timeText.setString(to_string(entry.startTime));
        timeText.setPosition(xPosition - 5, yPosition + 60);
        window.draw(timeText);

        xPosition += width;
    }

    if (!gantt.empty()) {
        sf::Text endTime;
        endTime.setFont(font);
        endTime.setCharacterSize(15);
        endTime.setFillColor(sf::Color::Black);
        endTime.setString(to_string(gantt.back().endTime));
        endTime.setPosition(xPosition - 5, yPosition + 60);
        window.draw(endTime);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 550), "Memory Blocks Visualization");

    ifstream inputFile("input.json");
    json inputData;
    inputFile >> inputData;

    string memoryStrategy = inputData["memoryAllocationStrategy"];
    string schedulingAlgorithm = inputData["schedulingAlgorithm"];
    vector<int> processMemory = inputData["processes"]["memory"];
    vector<int> processBurstTime = inputData["processes"]["burstTime"];

    cout << "\nMemory Allocation Strategy: " << memoryStrategy << endl;
    cout << "Scheduling Algorithm: " << schedulingAlgorithm << endl;

    cout << "\nProcesses and their Burst Times:\n";
    for (int i = 0; i < processMemory.size(); i++) {
        cout << "Process " << i + 1 << " - Memory: " << processMemory[i]
             << ", Burst Time: " << processBurstTime[i] << endl;
    }

    vector<MemoryBlock> memoryBlocks = {
        MemoryBlock(500, true),
        MemoryBlock(150, false, true),
        MemoryBlock(100, true),
        MemoryBlock(150, false, true),
        MemoryBlock(300, true),
        MemoryBlock(150, false, true),
        MemoryBlock(600, true),
        MemoryBlock(150, false, true),
        MemoryBlock(200, true)
    };

    vector<MemoryBlock> allocatedBlocks = memoryBlocks;

    for (int i = 0; i < processMemory.size(); ++i) {
        bool allocated = false;
        int bestIndex = -1;

        for (int j = 0; j < allocatedBlocks.size(); ++j) {
            if (allocatedBlocks[j].isFree && allocatedBlocks[j].size >= processMemory[i]) {
                if (memoryStrategy == "firstFit") {
                    bestIndex = j;
                    break;
                } else if (memoryStrategy == "bestFit") {
                    if (bestIndex == -1 || allocatedBlocks[j].size < allocatedBlocks[bestIndex].size) {
                        bestIndex = j;
                    }
                }
            }
        }

        if (bestIndex != -1) {
            int originalSize = allocatedBlocks[bestIndex].size;
            allocatedBlocks[bestIndex].isFree = false;
            allocatedBlocks[bestIndex].size = processMemory[i];
            allocatedBlocks[bestIndex].label = "P" + to_string(i + 1);
            allocatedBlocks[bestIndex].isReserved = false;

            cout << "Process " << i + 1 << " is allocated to memory block of size " << originalSize << endl;

            int remainingSize = originalSize - processMemory[i];
            if (remainingSize > 0) {
                allocatedBlocks.insert(allocatedBlocks.begin() + bestIndex + 1, MemoryBlock(remainingSize, true));
            }
        } else {
            cout << "Process " << i + 1 << " could not be allocated.\n";
        }
    }

    vector<GanttEntry> ganttChart = simulateScheduling(schedulingAlgorithm, processBurstTime);

    bool isInitial = true;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        drawMemoryBlocks(window, memoryBlocks, 50, isInitial);
        drawMemoryBlocks(window, allocatedBlocks, 150, false);
        drawGanttChart(window, ganttChart);

        window.display();
        isInitial = false;
    }

    return 0;
}
