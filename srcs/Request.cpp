/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mflores- <mflores-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/15 16:46:00 by mflores-          #+#    #+#             */
/*   Updated: 2024/02/02 18:41:33 by mflores-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

std::string Request::HTTP_METHODS_SUPPORTED[] = {
	"GET",
	"POST",
	"DELETE",
	"PUT",
	"PATCH",
	"HEAD",
	"OPTIONS"
};

size_t	Request::supported_extension( std::string & str )
{
	size_t i;
	for (i = 0; i < sizeof(HTTP_METHODS_SUPPORTED) / sizeof(HTTP_METHODS_SUPPORTED[0]); i++) {
		if (str.compare(HTTP_METHODS_SUPPORTED[i]) == 0) {
			return (i);
		}
	}
	return (i);
}

void	Request::checkMethod( void )
{
	size_t i = supported_extension(this->_method);
	if (i == 7)
		setCode(_BAD_REQUEST);
	else if (i > 2)
		setCode(_METHOD_NOT_ALLOWED);
	else
	{
		if (this->_method.compare("GET") == 0)
			this->_httpMethod = _GET;
		else if (this->_method.compare("POST") == 0)
			this->_httpMethod = _POST;
		else if (this->_method.compare("DELETE") == 0)
			this->_httpMethod = _DELETE;
	}
		
}


/******************************************************************************/
/*	CONSTRUCTORS & DESTRUCTORS                                                */
/******************************************************************************/

Request::Request( void ) :
	_parseState(PARSE_REQUEST_LINE),
	_method(""),
	_httpMethod(_METHOD_NOT_SET),
	_uri(""),
	_query(""),
	_httpVersion(""),
	_hostname(""),
	_port(""),
	_body(""),
	_keepConnection(true),
	_code(_CODE_NOT_SET),
	_strPos(0)
{
    return ;
}

Request::~Request( void ) {return ;}

/******************************************************************************/
/*	SETTERS & GETTERS                                                         */
/******************************************************************************/

Request::ParseState		Request::getParseState( void ) const {return (this->_parseState);}

void                	Request::setParseState(ParseState state) { this->_parseState = state; }

std::string				Request::getMethod( void ) const {return (this->_method);}

std::string				Request::getUri( void ) const {return (this->_uri);}

std::string				Request::getQuery( void ) const {return (this->_query);}

std::string				Request::getHttpVersion( void ) const {return (this->_httpVersion);}

const_string_map		Request::getHeaders( void ) const {return (this->_headers);}

std::string				Request::getHostname( void ) const {return (this->_hostname);}

std::string				Request::getPort( void ) const {return (this->_port);}

std::string				Request::getBody( void ) const {return (this->_body);}

HttpCode				Request::getCode( void ) const {return (this->_code);}

HttpMethod				Request::getHttpMethod( void ) const {return (this->_httpMethod);}

void			Request::setCode( HttpCode code )
{
	if (this->_code == _CODE_NOT_SET)
		this->_code = code;
	return ;
}

void			Request::setConnection( bool n )
{
	this->_keepConnection = n;
	return ;
}

bool			Request::keepConnectionAlive( void ) const {return (this->_keepConnection);}

bool			Request::isReqAlreadyShitty( void ) const {return (this->_code != _CODE_NOT_SET);}

void			Request::resetClassMembers( void )
{
	this->_parseState = PARSE_REQUEST_LINE;
	this->_method.clear();
	this->_uri.clear();
	this->_query.clear();
	this->_httpVersion.clear();
	this->_headers.clear();
	this->_hostname.clear();
	this->_port.clear();
	this->_body.clear();
	this->_keepConnection = true;
	this->_code = _CODE_NOT_SET;
	this->_httpMethod = _METHOD_NOT_SET;
	this->_strPos = 0;
}

/******************************************************************************/
/*	METHODS                                                                   */
/******************************************************************************/

// Trim from start (in place)
void	Request::startTrim( std::string & s )
{
	std::string::iterator it = s.begin();

	while (it != s.end() && std::isspace(static_cast<unsigned char>(*it))) {
		++it;
	}

	s.erase(s.begin(), it);
}

// Trim from end (in place)
void	Request::endTrim( std::string & s )
{
	std::string::reverse_iterator rit = s.rbegin();

	while (rit != s.rend() && std::isspace(static_cast<unsigned char>(*rit))) {
		++rit;
	}

	s.erase(rit.base(), s.end());
}

// Trim from both ends (in place)
void	Request::trimSpacesBothEnds( std::string & s)
{
    startTrim(s);
    endTrim(s);
	return ;
}

void	Request::trimAndLower( std::string & str )
{
	trimSpacesBothEnds(str);
	toLowerInline(str);
}

void	Request::toLowerInline( std::string & str )
{
    std::transform(str.begin(), str.end(), str.begin(), tolower);
}

