

#ifndef __TAIL_EVENT_LIST__
#define __TAIL_EVENT_LIST__

#define EVENT_NAME_SIZE (255)
#define EVENT_PARAM_SIZE (1024)

class CTailEvent
{
public:
  char szName[EVENT_NAME_SIZE];
  char szType[EVENT_NAME_SIZE];
  char szParameters[EVENT_PARAM_SIZE];

public:
  virtual DWORD Action (const char* pszContext1, const char* pszContext2) = 0;

};

class CTailEmailEvent : public CTailEvent
{
public:
  CTailEmailEvent ()
  {
    pEvent = NULL;
    strcpy (szType, "E-Mail");
  }

  CTailEmailEvent (const char* pszName)
  {
    pEvent = NULL;
    strcpy (szType, "E-Mail");
    strcpy (szName, pszName);
  }

public:
  CTailEvent* pEvent;

  DWORD Action (const char* pszContext1, const char* pszContext2);
};

class CEventListItem
{
public:
  CEventListItem () 
  {
      pEvent = NULL; 
      memset (szName, 0, EVENT_NAME_SIZE); 
  }

  char szName[EVENT_NAME_SIZE + 1];

private:
  CTailEvent* pEvent;

};

class CEventList
{
public:
  CEventList ()
  {
    pItems = new CObList;
  }

  ~CEventList ();
  void AddEvent (CTailEvent* pEvent);
  CTailEvent* FindEvent (const char* pszName);


private:
  CObList* pItems;

};

#endif // #ifndef __TAIL_EVENT_LIST__
