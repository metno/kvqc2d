
#ifndef INTERPOLATIONERROR_HH
#define INTERPOLATIONERROR_HH 1

class InterpolationError : public std::runtime_error {
public:
    InterpolationError(Message::Level level, const std::string& what)
        : std::runtime_error(what), mLevel(level) { }

    Message::Level level() const
        { return mLevel; }

private:
    Message::Level mLevel;
};

#endif /* INTERPOLATIONERROR_HH */
