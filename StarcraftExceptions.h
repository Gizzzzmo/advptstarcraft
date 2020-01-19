#pragma once

#include <exception>
class noMineralsException : public std::exception {
    const char * what () const throw () {
      return "noMins";
   }
};

class noGasException : public std::exception {
    const char * what () const throw () {
      return "noGas";
   }
};

class noSupplyException : public std::exception {
    const char * what () const throw () {
      return "noSupply";
   }
};

class noProducerAvailableException : public std::exception {
    const char * what () const throw () {
      return "noProducer";
   }
};

class requirementNotFulfilledException : public std::exception {
    const char * what () const throw () {
      return "noProducer";
   }
};

class noGasGeyserAllowedException : public std::exception {
    const char * what () const throw () {
      return "notEnoughBases";
   }
};