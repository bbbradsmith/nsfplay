#ifndef _VCM_H_
#define _VCM_H_
//
// Versatile Configuration Management Library
//
#include <string>
#include <map>
#include <deque>
#include <vector>
#include <list>
#include <set>
#include <mutex>

namespace vcm
{
  // l
  class Value
  {
  public:
    // ®«
    std::string data;    // lcÇñÈ^ÅàíÉ¶ñÆµÄÛ¶
    bool update;         // Abvf[g³ê½Èçtrue

    // ì
    Value();
    Value(const char *);
    Value(const std::string &);
    Value(int);

    operator int () const ;
    int GetInt() const ;
    operator std::string () const ;
    operator const char *() const ;
    std::string GetStr() const ;

    Value &operator = ( int );
    void SetInt( int );
    Value &operator = ( const std::string & );
    void SetStr( const std::string & );
    Value &operator = ( const char * );
    void SetStr( const char * );
  };

  // RtBO[V{ÌÌè`
  class ObserverI
  {
  public:
    virtual void Notify(int)=0;
  };

  class Observable
  {
  protected:
    std::set<ObserverI *>oblist;
  public:
    virtual void AttachObserver(ObserverI *p)
    {
      oblist.insert(p);
    }

    virtual void DetachObserver(ObserverI *p)
    {
      oblist.erase(p);
    }

    virtual int GetObserverNum()
    {
      return (int)oblist.size();
    }

    virtual ObserverI *GetObserver()
    {
      if(oblist.size()!=0)
        return *oblist.begin();
      else
        return NULL;
    }

    void Notify(int id)
    {
      std::set<ObserverI *>::iterator it;
      for(it=oblist.begin();it!=oblist.end();it++)
        (*it)->Notify(id);
    }
  }; 


  class Configuration : public Observable
  {
  protected:
    std::map < std::string, Value > data;

    // thread safety
    std::mutex mutex;
  public:
    // thread safety
    Configuration() = default;
    
    // lðÇÞD³¯êÎG[D
    Value &operator[]( const std::string id )
    {
      std::lock_guard<std::mutex> mg_(mutex);
      std::map< std::string, Value >::iterator it;
      if(( it = data.find( id ) )==data.end() )
        throw std::out_of_range( id );
      else
        return it->second;
    }
    // lðìé d¡µÄìë¤Æ·éÆ false ªAÁÄ¸s
    bool CreateValue( const std::string id, const Value &value )
    {
      std::lock_guard<std::mutex> mg_(mutex);
      std::map< std::string, Value >::iterator it;
      if(( it = data.find( id ) )!=data.end() )
        return false;
      else
      {
        data[id] = value;
        return true;
      }
    }
    // lðZbg·éD³¯êÎìéD
    inline void SetValue( const std::string id, const Value &value )
    {
      std::lock_guard<std::mutex> mg_(mutex);
      data[id] = value;
    }
    // lðÇÞD³¯êÎ©®IÉì¬³êéD
    inline Value &GetValue( const std::string id )
    {
      std::lock_guard<std::mutex> mg_(mutex);
      return data[id];
    }
    inline void Clear()
    { 
      std::lock_guard<std::mutex> mg_(mutex);
      data.clear(); 
    }
    void Read( Configuration &src )
    {
      std::lock_guard<std::mutex> mg_(mutex);
      std::map<std::string, Value>::iterator it;
      for(it=data.begin();it!=data.end();it++)
        it->second = src[it->first];
    }
    void Write( Configuration &src )
    {
      std::lock_guard<std::mutex> mg_(mutex);
      std::map<std::string, Value>::iterator it;
      for(it=data.begin();it!=data.end();it++)
        src[it->first]=it->second;
    }
  };

} // namespace vcm

#include "vcm_ui.h"

#endif