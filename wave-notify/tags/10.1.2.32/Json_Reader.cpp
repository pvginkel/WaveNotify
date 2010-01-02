#include "stdafx.h"
#include "json/json_reader.h"
#include "json/json_value.h"
#include <utility>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>

#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning( disable : 4996 )   // disable warning about strdup being deprecated.
#endif

namespace Json {

static inline bool 
in( Reader::Char c, Reader::Char c1, Reader::Char c2, Reader::Char c3, Reader::Char c4 )
{
   return c == c1  ||  c == c2  ||  c == c3  ||  c == c4;
}

static inline bool 
in( Reader::Char c, Reader::Char c1, Reader::Char c2, Reader::Char c3, Reader::Char c4, Reader::Char c5 )
{
   return c == c1  ||  c == c2  ||  c == c3  ||  c == c4  ||  c == c5;
}


static bool 
containsNewLine( Reader::Location begin, 
                 Reader::Location end )
{
   for ( ;begin < end; ++begin )
      if ( *begin == L'\n'  ||  *begin == L'\r' )
         return true;
   return false;
}


// Class Reader
// //////////////////////////////////////////////////////////////////

Reader::Reader()
{
}

bool
Reader::parse( const std::wstring &document, 
               Value &root,
               bool collectComments )
{
   document_ = document;
   const wchar_t *begin = document_.c_str();
   const wchar_t *end = begin + document_.length();
   return parse( begin, end, root, collectComments );
}
/*
bool
Reader::parse( std::istream& sin,
               Value &root,
               bool collectComments )
{
   //std::istream_iterator<wchar_t> begin(sin);
   //std::istream_iterator<wchar_t> end;
   // Those would allow streamed input from a file, if parse() were a
   // template function.

   // Since std::wstring is reference-counted, this at least does not
   // create an extra copy.
   std::wstring doc;
   std::getline(sin, doc, (wchar_t)EOF);
   return parse( doc, root, collectComments );
}
*/
bool 
Reader::parse( const wchar_t *beginDoc, const wchar_t *endDoc, 
               Value &root,
               bool collectComments )
{
   begin_ = beginDoc;
   end_ = endDoc;
   collectComments_ = collectComments;
   current_ = begin_;
   lastValueEnd_ = 0;
   lastValue_ = 0;
   commentsBefore_ = L"";
   errors_.clear();
   while ( !nodes_.empty() )
      nodes_.pop();
   nodes_.push( &root );
   
   bool successful = readValue();
   Token token;
   skipCommentTokens( token );
   if ( collectComments_  &&  !commentsBefore_.empty() )
      root.setComment( commentsBefore_, commentAfter );
   return successful;
}


bool
Reader::readValue()
{
   Token token;
   skipCommentTokens( token );
   bool successful = true;

   if ( collectComments_  &&  !commentsBefore_.empty() )
   {
      currentValue().setComment( commentsBefore_, commentBefore );
      commentsBefore_ = L"";
   }


   switch ( token.type_ )
   {
   case tokenObjectBegin:
      successful = readObject( token );
      break;
   case tokenArrayBegin:
      successful = readArray( token );
      break;
   case tokenNumber:
      successful = decodeNumber( token );
      break;
   case tokenString:
      successful = decodeString( token );
      break;
   case tokenTrue:
      currentValue() = true;
      break;
   case tokenFalse:
      currentValue() = false;
      break;
   case tokenNull:
      currentValue() = Value();
      break;
   default:
      return addError( L"Syntax error: value, object or array expected.", token );
   }

   if ( collectComments_ )
   {
      lastValueEnd_ = current_;
      lastValue_ = &currentValue();
   }

   return successful;
}


void 
Reader::skipCommentTokens( Token &token )
{
   do
   {
      readToken( token );
   }
   while ( token.type_ == tokenComment );
}


bool 
Reader::expectToken( TokenType type, Token &token, const wchar_t *message )
{
   readToken( token );
   if ( token.type_ != type )
      return addError( message, token );
   return true;
}


bool 
Reader::readToken( Token &token )
{
   skipSpaces();
   token.start_ = current_;
   Char c = getNextChar();
   bool ok = true;
   switch ( c )
   {
   case L'{':
      token.type_ = tokenObjectBegin;
      break;
   case L'}':
      token.type_ = tokenObjectEnd;
      break;
   case L'[':
      token.type_ = tokenArrayBegin;
      break;
   case L']':
      token.type_ = tokenArrayEnd;
      break;
   case L'"':
      token.type_ = tokenString;
      ok = readString();
      break;
   case L'/':
      token.type_ = tokenComment;
      ok = readComment();
      break;
   case L'0':
   case L'1':
   case L'2':
   case L'3':
   case L'4':
   case L'5':
   case L'6':
   case L'7':
   case L'8':
   case L'9':
   case L'-':
      token.type_ = tokenNumber;
      readNumber();
      break;
   case L't':
      token.type_ = tokenTrue;
      ok = match( L"rue", 3 );
      break;
   case L'f':
      token.type_ = tokenFalse;
      ok = match( L"alse", 4 );
      break;
   case L'n':
      token.type_ = tokenNull;
      ok = match( L"ull", 3 );
      break;
   case L',':
      token.type_ = tokenArraySeparator;
      break;
   case L':':
      token.type_ = tokenMemberSeparator;
      break;
   case 0:
   case L';':
      token.type_ = tokenEndOfStream;
      break;
   default:
      ok = false;
      break;
   }
   if ( !ok )
      token.type_ = tokenError;
   token.end_ = current_;
   return true;
}


void 
Reader::skipSpaces()
{
   while ( current_ != end_ )
   {
      Char c = *current_;
      if ( c == L' '  ||  c == L'\t'  ||  c == L'\r'  ||  c == L'\n' )
         ++current_;
      else
         break;
   }
}


bool 
Reader::match( Location pattern, 
               int patternLength )
{
   if ( end_ - current_ < patternLength )
      return false;
   int index = patternLength;
   while ( index-- )
      if ( current_[index] != pattern[index] )
         return false;
   current_ += patternLength;
   return true;
}


bool
Reader::readComment()
{
   Location commentBegin = current_ - 1;
   Char c = getNextChar();
   bool successful = false;
   if ( c == L'*' )
      successful = readCStyleComment();
   else if ( c == L'/' )
      successful = readCppStyleComment();
   if ( !successful )
      return false;

   if ( collectComments_ )
   {
      CommentPlacement placement = commentBefore;
      if ( lastValueEnd_  &&  !containsNewLine( lastValueEnd_, commentBegin ) )
      {
         if ( c != L'*'  ||  !containsNewLine( commentBegin, current_ ) )
            placement = commentAfterOnSameLine;
      }

      addComment( commentBegin, current_, placement );
   }
   return true;
}


void 
Reader::addComment( Location begin, 
                    Location end, 
                    CommentPlacement placement )
{
   assert( collectComments_ );
   if ( placement == commentAfterOnSameLine )
   {
      assert( lastValue_ != 0 );
      lastValue_->setComment( std::wstring( begin, end ), placement );
   }
   else
   {
      if ( !commentsBefore_.empty() )
         commentsBefore_ += L"\n";
      commentsBefore_ += std::wstring( begin, end );
   }
}


bool 
Reader::readCStyleComment()
{
   while ( current_ != end_ )
   {
      Char c = getNextChar();
      if ( c == L'*'  &&  *current_ == L'/' )
         break;
   }
   return getNextChar() == L'/';
}


bool 
Reader::readCppStyleComment()
{
   while ( current_ != end_ )
   {
      Char c = getNextChar();
      if (  c == L'\r'  ||  c == L'\n' )
         break;
   }
   return true;
}


void 
Reader::readNumber()
{
   while ( current_ != end_ )
   {
      if ( !(*current_ >= L'0'  &&  *current_ <= L'9')  &&
           !in( *current_, L'.', L'e', L'E', L'+', L'-' ) )
         break;
      ++current_;
   }
}

bool
Reader::readString()
{
   Char c = 0;
   while ( current_ != end_ )
   {
      c = getNextChar();
      if ( c == L'\\' )
         getNextChar();
      else if ( c == L'"' )
         break;
   }
   return c == L'"';
}


bool 
Reader::readObject( Token &tokenStart )
{
   Token tokenName;
   std::wstring name;
   currentValue() = Value( objectValue );
   while ( readToken( tokenName ) )
   {
      bool initialTokenOk = true;
      while ( tokenName.type_ == tokenComment  &&  initialTokenOk )
         initialTokenOk = readToken( tokenName );
      if  ( !initialTokenOk )
         break;
      if ( tokenName.type_ == tokenObjectEnd  &&  name.empty() )  // empty object
         return true;
      if ( tokenName.type_ != tokenString )
         break;
      
      name = L"";
      if ( !decodeString( tokenName, name ) )
         return recoverFromError( tokenObjectEnd );

      Token colon;
      if ( !readToken( colon ) ||  colon.type_ != tokenMemberSeparator )
      {
         return addErrorAndRecover( L"Missing ':' after object member name", 
                                    colon, 
                                    tokenObjectEnd );
      }
      Value &value = currentValue()[ name ];
      nodes_.push( &value );
      bool ok = readValue();
      nodes_.pop();
      if ( !ok ) // error already set
         return recoverFromError( tokenObjectEnd );

      Token comma;
      if ( !readToken( comma )
            ||  ( comma.type_ != tokenObjectEnd  &&  
                  comma.type_ != tokenArraySeparator &&
		  comma.type_ != tokenComment ) )
      {
         return addErrorAndRecover( L"Missing ',' or '}' in object declaration", 
                                    comma, 
                                    tokenObjectEnd );
      }
      bool finalizeTokenOk = true;
      while ( comma.type_ == tokenComment &&
              finalizeTokenOk )
         finalizeTokenOk = readToken( comma );
      if ( comma.type_ == tokenObjectEnd )
         return true;
   }
   return addErrorAndRecover( L"Missing '}' or object member name", 
                              tokenName, 
                              tokenObjectEnd );
}


bool 
Reader::readArray( Token &tokenStart )
{
   currentValue() = Value( arrayValue );
   skipSpaces();
   if ( *current_ == L']' ) // empty array
   {
      Token endArray;
      readToken( endArray );
      return true;
   }
   int index = 0;
   while ( true )
   {
      Value &value = currentValue()[ index++ ];
      nodes_.push( &value );
      bool ok = readValue();
      nodes_.pop();
      if ( !ok ) // error already set
         return recoverFromError( tokenArrayEnd );

      Token token;
      if ( !readToken( token ) 
           ||  ( token.type_ != tokenArraySeparator  &&  
                 token.type_ != tokenArrayEnd ) )
      {
         return addErrorAndRecover( L"Missing ',' or ']' in array declaration", 
                                    token, 
                                    tokenArrayEnd );
      }
      if ( token.type_ == tokenArrayEnd || *current_ == L']' )
         break;
   }
   return true;
}


bool 
Reader::decodeNumber( Token &token )
{
   bool isDouble = false;
   for ( Location inspect = token.start_; inspect != token.end_; ++inspect )
   {
      isDouble = isDouble  
                 ||  in( *inspect, L'.', L'e', L'E', L'+' )  
                 ||  ( *inspect == L'-'  &&  inspect != token.start_ );
   }
   if ( isDouble )
      return decodeDouble( token );
   Location current = token.start_;
   bool isNegative = *current == L'-';
   if ( isNegative )
      ++current;
   Value::UInt threshold = (isNegative ? Value::UInt(-Value::minInt) 
                                       : Value::maxUInt) / 10;
   Value::UInt value = 0;
   while ( current < token.end_ )
   {
      Char c = *current++;
      if ( c < L'0'  ||  c > L'9' )
         return addError( L"'" + std::wstring( token.start_, token.end_ ) + L"' is not a number.", token );
      if ( value >= threshold )
         return decodeDouble( token );
      value = value * 10 + Value::UInt(c - L'0');
   }
   if ( isNegative )
      currentValue() = -Value::Int( value );
   else if ( value <= Value::UInt(Value::maxInt) )
      currentValue() = Value::Int( value );
   else
      currentValue() = value;
   return true;
}


bool 
Reader::decodeDouble( Token &token )
{
   double value = 0;
   const int bufferSize = 32;
   int count;
   int length = int(token.end_ - token.start_);
   if ( length <= bufferSize )
   {
      Char buffer[bufferSize];
      memcpy( buffer, token.start_, length * sizeof(Char) );
      buffer[length] = 0;
      count = swscanf( buffer, L"%lf", &value );
   }
   else
   {
      std::wstring buffer( token.start_, token.end_ );
      count = swscanf( buffer.c_str(), L"%lf", &value );
   }

   if ( count != 1 )
      return addError( L"'" + std::wstring( token.start_, token.end_ ) + L"' is not a number.", token );
   currentValue() = value;
   return true;
}


bool 
Reader::decodeString( Token &token )
{
   std::wstring decoded;
   if ( !decodeString( token, decoded ) )
      return false;
   currentValue() = decoded;
   return true;
}


bool 
Reader::decodeString( Token &token, std::wstring &decoded )
{
   decoded.reserve( token.end_ - token.start_ - 2 );
   Location current = token.start_ + 1; // skip '"'
   Location end = token.end_ - 1;      // do not include '"'
   while ( current != end )
   {
      Char c = *current++;
      if ( c == L'"' )
         break;
      else if ( c == L'\\' )
      {
         if ( current == end )
            return addError( L"Empty escape sequence in string", token, current );
         Char escape = *current++;
         switch ( escape )
         {
         case L'"': decoded += L'"'; break;
         case L'/': decoded += L'/'; break;
         case L'\\': decoded += L'\\'; break;
         case L'b': decoded += L'\b'; break;
         case L'f': decoded += L'\f'; break;
         case L'n': decoded += L'\n'; break;
         case L'r': decoded += L'\r'; break;
         case L't': decoded += L'\t'; break;
         case L'u':
            {
               if ( !decodeUnicodeEscapeSequence( token, current, end, decoded ) )
                  return false;
            }
            break;
         default:
            return addError( L"Bad escape sequence in string", token, current );
         }
      }
      else
      {
         decoded += c;
      }
   }
   return true;
}

bool 
Reader::decodeUnicodeEscapeSequence( Token &token, 
                                     Location &current, 
                                     Location end, 
				     std::wstring &decoded )
{
   if ( end - current < 4 )
      return addError( L"Bad unicode escape sequence in string: four digits expected.", token, current );
   
   unsigned int ch = 0;
   
   for ( int index =0; index < 4; ++index )
   {
      Char c = *current++;
      ch *= 16;
      if ( c >= L'0'  &&  c <= L'9' )
         ch += c - L'0';
      else if ( c >= L'a'  &&  c <= L'f' )
         ch += c - L'a' + 10;
      else if ( c >= L'A'  &&  c <= L'F' )
         ch += c - L'A' + 10;
      else
         return addError( L"Bad unicode escape sequence in string: hexadecimal digit expected.", token, current );
   }

   decoded += ch;

   return true;
}


bool 
Reader::addError( const std::wstring &message, 
                  Token &token,
                  Location extra )
{
   ErrorInfo info;
   info.token_ = token;
   info.message_ = message;
   info.extra_ = extra;
   errors_.push_back( info );
   return false;
}


bool 
Reader::recoverFromError( TokenType skipUntilToken )
{
   int errorCount = int(errors_.size());
   Token skip;
   while ( true )
   {
      if ( !readToken(skip) )
         errors_.resize( errorCount ); // discard errors caused by recovery
      if ( skip.type_ == skipUntilToken  ||  skip.type_ == tokenEndOfStream )
         break;
   }
   errors_.resize( errorCount );
   return false;
}


bool 
Reader::addErrorAndRecover( const std::wstring &message, 
                            Token &token,
                            TokenType skipUntilToken )
{
   addError( message, token );
   return recoverFromError( skipUntilToken );
}


Value &
Reader::currentValue()
{
   return *(nodes_.top());
}


Reader::Char 
Reader::getNextChar()
{
   if ( current_ == end_ )
      return 0;
   return *current_++;
}


void 
Reader::getLocationLineAndColumn( Location location,
                                  int &line,
                                  int &column ) const
{
   Location current = begin_;
   Location lastLineStart = current;
   line = 0;
   while ( current < location  &&  current != end_ )
   {
      Char c = *current++;
      if ( c == L'\r' )
      {
         if ( *current == L'\n' )
            ++current;
         lastLineStart = current;
         ++line;
      }
      else if ( c == L'\n' )
      {
         lastLineStart = current;
         ++line;
      }
   }
   // column & line start at 1
   column = int(location - lastLineStart) + 1;
   ++line;
}


std::wstring
Reader::getLocationLineAndColumn( Location location ) const
{
   int line, column;
   getLocationLineAndColumn( location, line, column );
   wchar_t buffer[18+16+16+1];
   wsprintf( buffer, L"Line %d, Column %d", line, column );
   return buffer;
}


std::wstring 
Reader::getFormatedErrorMessages() const
{
   std::wstring formattedMessage;
   for ( Errors::const_iterator itError = errors_.begin();
         itError != errors_.end();
         ++itError )
   {
      const ErrorInfo &error = *itError;
      formattedMessage += L"* " + getLocationLineAndColumn( error.token_.start_ ) + L"\n";
      formattedMessage += L"  " + error.message_ + L"\n";
      if ( error.extra_ )
         formattedMessage += L"See " + getLocationLineAndColumn( error.extra_ ) + L" for detail.\n";
   }
   return formattedMessage;
}

/*
std::istream& operator>>( std::istream &sin, Value &root )
{
    Json::Reader reader;
    bool ok = reader.parse(sin, root, true);
    //JSON_ASSERT( ok );
    if (!ok) {
        std::wstring wideMessage(reader.getFormatedErrorMessages());
        std::string shortMessage = "";

        int chars = WideCharToMultiByte(CP_ACP, 0, wideMessage.c_str() , wideMessage.length(), NULL, 0, NULL, NULL);

	if (chars > 0) {
            char * buffer = new char[chars + 1];

            WideCharToMultiByte(CP_ACP , 0, wideMessage.c_str(), wideMessage.length(), buffer, chars + 1, NULL, NULL);
            buffer[chars] = '\0';

	    shortMessage = buffer;

	    delete buffer;
	}

        throw std::runtime_error(shortMessage);
    }

    return sin;
}
*/

} // namespace Json
