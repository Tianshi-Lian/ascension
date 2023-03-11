#pragma once

namespace erika::plugins {

enum class Plugin_Type : u32 {
    Unknown = 0,
    Renderer = 1,
    Count
};

class Plugin {
  public:
    // TODO: replace passing in an explicit type with using sub dirs in the game distribution folder
    // /t and using magic_enum to convert the sub dir name into a corresponding Plugin_Type. This will
    // /t require us to move to use LoadLibrary (win32 (and linux equiv)) instead of relying on the
    // /t current behaviour of dumping all shared libs into the same dir as the executable.
    explicit Plugin(Plugin_Type type, std::string name);
    virtual ~Plugin() = default;

    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    [[nodiscard]] const std::string& get_name() const;
    [[nodiscard]] const Plugin_Type& get_type() const;

    Plugin(const Plugin&) = default;
    Plugin(Plugin&&) = delete;
    Plugin& operator=(const Plugin&) = default;
    Plugin& operator=(Plugin&&) = delete;

  private:
    Plugin_Type m_type;
    std::string m_name;
};

template<typename T>
class Plugin_Factory {
  public:
    Plugin_Factory<T>() = default;
    virtual ~Plugin_Factory<T>() = default;

    virtual std::shared_ptr<T> create() = 0;

    Plugin_Factory(const Plugin_Factory&) = delete;
    Plugin_Factory(Plugin_Factory&&) = delete;
    Plugin_Factory& operator=(const Plugin_Factory&) = delete;
    Plugin_Factory& operator=(Plugin_Factory&&) = delete;
};

}
