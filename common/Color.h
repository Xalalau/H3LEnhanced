#ifndef COMMON_COLOR_H
#define COMMON_COLOR_H

#include <cstdint>
#include <cstring>

/**
*	Makes a 4 byte RGB color. A is set to 255.
*/
inline constexpr uint32_t MakeRGB( uint8_t r, uint8_t g, uint8_t b )
{
	return static_cast< uint32_t >( ( ( r & 0xFF ) ) | ( ( g & 0xFF ) << 8 ) | ( ( b & 0xFF ) << 16 ) | ( 0xFF << 24 ) );
}

/**
*	Represents a HUD color. - Solokiller
*	Note: not the same as vgui::Color. That has a scheme color member in addition to the RGBA data.
*/
struct Color final
{
	static const size_t NUM_COMPONENTS = 4;

	/**
	*	Constructor.
	*	Creates a color from a 32 bit color value. Layout is | Unused | R | G | B |
	*	Defaults to white.
	*/
	Color( uint32_t color32 = MakeRGB( 255, 255, 255 ) )
		: color32( color32 )
	{
	}

	/**
	*	Constructor.
	*	Creates a color from a red, green and blue color.
	*/
	Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
		: rgba( r, g, b, a )
	{
	}

	Color( const Color& other ) = default;
	Color& operator=( const Color& other ) = default;

	/**
	*	Unpacks this color into r, g and b components.
	*	Obsolete. Directly accessing the color components is easier.
	*/
	void UnpackRGB( int& r, int& g, int& b ) const
	{
		r = this->rgba.r;
		g = this->rgba.g;
		b = this->rgba.b;
	}

	/**
	*	Sets a color from a 32 bit color value. Layout is | Unused | R | G | B |
	*/
	void Set( uint32_t color32In )
	{
		this->color32 = color32In;
	}

	/**
	*	Sets a color from a red, green and blue color.
	*/
	void Set( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
	{
		this->rgba.r = r;
		this->rgba.g = g;
		this->rgba.b = b;
		this->rgba.a = a;
	}

	uint8_t operator[]( const size_t uiIndex ) const
	{
		return reinterpret_cast<const uint8_t*>( this )[ uiIndex ];
	}

	uint8_t& operator[]( const size_t uiIndex )
	{
		return reinterpret_cast<uint8_t*>( this )[ uiIndex ];
	}

	uint8_t r() const { return rgba.r; }
	uint8_t g() const { return rgba.g; }
	uint8_t b() const { return rgba.b; }
	uint8_t a() const { return rgba.a; }

	void r( uint8_t r ) { rgba.r = r; }
	void g( uint8_t g ) { rgba.g = g; }
	void b( uint8_t b ) { rgba.b = b; }
	void a( uint8_t a ) { rgba.a = a; }

	struct rgba_t
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		rgba_t( uint8_t rIn, uint8_t gIn, uint8_t bIn, uint8_t aIn )
			: r( rIn )
			, g( gIn )
			, b( bIn )
			, a( aIn )
		{
		}
	};

	union
	{
		uint32_t color32;

		rgba_t rgba;
	};
};

/**
*	Default main HUD color.
*/
#define RGB_YELLOWISH MakeRGB( 255, 160, 0 )

/**
*	Default empty / nearly empty item color.
*/
#define RGB_REDISH MakeRGB( 255, 16, 16 )

/**
*	Default ammo bar color in weapon list.
*/
#define RGB_GREENISH MakeRGB( 0, 160, 0 )

// ############ hu3lifezado ############ //
// Novas cores
#define RGB_WHITE MakeRGB( 255, 255, 255 )
#define RGB_WHITEISH MakeRGB( 220, 220, 220 )
#define RGB_GRAYISH MakeRGB( 169, 169, 169 )
// ############ //

#endif //COMMON_COLOR_H
