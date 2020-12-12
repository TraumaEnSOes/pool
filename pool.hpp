#ifndef POOL_HPP
#define POOL_HPP

#include <limits>
#include <stdexcept>
#include <variant>
#include <vector>

template< typename T, typename ALLOC = std::allocator< T > > class Pool {
public:
    using index_type = size_t;
    using value_type = T;
    using allocator = ALLOC;
    using reference = value_type &;
    using const_reference = const value_type &;

    Pool( ) = default;
    Pool( const Pool & ) = default;
    Pool( Pool && ) = default;

    reference operator[]( index_type idx ) {
        return std::get< 1 >( m_vector[idx] );
    }

    const_reference operator[]( index_type idx ) const {
        return std::get< 1 >( m_vector[idx] );
    }

    template< typename... ARGS > index_type emplace( ARGS... args ) {
        if( m_next == InvalidNext ) {
            // Hay que aumentar el tamaño del contenedor.
            m_vector.resize( m_vector.size( ) + 1 );

            auto &item = std::get< 0 >( m_vector.back( ) );
            item = m_next;
            m_next = m_vector.size( ) - 1;
        }

        // Obtenemos el id del elemento a usar.
        auto resultIdx = m_next;
        auto &item = m_vector[resultIdx];
        // Establecemos el siguiente libre.
        m_next = std::get< 0 >( item );
        // Instanciamos el tipo.
        item.template emplace< 1 >( std::forward< ARGS >( args )... );

        return resultIdx;
    }

    void free( index_type idx ) {
        if( idx >= m_vector.size( ) ) {
            throw std::runtime_error( "pool: try free invalid index" );
        }

        if constexpr( std::is_trivially_destructible< value_type >::value != false ) {
            // Es necesario destruir la instancia.
            value_type &value = std::get< 1 >( m_vector[idx] );
            value.~value_type( );
        }

        // Colocamos el elemento en la cola de disponibles.
        m_vector[idx].template emplace< 0 >( m_next );
        m_next = idx;
    }

private:
    using Node = std::variant< size_t, value_type >;
    using RebindAlloc = typename std::allocator_traits< ALLOC >::template rebind_alloc< Node >;
    using VectorType = std::vector< Node, RebindAlloc >;

    static constexpr size_t InvalidNext = std::numeric_limits< size_t >::max( );

    size_t m_next = InvalidNext;
    VectorType m_vector;
};

#endif
