#ifndef COMMON_H
#define COMMON_H
#include <ctype.h>
#include <cstring>
#include <stdio.h>
#include <map>
#include <stdlib.h>
#include <vector>

#include <string>
#include <iostream>
#include <typeinfo>
#include <assert.h>

#define HIER_SEPARATOR "/"

const char *new_string(const char *name);
void delete_string(char *_name);

using namespace std;


struct eqstr {  
  bool operator()(const char* s1, const char* s2) const {
    return strcmp(s1, s2);  
  }
};

#define DictType       map<const char*, int, eqstr>

extern DictType dict;

inline const char *new_string(const char *name){
  DictType::iterator i;
  char *_name;
  i = dict.find(name);
  if(i!=dict.end()) {
    _name = (char*)i->first;
    i->second++;
  } 
  
  else {
    _name = strdup(name);
    dict[_name] = 1;
  }

  return _name;
}

inline void delete_string(char *_name){
  DictType::iterator i;
  //      cout << "Delete "<< _name << endl;
  i = dict.find(_name);
  if(i==dict.end()) {
    cerr << "Over delete "<< _name << endl;
    return;
  }

  i->second--;
  if(i->second == 0) {
    // delete i->first;
    // dict.erase(i);
  }
}

class CharBuf{
  char *charbuf,*charbuf_ptr;
  unsigned int charbuf_len;
 public:
  CharBuf(){
    charbuf=NULL;
    charbuf_len=0;
  }
  ~CharBuf(){
    //cerr << "Free CharBuf=" << charbuf << endl;
    if(charbuf)free(charbuf);
  }
  void init(){
    charbuf_ptr = charbuf;  
  }
  char *str(){ return charbuf;}

  char *append_str(const char *s){
    unsigned int old_len=charbuf_ptr-charbuf;  
    unsigned int delta_len=strlen(s);
    unsigned int new_len=old_len+delta_len+1;
    if(new_len>charbuf_len){
      charbuf=(char*)realloc(charbuf,new_len);
      charbuf_len=new_len;
      charbuf_ptr=charbuf+old_len;
    }
    
    strcpy(charbuf_ptr,s);
    charbuf_ptr+=delta_len;
    return charbuf;
  }

  char *append_char(char c){
    unsigned int old_len=charbuf_ptr-charbuf;  
    unsigned int new_len=old_len+2;
    if(new_len>charbuf_len){
      charbuf=(char*)realloc(charbuf,new_len);
      charbuf_len=new_len;
      charbuf_ptr=charbuf+old_len;
    }
    
    *charbuf_ptr++=c;
    *charbuf_ptr='\0';
    return charbuf;
  }
};

extern CharBuf token;

// implemented in verilog.y - used anywhere
string busNodeName(string busName, int index);


/*! \def stringify(name)
    \brief convert the argument \a name to a string with the value of \a "name".
*/
#define stringify( name ) # name

// return codes:
/*! \var typedef enum hcmResultTypes hcmRes
    \brief Object representing optional return types from the HCM requests.
*/
typedef enum hcmResultTypes { 
  OK,                                   /**< Successfull operation.*/
  BAD_PARAM,                            /**< The parmeter supplied to the function is not valid.*/ 
  NO_MEM,                               /**< .*/
  NOT_FOUND,                            /**< Property not found.*/
  PROPERTY_EXISTS_WITH_DIFFERENT_TYPE   /**< Property exists but with diffrent type.*/ 
} hcmRes;

// wire types:
/*! \var typedef enum NetType_ NetType
    \brief Object representing type of wire.
*/
typedef enum NetType_ { 
  NullNet,                              /**< .*/
  WireNet,                              /**< .*/
  TriNet,                               /**< .*/
  WandNet,                              /**< .*/
  WorNet,                               /**< .*/
  RegNet,                               /**< .*/
  Supply1Net,                           /**< .*/
  Supply0Net                            /**< .*/
} NetType;

// port types:
/*! \var typedef enum hcmPortTypes hcmPortDir
    \brief Object representing type of wire.
*/
typedef enum hcmPortTypes { 
  NOT_PORT,                             /**< .*/ 
  IN,                                   /**< .*/ 
  OUT,                                  /**< .*/
  IN_OUT                                /**< .*/
} hcmPortDir;

/**! \struct Range hcm.h
 * \brief this struct represent a wire with direction, type and boundaries
 * 
 */
struct Range {
  hcmPortDir dir;
  NetType type;
  int upper,lower;
};

class hcmProperty;
class hcmObject;
class hcmDesign;
class hcmCell;
class hcmInstance;
class hcmNode;
class hcmInstPort;
class hcmPort;

#endif
