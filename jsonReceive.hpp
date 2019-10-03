#pragma once
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdexcept>
#include "Agent-Action.hpp"

class Board;

class jsonReceive{
    public:
      static Board* jsonRead(int ourTeamID,std::string jsonName);
};