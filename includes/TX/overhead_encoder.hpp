#pragma once

#include <vector>
#include <iostream>

/*append to packet overheads (CRC, sync_seq)*/
class overhead_encoder{   
    public:
        std::vector<int16_t> generate_barker_code(const int len);

        /*barker code support len=2,3,4,5,7,11,13*/
        std::vector<int16_t> add_barker_to_message(const std::vector<int16_t>& bits, const std::vector<int16_t>& barker_code);
        
        /*TODO: add CRC to message*/
};  