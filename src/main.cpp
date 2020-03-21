#include <iostream>
#include <Windows.h>
#include "HazeDumper.h"
#include "ProcessManager.h"
#include "Cheats.h"
#include <future>

int main() {

    auto *dumper = new HazeDumper();

    auto *csgo = new ProcessManager("csgo.exe");

    std::future<bool> wait_thread = std::async(&ProcessManager::wait, csgo);
    puts("Searching for Program...");
    if (!wait_thread.get()) {
        puts("ERROR: Unable to attach to program!");
        system("pause");
        exit(1);
    } else {
        puts("SUCCESS: Program attached successfully");
    }

    puts("Searching for Modules...");
    Cheats *cheats = new Cheats(csgo, dumper);
    puts("Starting Up");
    cheats->run();

    puts("Exiting...");
    delete dumper;
    delete csgo;
    delete cheats;
    return 0;
}