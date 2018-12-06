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
  BOOST_LOG_TRIVIAL(info)<< "receive signal [" << signum << "].";
  if (signum == SIGINT) {
    g_quit = true;
  } else if (signum == SIGQUIT) {
    g_quit = true;
  }
}

void RegisterSignal() {
  signal(SIGINT, SigHandler);
  signal(SIGQUIT, SigHandler);
  BOOST_LOG_TRIVIAL(info)<< "register signal [SIGINT SIGQUIT] handler.";
}

//----------------------------------------------------------------------

int main(int argc, char* argv[]) {
  try {
    options_description desc { "Options" };
    desc.add_options()("help", "help screen")
        ("pool_size", value<uint32_t>()->default_value(BUFFER_POOL_SIZE), "buffer pool size (byte)")
        ("db_path", value<std::string>()->default_value("/tmp"), "datebase path")
        ("port", value<uint32_t>()->default_value(8432),"port of the server listen on");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
      std::cout << "USAGE: DBMS18 [options]" << std::endl;
      std::cout << desc << std::endl;
      return 0;
    }
    if (vm.count("pool_size")) {
      config::Setting::instance().buffer_pool_size_ = vm["pool_size"].as<uint32_t>();
    }
    if (vm.count("db_path")) {
      config::Setting::instance().data_directory_ =
          vm["db_path"].as<std::string>();
    }
    if (vm.count("port")) {
      config::Setting::instance().port_ = vm["port"].as<uint32_t>();
    }

    BOOST_LOG_TRIVIAL(info)<< "DBMS18 startup .........";
    RegisterSignal();
    Server server(config::Setting::instance().port_);
    server.Start();
    while (true) {
      if (g_quit) {
        server.Stop();
        BOOST_LOG_TRIVIAL(info)<< "DBMS18 stop .........";
        break;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
  }
  return 0;
}

