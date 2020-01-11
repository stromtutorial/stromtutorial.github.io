#pragma once    ///start

#include <ctime>
#include <memory>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/gamma_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace strom {

    class Lot {
        public:
                                            Lot();
                                            ~Lot();
            
            void                            setSeed(unsigned seed);
            double                          uniform();
            int                             randint(int low, int high);
            double                          normal();
            double                          gamma(double shape, double scale);
            double                          logUniform();
            
            typedef std::shared_ptr<Lot>    SharedPtr;

        private:
        
            typedef boost::variate_generator<boost::mt19937 &, boost::random::uniform_01<> >                uniform_variate_generator_t;
            typedef boost::variate_generator<boost::mt19937 &, boost::random::normal_distribution<> >       normal_variate_generator_t;
            typedef boost::variate_generator<boost::mt19937 &, boost::random::gamma_distribution<> >        gamma_variate_generator_t;
            typedef boost::variate_generator<boost::mt19937 &, boost::random::uniform_int_distribution<> >  uniform_int_generator_t;

            unsigned                                        _seed;
            boost::mt19937                                  _generator;
            std::shared_ptr<uniform_variate_generator_t>    _uniform_variate_generator;
            std::shared_ptr<normal_variate_generator_t>     _normal_variate_generator;
            std::shared_ptr<gamma_variate_generator_t>      _gamma_variate_generator;
            std::shared_ptr<uniform_int_generator_t>        _uniform_int_generator;

            double                                          _gamma_shape;
            int                                             _low;
            int                                             _high;
    };
    
    // member function bodies go here
    ///end_class_declaration
    inline Lot::Lot() : _seed(0), _gamma_shape(1.0), _low(0), _high(100) {  ///begin_constructor
        //std::cout << "Constructing a Lot" << std::endl;
        _generator.seed(static_cast<unsigned int>(std::time(0)));
        _uniform_variate_generator = std::shared_ptr<uniform_variate_generator_t>(new uniform_variate_generator_t(_generator, boost::random::uniform_01<>()));
        _normal_variate_generator = std::shared_ptr<normal_variate_generator_t>(new normal_variate_generator_t(_generator, boost::random::normal_distribution<>()));
        _gamma_variate_generator = std::shared_ptr<gamma_variate_generator_t>(new gamma_variate_generator_t(_generator, boost::random::gamma_distribution<>(_gamma_shape)));
        _uniform_int_generator = std::shared_ptr<uniform_int_generator_t>(new uniform_int_generator_t(_generator, boost::random::uniform_int_distribution<>(_low, _high)));
    }
        
    inline Lot::~Lot() {
        //std::cout << "Destroying a Lot" << std::endl;
        _uniform_variate_generator.reset();
        _normal_variate_generator.reset();
        _gamma_variate_generator.reset();
        _uniform_int_generator.reset();
    }   ///end_destructor
        
    inline void Lot::setSeed(unsigned seed) {   ///begin_setSeed
        _seed = seed;
        _generator.seed(_seed > 0 ? _seed : static_cast<unsigned int>(std::time(0)));
    }   ///end_setSeed
        
    inline double Lot::uniform() {  ///begin_uniform
        return (*_uniform_variate_generator)();
        while (u <= 0.0)
            u = (*_uniform_variate_generator)();
    }   ///end_uniform

    inline double Lot::logUniform() {   ///begin_logUniform
        double u = (*_uniform_variate_generator)();
        while (u <= 0.0)
            u = (*_uniform_variate_generator)();
        return std::log(u);
    }   ///end_logUniform
    
    inline double Lot::normal() {   ///begin_normal
        return (*_normal_variate_generator)();
    }   ///end_normal

    inline double Lot::gamma(double shape, double scale) {  ///begin_gamma
        assert(shape > 0.0);
        assert(scale > 0.0);
        if (shape != _gamma_shape) {
            _gamma_shape = shape;
            _gamma_variate_generator.reset(new gamma_variate_generator_t(_generator, boost::random::gamma_distribution<>(_gamma_shape,1)));
        }
        double deviate = (*_gamma_variate_generator)();
        return scale*deviate;
    }   ///end_gamma

    inline int Lot::randint(int low, int high) {    ///begin_randint
        if (low != _low || high != _high) {
            _low  = low;
            _high = high;
            _uniform_int_generator.reset(new uniform_int_generator_t(_generator, boost::random::uniform_int_distribution<>(_low,_high)));
        }
        return (*_uniform_int_generator)();
    }   ///end_randint
    
}   ///end
