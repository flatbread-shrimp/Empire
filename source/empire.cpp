#include "../include/simulation.hpp"
int main()
{
  auto simulation = Simulation(1280, 720);
  do
  {
    simulation.poll_events();

    simulation.update();

    simulation.render();
  }while(!simulation.should_close());
  return 0;
}