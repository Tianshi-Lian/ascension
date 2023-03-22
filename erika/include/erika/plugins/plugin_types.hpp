#pragma once

namespace erika::plugins {

/**
 * @enum Plugin_Type
 *
 * @brief Specifies the types of plugins available
 */
enum class Plugin_Type : u32 {
    Unknown = 0,
    Renderer = 1
};

/**
 * @class Plugin
 *
 * @brief A base class for any erika engine plugin. All plugins which should be handled automatically by the Plugin_Manager
 * should derive from this base.
 */
class Plugin {
  public:
    explicit Plugin(Plugin_Type type)
      : m_type(type)
    {
    }
    virtual ~Plugin() = default;

    /**
     * @brief Shutdown this plugin.
     */
    virtual void shutdown() = 0;

    /**
     * @brief Get the type of this plugin.
     *
     * @returns     Plugin_Type specifying the type of this plugin
     */
    [[nodiscard]] const Plugin_Type& get_type() const;

    Plugin(const Plugin&) = default;
    Plugin(Plugin&&) = delete;
    Plugin& operator=(const Plugin&) = default;
    Plugin& operator=(Plugin&&) = delete;

  private:
    Plugin_Type m_type;
};

/**
 * @class Plugin_Factory
 *
 * @brief A base class for any erika plugin factories. All plugins which should be handled automatically by the Plugin_Manager
 * must have a respective Plugin_Factory for their type.
 */
template<typename T>
class Plugin_Factory {
  public:
    Plugin_Factory<T>() = default;
    virtual ~Plugin_Factory<T>() = default;

    /**
     * @brief Create a new plugin.
     *
     * @returns     a shared pointer containing a new plugin of the type handled by this factory
     */
    virtual std::shared_ptr<T> create() = 0;

    Plugin_Factory(const Plugin_Factory&) = delete;
    Plugin_Factory(Plugin_Factory&&) = delete;
    Plugin_Factory& operator=(const Plugin_Factory&) = delete;
    Plugin_Factory& operator=(Plugin_Factory&&) = delete;
};

}