bool	Request::strHasSpace( const std::string & str )
{
    return (std::find(str.begin(), str.end(), ' ') != str.end());
}

/*
	RFC 7230, Section 3.1.1. The first line of an HTTP request
	has the following format: <HTTP-METHOD> <URI> <HTTP-VERSION>\r\n
*/
void	Request::parseFirstLine( const std::string & firstLine )
{
	std::istringstream	iss(firstLine);

	if (!(iss >> this->_method >> this->_uri >> this->_httpVersion) || !iss.eof())
		setCode(_BAD_REQUEST);
	else
	{
		this->trimSpacesBothEnds(this->_method);
		this->trimSpacesBothEnds(this->_uri);
		this->trimSpacesBothEnds(this->_httpVersion);
		this->checkMethod();

		if (this->_uri[0] != '/' || this->_uri.length() > MAX_URI_SIZE)
			setCode(_BAD_REQUEST);
		else if (this->_httpVersion != HTTP_VERSION_SUPPORTED)
			setCode(_HTTP_VERSION_NOT_SUPPORTED);
		this->findQuery();
	}
	return ;
}

/*
	RFC 7230, Section 5.4
	A client MUST include a Host header field in all HTTP/1.1 request messages.
*/
void	Request::checkHost( void )
{
	str_map_it it = this->_headers.find("host");
	if (it == this->_headers.end() || strHasSpace(it->second))
		setCode(_BAD_REQUEST);
	else 
	{
		size_t	delimiter = it->second.find(":");
		if (delimiter != std::string::npos)
		{
			this->_port = it->second.substr(delimiter + 1, it->second.size());
			if (this->_port.find_first_not_of("0123456789") != std::string::npos || this->_port.empty())
				setCode(_BAD_REQUEST);
		}
		this->_hostname = it->second.substr(0, delimiter);
		if (this->_hostname.empty())
			setCode(_BAD_REQUEST);
	}
	return ;
}

/*
	RFC 7230, Section 3.2: Field names are case-insensitive and only allow tokens.
	RFC 7230, Section 3.2: Field names not recognized or not allowed by the server can lead to a 400 Bad Request.
	RFC 7230, Section 3.2.2: A sender MUST NOT generate multiple header fields with the same field name in a message 
	unless either the entire field value for that header field is defined as a comma-separated list.

	RFC 7230, Section 3.3.2: A sender MUST NOT send a Content-Length header field in any message that contains a 
	Transfer-Encoding header field.
*/
void	Request::parseHeaders( const std::string & request )
{
	std::istringstream	iss(request);
	std::string			line;
	while (std::getline(iss, line) && line != "\r") {
		size_t delimiter = line.find(":");
		if (delimiter != std::string::npos) {
			std::string key = line.substr(0, delimiter);
			std::string value = line.substr(delimiter + 1);
			this->trimAndLower(key);
			this->trimSpacesBothEnds(value);
			if (key.empty() || value.empty() || (this->_headers.find(key) != this->_headers.end())
				|| key.length() > MAX_HEADER_SIZE || isInvalidChar(key) || strHasSpace(key))
				setCode(_BAD_REQUEST);
			this->_headers.insert(std::make_pair(key, value));
		} else
			setCode(_BAD_REQUEST);
	}

	if (this->getTokenValue("connection").compare("close") == 0)
		setConnection(false);

	this->checkHost();

	if (this->_httpMethod == _POST && (this->_headers.count("content-length") || this->_headers.count("transfer-encoding"))) {
		if ((this->_headers.count("content-length") && this->_headers.count("transfer-encoding"))
			|| (this->_headers.count("content-length") && this->_headers["content-length"].find_first_not_of("0123456789") != std::string::npos))
			setCode(_BAD_REQUEST);
		
		if (this->_code == _CODE_NOT_SET)
			this->_parseState = PARSE_BODY;
	} else
		this->_parseState = PARSE_COMPLETE;
	return ;
}

void	Request::findQuery( void )
{
	size_t		idx;

	idx = this->_uri.find_first_of('?');
	if (idx != std::string::npos)
	{
		this->_query = this->_uri.substr(idx + 1);
		this->_uri = this->_uri.substr(0, idx);
		if (this->_query.empty() || this->_uri.empty())
			setCode(_BAD_REQUEST);
	}
}

int		Request::hexStringToInt( const std::string & hexString )
{
    int intValue;

    std::istringstream(hexString) >> std::hex >> intValue;
    return (intValue);
}

/*
	Checks if end of str is equal to end passed in parameter
*/
bool	Request::doEndsMatch( const std::string & str, const std::string & end )
{
	size_t	i = str.size();
	size_t	j = end.size();

	while (j > 0)
	{
		i--;
		j--;
		if ((int)i < 0 || str[i] != end[j])
			return (false);
	}
	return (true);
}

