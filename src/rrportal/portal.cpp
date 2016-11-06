#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <boost/program_options.hpp>
#include <signal.h>
#include "common/config.h"
#include "server.h"

using namespace boost::program_options;

bool g_quit = false;

void SigHandler(int signum) {
  std::cout << "receive signal " << signum << std::endl;
  if (signum == SIGINT) {
    g_quit = true;
  } else if (signum == SIGQUIT) {
    g_quit = true;
  }
}

void RegisterSignal() {
  signal(SIGINT, SigHandler);
  signal(SIGQUIT, SigHandler);
  std::cout << "register signal[SIGINT SIGQUIT] handler." << std::endl;
}

//----------------------------------------------------------------------

int main(int argc, char* argv[]) {
  try {
    options_description desc { "Options" };
    desc.add_options()("help", "help screen")("pool_size",
                                              "buffer pool size (byte)")(
        "db_path", "datebase path")("port", "port of the server listen on");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
      std::cout << "USAGE: nutshell [options]" << std::endl;
      std::cout << desc << std::endl;
      return 0;
    }
    if (vm.count("pool_size")) {
      config::Setting::instance().buffer_pool_size_ = vm["pool_size"].as<int>();
    }
    if (vm.count("db_path")) {
      config::Setting::instance().data_directory_ =
          vm["path"].as<std::string>();
    }
    if (vm.count("port")) {
      config::Setting::instance().port_ = vm["port"].as<uint32_t>();
    }

    std::cout << "nutshell startup ........." << std::endl;
    RegisterSignal();
    Server server(config::Setting::instance().port_);
    server.Start();
    while (true) {
      if (g_quit) {
        server.Stop();
        std::cout << "nutshell stop ........." << std::endl;
        break;
      }
      std::this_thread::sleep_for (std::chrono::seconds(1));
    }
  } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
  }
  return 0;
}

