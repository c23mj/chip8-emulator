#include <cstdint>
class Timers{

public:
    uint8_t delay_timer_;
    uint8_t sound_timer_;

    Timers(uint8_t delay_timer = 0, uint8_t sound_timer = 0): delay_timer_(delay_timer), sound_timer_(sound_timer){

    }

    void update_delay_timer(){
        if (delay_timer_ > 0){
            delay_timer_--;
        }
    }

    bool update_sound_timer(){
        bool beep = false;
        if (sound_timer_ > 0){
            beep = true;
            sound_timer_--;
        } 
        return beep;
    }

};