void	Request::readBodyWithChunkedEncoding( const std::string & chunks)
{
	if (chunks.empty()) {
		std::cerr << RED "Error: chunks are empty\n" RESET;	
		setCode(_BAD_REQUEST);
		this->_parseState = PARSE_COMPLETE;
	}

	size_t	idx = 0;
    while (idx < chunks.size()) {
        size_t	endOfLine = chunks.find("\r\n", idx);
        if (endOfLine == std::string::npos) {
			setCode(_BAD_REQUEST);
			this->_parseState = PARSE_COMPLETE;
            std::cerr << RED "Error: Invalid chunked data format\n" RESET;
            return;
        }
        
        size_t	chunkSize = hexStringToInt(chunks.substr(idx, endOfLine - idx));

        if (chunkSize == 0) {
			this->_parseState = PARSE_COMPLETE; 
            return ;
        }

        idx = endOfLine + 2;
		
        if (idx + chunkSize + 2 > chunks.size()) {
			setCode(_BAD_REQUEST);
			this->_parseState = PARSE_COMPLETE;
            std::cerr << RED "Error: Invalid chunked data format\n" RESET;
            return;
        }

        this->_body += chunks.substr(idx, chunkSize);
        
        idx += chunkSize + 2;
    }
	return ;
}

void	Request::readBodyWithContentLength( const std::string & request, size_t pos, size_t contentLength )
{
	this->_body = request.substr(pos);
	if (this->_body.length() == contentLength)
		this->_parseState = PARSE_COMPLETE;
	else if (this->_body.length() > contentLength)
	{
		this->_parseState = PARSE_COMPLETE;
		setCode(_BAD_REQUEST);
	}
	return ;
}

void	Request::extractBody( const std::string & request, size_t pos )
{
	if (pos == std::string::npos)
	{
		this->_parseState = PARSE_COMPLETE;
		return ;
	}
	if (this->_headers.count("transfer-encoding")
		&& (this->_headers["transfer-encoding"].compare("chunked") == 0))
	{
		if (doEndsMatch(request.substr(pos), "0\r\n\r\n") == false)
		{
			return ;
		}
		this->readBodyWithChunkedEncoding(request.substr(pos));
	}
	else if (this->_headers.count("content-length"))
	{
		size_t contentLength = stringToInt(this->_headers["content-length"]);
		if ((int)contentLength < 0)
		{
			setCode(_BAD_REQUEST);
			this->_parseState = PARSE_COMPLETE;
			return ;
		}
		this->readBodyWithContentLength(request, pos, contentLength);
	}
	return ;
}

void	Request::parseRequest( std::string & request, size_t idx )
{
	if (this->getParseState() == Request::PARSE_REQUEST_LINE)
	{
		size_t firstLineEnd = request.find("\r\n");
		if (firstLineEnd != std::string::npos)
		{
			parseFirstLine(request.substr(0, firstLineEnd));
			this->_parseState = PARSE_HEADERS;
			this->_strPos = firstLineEnd + 2;
		}
	}
	if (this->getParseState() == Request::PARSE_HEADERS)
		parseHeaders(request.substr(this->_strPos, idx));
	if (this->getParseState() == Request::PARSE_BODY)
		extractBody(request, (idx + 4));
	return ;
}

std::ostream &		operator<<( std::ostream & os, const Request & re )
{
	const_str_map_it	it;

	os << CYAN << "=> members of class Request:\n" << RESET;
	os << "Method: " << re.getMethod() << '\n';
	os << "Uri: " << re.getUri() << '\n';
	os << "Query: " << re.getQuery() << '\n';
	os << "Http Version: " << re.getHttpVersion() << '\n';
	os << "Hostname: " << re.getHostname() << '\n';
	os << "Port: " << re.getPort() << '\n';
	os << "********************************************************" << '\n';
	for (it = re.getHeaders().begin(); it != re.getHeaders().end(); it++)
		os << YELLOW << it->first << ": " << CYAN << it->second << RESET << '\n';
	os << "********************************************************" << '\n';
	os << YELLOW << "Body:[" << RESET << re.getBody() << YELLOW << "]\n" << RESET;
	os << "Keep connection alive? " << (re.keepConnectionAlive() ? "YES" : "NO") << "\n";

	return (os);
}

// Returns the value of token map<string, string>, if not found, returns empty string
std::string		Request::getTokenValue( const std::string & token ) const
{
	const std::map<std::string, std::string>::const_iterator tok = this->getHeaders().find(token);
	if (tok == this->getHeaders().end())
		return ("");
	return (tok->second);
}

bool			Request::isInvalidChar( const std::string & key )
{
	if (key.empty())
		return (false);

	const char* invalidChars = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
								"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F";

	for (std::string::size_type i = 0; i < key.length(); i++)
	{
		char c = key[i];
		if (strchr(invalidChars, c) != NULL)
			return (true);
	}

	return (false);
}
