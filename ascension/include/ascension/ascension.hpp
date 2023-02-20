#pragma once

namespace ascension {

class Ascension {
    public:
    Ascension() = default;
    ~Ascension() = default;

    Ascension(const Ascension&) = default;
    Ascension(Ascension&&) = delete;
    Ascension& operator=(const Ascension&) = default;
    Ascension& operator=(Ascension&&) = delete;

    void initialize();
    void run();
};

}   // namespace ascension
