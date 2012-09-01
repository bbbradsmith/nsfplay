#include "vcm.h"

using namespace vcm;

ConfigGroup::ConfigGroup( const std::string &l, const std::string &d, Configuration *b/*=NULL*/ )
  : label(l), desc(d)
{
}

ConfigGroup::~ConfigGroup()
{
  Clear();
}

bool ConfigGroup::AddSubGroup( ConfigGroup *sub )
{
  subGroup.push_back(sub);
  return true;
}

bool ConfigGroup::Insert( const std::string &id, ValueCtrl *ctrl, ValueConv *conv /*=NULL*/ )
{
  if(conv) ctrl->AddConv( conv );
  this->members.push_back( std::pair < const std::string, ValueCtrl * >( id, ctrl ) );
  return true;
}

bool ConfigGroup::Insert( const std::string &id, ValueCtrl *ctrl, std::vector<ValueConv *> convs )
{
  ctrl->AddConv( convs );
  this->members.push_back( std::pair < const std::string, ValueCtrl * >( id, ctrl ) );
  return true;
}

void ConfigGroup::Clear()
{
  GroupList::iterator it;
  for( it=this->members.begin(); it!=this->members.end(); it++ )
    delete (*it).second;
  members.clear();
}
