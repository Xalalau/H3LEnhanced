#ifndef COMMON_CBITSET_H
#define COMMON_CBITSET_H

/**
*	A bitset designed to be usable in entvars_t as a replacement for bit vectors.
*	std::bitset is not an option because it doesn't have all of the required methods, and doesn't allow you to specify the underlying type.
*/
template<typename T>
class CBitSet
{
public:
	typedef T Type_t;

	explicit CBitSet( T value = 0 );

	CBitSet( const CBitSet& other ) = default;
	CBitSet& operator=( const CBitSet& ) = default;

	CBitSet& operator=( const T flags );

	/**
	*	@param flags Flags to check.
	*	@return Whether any of the given flags are set.
	*/
	bool Any( const T flags ) const;

	/**
	*	@param flags Flags to check.
	*	@return Whether all of the given flags are set.
	*/
	bool All( const T flags ) const;

	/**
	*	@brief Whether no flags are set
	*/
	bool None() const { return m_Data == 0; }

	/**
	*	@return The bit vector.
	*/
	inline T Get() const { return m_Data; }

	/**
	*	@return The bit vector.
	*/
	inline T& Get() { return m_Data; }

	/**
	*	Sets the bit vector flags to the given states.
	*	@param flags Flags to set.
	*/
	void Set( const T flags );

	/**
	*	Adds the given flags.
	*	@param flags Flags to set.
	*/
	void AddFlags( const T flags );

	/**
	*	Clears all flags.
	*/
	void ClearAll();

	/**
	*	Clears the given flags.
	*	@param flags Flags to clear.
	*/
	void ClearFlags( const T flags );

	//operators

	inline operator T() const { return m_Data; }

	CBitSet& operator|=( const T flags );

	CBitSet& operator&=( const T flags );

	CBitSet& operator^=( const T flags );

	CBitSet operator|( const T flags ) const;

	CBitSet operator&( const T flags ) const;

	CBitSet operator^( const T flags ) const;

	CBitSet operator~() const;

	bool operator==( const CBitSet& other ) const;

	bool operator!=( const CBitSet& other ) const;

	bool operator==( const T other ) const;

	bool operator!=( const T other ) const;

	bool operator!() const;

private:
	T m_Data;
};

template<typename T>
inline CBitSet<T>::CBitSet( T value )
	: m_Data( value )
{
}

template<typename T>
inline CBitSet<T>& CBitSet<T>::operator=( const T flags )
{
	m_Data = flags;

	return *this;
}

template<typename T>
inline bool CBitSet<T>::Any( const T flags ) const
{
	return ( m_Data & flags ) != 0;
}

template<typename T>
inline bool CBitSet<T>::All( const T flags ) const
{
	return ( m_Data & flags ) == flags;
}

template<typename T>
inline void CBitSet<T>::Set( const T flags )
{
	m_Data = flags;
}

template<typename T>
inline void CBitSet<T>::AddFlags( const T flags )
{
	m_Data |= flags;
}

template<typename T>
inline void CBitSet<T>::ClearAll()
{
	m_Data = 0;
}

template<typename T>
inline void CBitSet<T>::ClearFlags( const T flags )
{
	m_Data &= ~flags;
}

template<typename T>
inline CBitSet<T>& CBitSet<T>::operator|=( const T flags )
{
	m_Data |= flags;

	return *this;
}

template<typename T>
inline CBitSet<T>& CBitSet<T>::operator&=( const T flags )
{
	m_Data &= flags;

	return *this;
}

template<typename T>
inline CBitSet<T>& CBitSet<T>::operator^=( const T flags )
{
	m_Data ^= flags;

	return *this;
}

template<typename T>
inline CBitSet<T> CBitSet<T>::operator|( const T flags ) const
{
	return CBitSet<T>( m_Data | flags );
}

template<typename T>
inline CBitSet<T> CBitSet<T>::operator&( const T flags ) const
{
	return CBitSet<T>( m_Data & flags );
}

template<typename T>
inline CBitSet<T> CBitSet<T>::operator^( const T flags ) const
{
	return CBitSet<T>( m_Data ^ flags );
}

template<typename T>
inline CBitSet<T> CBitSet<T>::operator~() const
{
	return CBitSet<T>( ~m_Data );
}

template<typename T>
inline bool CBitSet<T>::operator==( const CBitSet<T>& other ) const
{
	return m_Data == other.m_Data;
}

template<typename T>
inline bool CBitSet<T>::operator!=( const CBitSet<T>& other ) const
{
	return m_Data != other.m_Data;
}

template<typename T>
inline bool CBitSet<T>::operator==( const T other ) const
{
	return m_Data == other;
}

template<typename T>
inline bool CBitSet<T>::operator!=( const T other ) const
{
	return m_Data != other;
}

template<typename T>
inline bool CBitSet<T>::operator!() const
{
	return !m_Data;
}

#endif //COMMON_CBITSET_H
