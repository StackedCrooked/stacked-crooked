#ifndef MYCOMPONENT_H_INCLUDED
#define MYCOMPONENT_H_INCLUDED


#include "IMyComponent.h"

#define MY_COMPONENT_CONTRACTID "@mydomain.com/XPCOMSample/MyComponent;1"
#define MY_COMPONENT_CLASSNAME "A Simple XPCOM Sample"
#define MY_COMPONENT_CID { 0xd21b0c44, 0x9519, 0x4144, { 0x5d6, 0xec, 0x0a, 0x41, 0x41, 0xbf, 0xc2 } }


class MyComponent : public IMyComponent
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_IMYCOMPONENT

  MyComponent();

private:
  ~MyComponent();
};


#endif // MYCOMPONENT_H_INCLUDED
