#ifndef GAME_CLIENT_UI_SHARED_CLOCALIZE_H
#define GAME_CLIENT_UI_SHARED_CLOCALIZE_H

/**
*	Handles localization for the Hud.
*/
class CLocalize final
{
public:
	CLocalize() = default;

	/**
	*	Searches through the string for any msg names( indicated by a '#' )
	*	any found are looked up in titles.txt and the new message substituted
	*	the new value is pushed into dst_buffer
	*/
	char* LocaliseTextString( const char* msg, char* dst_buffer, int buffer_size );

	/**
	*	As above, but with a local static buffer
	*/
	char* BufferedLocaliseTextString( const char* msg );

	/**
	*	Simplified version of LocaliseTextString;  assumes string is only one word
	*/
	const char* LookupString( const char* msg_name, int* msg_dest = nullptr );

private:
	CLocalize( const CLocalize& ) = delete;
	CLocalize& operator=( const CLocalize& ) = delete;
};

CLocalize& Localize();

#endif //GAME_CLIENT_UI_SHARED_CLOCALIZE_H
