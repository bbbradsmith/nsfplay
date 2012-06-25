#include <cstdio>
#include "vcm.h"

using namespace vcm;

Value::Value ( ) : data(""), update(false)
{
}

Value::Value (int i) : update(false)
{
  SetInt(i);
}

Value::Value (const std::string &s) : update(false)
{
  SetStr(s);
}

Value::Value (const char *s) : update(false)
{
  SetStr(s);
}

int Value::GetInt() const 
{
  return ::atoi( this->data.c_str() );
}

Value::operator const char * () const
{
  return this->data.c_str();
}

Value::operator int() const 
{
  return ::atoi( this->data.c_str() );
}

std::string Value::GetStr() const 
{
  return this->data;
}

Value::operator std::string() const
{
  return this->data;
}

void Value::SetInt( int i )
{
  char buf[16];
  this->update = true;
  this->data = itoa( i, buf, 10 );
}

Value &Value::operator = ( int i )
{
  char buf[16];
  this->update = true;
  this->data = itoa( i, buf, 10 );
  return *this;
}

void Value::SetStr( const char *str )
{
  this->update = true;
  this->data = (std::string)(str);
}

Value &Value::operator =( const char *str )
{
  this->update = true;
  this->data = (std::string)(str);
  return *this;
}

void Value::SetStr( const std::string &str )
{
  this->update = true;
  this->data = str;
}

Value &Value::operator =( const std::string &str )
{
  this->update = true;
  this->data = str;
  return *this;
}

