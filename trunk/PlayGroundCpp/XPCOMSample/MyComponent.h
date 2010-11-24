#ifndef MYCOMPONENT_H_INCLUDED
#define MYCOMPONENT_H_INCLUDED


#include "IMyComponent.h"

#define MY_COMPONENT_CONTRACTID "@mydomain.com/XPCOMSample/MyComponent;1"
#define MY_COMPONENT_CLASSNAME "A Simple XPCOM Sample"
#define MY_COMPONENT_CID { 0x99d79656, 0xf804, 0x11df, { 0xaee5, 0x08, 0x00, 0x27, 0xa4, 0x54, 0x55 } } 


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
