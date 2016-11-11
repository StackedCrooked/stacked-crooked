#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H


enum class Filter
{
    BPF,
    Native,
    Mask,
    Vector
};


struct Options
{
    Filter filter = Filter::BPF;
    int packet_size = 1536;
    int prefetch = 0;
};


int initialize_program_options(int argc, char** argv, Options& options);


#endif // PROGRAMOPTIONS_H
