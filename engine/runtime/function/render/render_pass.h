#pragma once


namespace rtr {
    
class Render_pass {

public:
    Render_pass() {}
    virtual ~Render_pass() {}
    virtual void excute() = 0;
};

class Main_pass : public Render_pass {

};

class Gamma_pass : public Render_pass {

};



}