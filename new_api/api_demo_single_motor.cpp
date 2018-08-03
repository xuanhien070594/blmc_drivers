#include <new_api.hpp>

class Controller
{
private:
    std::shared_ptr<Motor> motor_;

public:
    Controller(std::shared_ptr<Motor> motor): motor_(motor) { }

    void start_loop()
    {
        osi::start_thread(&Controller::loop, this);
    }

    static void
#ifndef __XENO__
    *
#endif
    loop(void* instance_pointer)
    {
        ((Controller*)(instance_pointer))->loop();
    }

    void loop()
    {
        Timer<10> time_logger("controller", 1000);
        while(true)
        {
            double current_target = 0.5;
            motor_->set_current_target(current_target);

            // print -----------------------------------------------------------
            Timer<>::sleep_ms(1);
            time_logger.end_and_start_interval();
            if ((time_logger.count() % 1000) == 0)
            {
                osi::print_to_screen("sending current: %f\n", current_target);
            }
        }
    }
};




int main(int argc, char **argv)
{  
    osi::initialize_realtime_printing();

    // create bus and boards -------------------------------------------------
    auto can_bus1 = std::make_shared<XenomaiCanbus>("rtcan0");
    auto board1 = std::make_shared<XenomaiCanMotorboard>(can_bus1);

    // create motors and sensors ---------------------------------------------
    auto motor_1 = std::make_shared<Motor>(board1, BLMC_MTR1);

    Controller controller1(motor_1);

    // somehow this is necessary to be able to use some of the functionality
    osi::make_this_thread_realtime();
    board1->enable();
    controller1.start_loop();

    while(true)
    {
        Timer<>::sleep_ms(10);
    }

    return 0;
}