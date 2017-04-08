#include "cl_dll.h"
#include "cl_util.h"

#include "CLocalize.h"

static CLocalize g_Localize;

CLocalize& Localize()
{
	return g_Localize;
}

char* CLocalize::LocaliseTextString( const char* msg, char* dst_buffer, int buffer_size )
{
	char* dst = dst_buffer;
	for( const char* src = msg; *src != '\0' && buffer_size > 0; buffer_size-- )
	{
		if( *src == '#' )
		{
			// cut msg name out of string
			static char word_buf[ 255 ];
			char* wdst = word_buf;

			const char* word_start = src;

			for( ++src; ( *src >= 'A' && *src <= 'z' ) || ( *src >= '0' && *src <= '9' ); ++wdst, ++src )
			{
				*wdst = *src;
			}
			*wdst = '\0';

			// lookup msg name in titles.txt
			client_textmessage_t *clmsg = TextMessageGet( word_buf );
			if( !clmsg || !( clmsg->pMessage ) )
			{
				src = word_start;
				*dst = *src;
				++dst, ++src;
				continue;
			}

			// copy string into message over the msg name
			for( const char* wsrc = clmsg->pMessage; *wsrc != '\0'; ++wsrc, ++dst )
			{
				*dst = *wsrc;
			}
			*dst = '\0';
		}
		else
		{
			*dst = *src;
			++dst, ++src;
			*dst = '\0';
		}
	}

	dst_buffer[ buffer_size - 1 ] = '\0'; // ensure null termination
	return dst_buffer;
}

char* CLocalize::BufferedLocaliseTextString( const char* msg )
{
	static char dst_buffer[ 1024 ];
	LocaliseTextString( msg, dst_buffer, sizeof( dst_buffer ) );
	return dst_buffer;
}

const char* CLocalize::LookupString( const char* msg, int* msg_dest )
{
	if( !msg )
		return "";

	// '#' character indicates this is a reference to a string in titles.txt, and not the string itself
	if( msg[ 0 ] == '#' )
	{
		// this is a message name, so look up the real message
		client_textmessage_t *clmsg = TextMessageGet( msg + 1 );

		if( !clmsg || !( clmsg->pMessage ) )
			return msg; // lookup failed, so return the original string

		if( msg_dest )
		{
			// check to see if titles.txt info overrides msg destination
			// if clmsg->effect is less than 0, then clmsg->effect holds -1 * message_destination
			if( clmsg->effect < 0 )  // 
				*msg_dest = -clmsg->effect;
		}

		return clmsg->pMessage;
	}
	else
	{  // nothing special about this message, so just return the same string
		return msg;
	}
}
