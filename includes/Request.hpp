/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mflores- <mflores-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/15 16:46:07 by mflores-          #+#    #+#             */
/*   Updated: 2024/02/02 16:57:32 by mflores-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

# define MAX_URI_SIZE       4096 //characters
# define MAX_HEADER_SIZE    8192 // 5 kilobytes


class   Request
{
    public:

        Request( void );
        ~Request( void );

        enum    ParseState
        {
            PARSE_REQUEST_LINE,
            PARSE_HEADERS,
            PARSE_BODY,
            PARSE_COMPLETE
        };

        void                parseRequest( std::string & request, size_t pos );

        ParseState          getParseState( void ) const;
        void                setParseState( ParseState state );
        std::string         getMethod( void ) const;
        std::string         getUri( void ) const;
        std::string         getQuery( void ) const;
        std::string         getHttpVersion( void ) const;

        const_string_map    getHeaders( void ) const;
        std::string         getHostname( void ) const;
        std::string         getPort( void ) const;
        std::string         getBody( void ) const;
        HttpCode            getCode( void ) const;
        HttpMethod          getHttpMethod( void ) const;
        void                setCode( HttpCode code );
        void			    setConnection( bool n );
        void                resetClassMembers( void );

        bool                keepConnectionAlive( void ) const;
        bool                isReqAlreadyShitty( void ) const;
        bool                strHasSpace( const std::string & str );
        
        std::string         getTokenValue( const std::string & token ) const;
        
    private:

        static std::string  HTTP_METHODS_SUPPORTED[];
        Request::ParseState _parseState; // flag to know the state of the request
        std::string         _method; // MANDATORY FOR HTTP/1.1
        HttpMethod          _httpMethod; // enum after checking _method
        std::string         _uri; // MANDATORY FOR HTTP/1.1: uri received from request
        std::string         _query; // if theres a ? in URI is must probably CGI
        std::string         _httpVersion; // MANDATORY FOR HTTP/1.1
        string_map          _headers; // tokens and values
        std::string         _hostname; // MANDATORY FOR HTTP/1.1: string with the hostname from header "Host" ex. localhost:8080 => _hostname = localhost
        std::string         _port; // string with the port from header "Host" if exists ex. localhost:8080 => _port = 8080 or "" if it doesnt have one
        std::string         _body; // string with the entire body recovered
        bool                _keepConnection; // flag to know if we should keep the connection alive or not depending on _code and client request
        HttpCode            _code; // flag for potential 400 BAD REQUEST, 405 METHOD NOT ALLOWED or 505 HTTP VERSION NOT SUPPORTED
        size_t              _strPos;

        void                parseFirstLine( const std::string & firstLine );
        void                findQuery( void );
        size_t	            supported_extension( std::string & str );
        void                checkMethod( void );
        void                parseHeaders( const std::string & request );
        void                checkHost( void );
        bool                doEndsMatch( const std::string & str, const std::string & end );
        void                readBodyWithChunkedEncoding( const std::string & chunks );
        void                readBodyWithContentLength( const std::string & request, size_t pos, size_t contentLength );
        void                extractBody( const std::string & request, size_t pos );

        bool			    isInvalidChar( const std::string & key );
        void                startTrim( std::string & s );
        void                endTrim( std::string & s );
        void                trimSpacesBothEnds( std::string & s );
        void                toLowerInline( std::string & str );
        void                trimAndLower( std::string & str );
        int                 hexStringToInt(const std::string& hexString);

};

std::ostream &		        operator<<( std::ostream & os, const Request & re );

# endif