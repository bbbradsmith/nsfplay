#ifndef _VCM_UI_H_
#define _VCM_UI_H_

#include <cstdlib>

namespace vcm
{
  class ValueCtrl;

  // ValueCtrl�ϊ�
  class ValueConv
  {
  public:
    virtual ~ValueConv(){}
    // �����l�����J�l�ɕϊ�����D�ϊ��Ɏ��s����ꍇ�� false ��Ԃ��D
    virtual bool GetExportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &src_value, Value &result ) 
    {
      result = src_value;
      return true;
    }
    // ���J�l������l�ɕϊ�����D�ϊ��Ɏ��s������Cfalse ��Ԃ��D
    virtual bool GetImportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &src_value, Value &result )
    {
      result = src_value;
      return true;
    }
  };

  // �l�̃��x���C���� �� ����
  class ValueCtrl
  {
  public:
    enum CtrlType { CT_INVALID, CT_CHECK, CT_SPIN, CT_SLIDER, CT_TEXT, CT_COMBO, CT_ENUM, CT_NOBREAK, CT_USER };
    std::string label;
    std::string desc;
    const int ctrlType;
    std::vector<ValueConv *> vcs;

    explicit ValueCtrl ( const std::string &l, const std::string &d, const CtrlType t )
      : label(l), desc(d), ctrlType(t) {}

    void AddConv ( ValueConv *vc )
    {
      vcs.push_back ( vc );
    }

    void AddConv ( std::vector<ValueConv *> &convs )
    {
      vcs.insert( vcs.begin(), convs.begin(), convs.end() );
    }

    void ClearConv ( )
    {
      for( std::vector<ValueConv *>::iterator it=vcs.begin(); it!=vcs.end(); it++ )
        delete *it;
      vcs.clear();
    }

    bool Export ( Configuration &cfg, const std::string &id, Value &result )
    {
      result = cfg[id];

      for( std::vector<ValueConv *>::reverse_iterator it=vcs.rbegin(); it!=vcs.rend(); it++ )
      {
        Value tmp = result;
        if(!(*it)->GetExportValue( this, cfg, id, tmp, result ))
          return false;
      }

      return true;
    }

    bool Import ( Configuration &cfg, const std::string &id, Value &result )
    {
      for( std::vector<ValueConv *>::iterator it=vcs.begin(); it!=vcs.end(); it++ )
      {
        Value tmp = result;
        if(!(*it)->GetImportValue( this, cfg, id, tmp, result  ))
          return false;
      }

      cfg[id] = result; 
      return true;
    }

    virtual ~ValueCtrl()
    {
      ClearConv();
    }
  };

  //
  // �R���t�B�O���̃O���[�v��
  //
  class ConfigGroup
  {
  public:
    typedef std::deque < std::pair< std::string, ValueCtrl * > > GroupList;
    GroupList members;     // ���̃O���[�v�Ɋ܂܂�鍀��
    std::string label;     // ���̃O���[�v�̃��x��
    std::string desc;      // ����

    std::list< ConfigGroup * > subGroup;

    ConfigGroup( const std::string &label, const std::string &desc, Configuration *base=NULL );
    ~ConfigGroup();
    bool AddSubGroup( ConfigGroup *sub );
    bool Insert( const std::string &id, ValueCtrl *type, ValueConv *conv=NULL );
    bool Insert( const std::string &id, ValueCtrl *type, std::vector<ValueConv *>convs );
    void UpdateData( Configuration &config, bool d ); // �R���t�B�O�����o��������ꂽ��
    void Clear();
    void NotifyUpdate( const std::string &id ); // �X�V�ʒm
  };

  // ���ɂ��}�b�v����Ȃ��f�[�^�����̑���
  class VT_HIDDEN : public ValueCtrl {};

  class VT_LABEL : public ValueCtrl {};

  // bool�l�Ƃ��ă`�F�b�N�{�b�N�X�Ƀ}�b�v
  class VT_CHECK : public ValueCtrl
  {
  public:
    VT_CHECK ( const std::string &label, const std::string &desc )
      : ValueCtrl ( label, desc, CT_CHECK ){}
  };

  // �ő�C�ŏ��l�̐���t���ϊ�
  class VC_RANGE : public ValueConv
  {
  public:
    int minValue, maxValue;
    VC_RANGE( int min, int max ) : ValueConv(), minValue(min), maxValue(max){}
    virtual bool GetExportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &src_value, Value &result )
    {
      if ( minValue <= (int)src_value && (int)src_value <= maxValue )
      {
        result = src_value;
        return true;
      }
      else
      {
        return false;
      }
    }
    virtual bool GetImportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &src_value, Value &result )
    {
      if ( minValue <= (int)src_value && (int)src_value <= maxValue )
      {
        result = src_value;
        return true;
      }
      else
      {
        return false;
      }
    }
  };

  // ���l�Ƃ��ăe�L�X�g�̈�Ƀ}�b�v����
  class VT_SPIN : public ValueCtrl
  {
  public:
    int minValue, maxValue;
    VT_SPIN ( const std::string &label, const std::string &desc, int min, int max )
      : ValueCtrl( label, desc, CT_SPIN ), minValue( min ), maxValue( max )
    {
      AddConv ( new VC_RANGE( min, max ) );
    }
  };

  // ���l�Ƃ��ăX���C�_�[�R���g���[���Ƀ}�b�v����
  class VT_SLIDER : public ValueCtrl
  {
  public:
    int minValue, maxValue;
    std::string minDesc, maxDesc;
    int ticFreq, pageSize;
    VT_SLIDER ( const std::string &label, const std::string &desc, int min, int max,
      const std::string &min_desc, const std::string max_desc, int tic_freq, int page_size )
      : ValueCtrl( label, desc, CT_SLIDER ), minValue(min), maxValue(max), 
        minDesc(min_desc), maxDesc(max_desc), ticFreq(tic_freq), pageSize(page_size)
    {
      AddConv ( new VC_RANGE( min, max ) );
    }
  };

  // ������̒�������
  class VC_LENGTH : public ValueConv
  {
  public:
    unsigned long maxLength ;
    VC_LENGTH( unsigned long length ) : maxLength( length ) {} 
    virtual bool GetImportValue ( ValueCtrl *, Configuration &, const std::string &id, const Value &src_value, Value &result )
    {
      if( (src_value.GetStr()).length() <= maxLength )
      {
        result = src_value;
        return true;
      }
      else
      {
        return false;
      }
    }
  };

  // �h���b�v�_�E�����X�g
  class VT_COMBO : public ValueCtrl
  {
  public:
    std::vector<std::string> items;
    VT_COMBO ( const std::string &label, const std::string &desc, const std::vector<std::string> &list )
      : ValueCtrl ( label, desc, CT_COMBO )
    {
      items = list;
    }
  };

  class VC_STR2INT : public ValueConv
  {
  public:
    std::map<std::string, int> s2i;
    std::map<int, std::string> i2s;

    VC_STR2INT( const std::map<std::string,int> &map )
    {
      s2i = map;
      for(std::map<std::string,int>::const_iterator it=map.begin(); it!=map.end();it++)
        i2s[it->second]=it->first;
    }
    
    virtual bool GetExportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &src_value, Value &result )
    {
      std::map<int, std::string>::iterator it=i2s.find(src_value);
      if(it!=i2s.end())
      {
        result = it->second;
        return true;
      }
      else
      {
        return false;
      }  
    }

    virtual bool GetImportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &src_value, Value &result )
    {
      std::map<std::string,int>::iterator it=s2i.find(src_value);
      if(it!=s2i.end())
      {
        result = it->second;
        return true;
      }
      else
      {
        return false;
      }  
    }
  };

  class VT_COMBO_INT : public VT_COMBO
  {
  public:
    std::vector<std::string> items;
    VT_COMBO_INT ( const std::string &label, const std::string &desc, const std::vector<std::string> &list, const std::map<std::string, int> &map )
      : VT_COMBO( label, desc, list )
    {
      AddConv( new VC_STR2INT ( map ) );
    }
  };


  // ������Ƃ��ăe�L�X�g�̈�Ƀ}�b�v����
  class VT_TEXT : public ValueCtrl
  {
  public:
    unsigned long maxLength;
    VT_TEXT ( const std::string &label, const std::string &desc, unsigned long max_length )
      : ValueCtrl( label, desc, CT_TEXT ), maxLength( max_length )
    {
      AddConv ( new VC_LENGTH ( max_length ) );
    }
  };

  class VC_TIME : public ValueConv
  {
  public:
    VC_TIME()
    {
    }
  
    virtual bool GetExportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &src_value, Value &result )
    {
      int time = (int)src_value / 1000;
      char buf[32];

      if(time<0) time = 0;
      sprintf(buf,"%02d:%02d:%02d", (time/3600), (time/60)%60, time%60 );

      result = ((std::string)buf);
      return true;
    }

    virtual bool GetImportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &src_value, Value &result )
    {
      const char *str = src_value;
      char buf[16];
      unsigned int i,j=0;
      int tmp = 0;

      for( i=0; str[i]!='\0'; i++ )
      {
        if( str[i]==' '||str[i]=='\t' )
        {
          continue;
        }
        else if( '0' <= str[i] && str[i] <= '9' && j <= 14 )
        {
          buf[j++] = str[i];
        }
        else if( (str[i] == ':'||str[i]=='\0') && j )
        {
          buf[j] = '\0';
          tmp *= 60;
          tmp += ::atoi( buf );
          j=0;
        }
        else
        {
          return false;
        }
      }

      if( j )
      {
        buf[j]='\0';
        tmp *= 60;
        tmp += ::atoi( buf );
      }

      result = tmp * 1000;
      return true;
    }
  };

} // namespace vcm;

#endif
