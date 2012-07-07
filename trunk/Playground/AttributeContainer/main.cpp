struct AttributeContainer
{
    template<typename T>
    bool hasAttribute() const
    {
        return mAttributes.find(&typeid(T)) != mAttributes.end();
    }

    template<typename T>
    T & getAttribute()
    {
        auto it = mAttributes.find(&typeid(T));
        if (it == mAttributes.end())
        {
            throw std::logic_error("Attribute not found.");
        }
        return *static_cast<T*>(it->second.get());
    }

    template<typename T>
    void setAttribute(const T & inValue)
    {
        mAttributes[&typeid(T)] = std::shared_ptr<void>(new T(inValue));
    }

private:
    std::map<const std::type_info*, std::shared_ptr<void> > mAttributes;
};

