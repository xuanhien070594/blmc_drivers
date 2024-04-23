#include "blmc_drivers/devices/motor_board.hpp"
#include <array>
#include <atomic>
#include <iostream>
#include <memory>
#include <signal.h>
#include <unistd.h>

/**
 * @brief This boolean is here to kill cleanly the application upon ctrl+c
 */
std::atomic_bool StopDemos(false);

/**
 * @brief This function is the callback upon a ctrl+c call from the terminal.
 *
 * @param s
 */
void my_handler(int) { StopDemos = true; }

/**
 * @brief This is the main demo program.
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int, char **) {
  // make sure we catch the ctrl+c signal to kill the application properly.
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
  StopDemos = false;
  const int N_MOTOR_BOARDS = 6;
  std::array<std::string, N_MOTOR_BOARDS> can_ports{"can0", "can1", "can2",
                                                    "can3", "can4", "can5"};

  typedef std::array<std::shared_ptr<blmc_drivers::CanBusMotorBoard>,
                     N_MOTOR_BOARDS>
      MotorBoards;

  // setup can buses -----------------------------------------------------
  std::array<std::shared_ptr<blmc_drivers::CanBus>, N_MOTOR_BOARDS> can_buses;
  for (size_t i = 0; i < can_buses.size(); i++) {
    can_buses[i] = std::make_shared<blmc_drivers::CanBus>(can_ports[i]);
  }

  // set up motor boards -------------------------------------------------
  MotorBoards motor_boards;
  for (size_t i = 0; i < motor_boards.size(); i++) {
    motor_boards[i] = std::make_shared<blmc_drivers::CanBusMotorBoard>(
        can_buses[i], 1000, 10);
    /// \TODO: reduce the timeout further!!
  }

  while(!StopDemos){
      for (size_t i = 0; i < motor_boards.size(); i++) {
          rt_printf("Motor boards %ld\n", i);
          motor_boards[i]->print_status();
          usleep(1000);
      }
  }
}