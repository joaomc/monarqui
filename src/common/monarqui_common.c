#include "monarqui_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>
#include <glib.h>
#include <lua.h>
#include <zmq.h>

char *string_join(GList *list) 
{    
  int num_items,i,lenstr;
  lenstr = 0;
  num_items = 0;
  GList *item = g_list_first(list);
  while(item)
  {
    lenstr = lenstr + (strlen((char *)item->data)*sizeof(char));
    item = item->next;
    num_items++;
  }
  lenstr += num_items;
  char *retval = (char *)calloc(lenstr,sizeof(char));
  item = g_list_first(list);
  i = 0;
  while(item)
  {
    sprintf(&retval[strlen(retval)],"%s",(char *)item->data);    
    if(i < (num_items-1))
      sprintf(&retval[strlen(retval)],",");
    item = item->next;
    i++;
  }
  return retval;
}

int str_events_to_int(char *str) 
{  
  char *tmpstr = strdup(str);
  char *token, *rest;
  int events = 0;      
  token = strtok_r(tmpstr, STR_EVENT_SEPARATOR, &rest);
  while(token) 
  {    
    if(strcmp(token,STR_CREATE)==0)
      events = events | MON_CREATE;
    else if(strcmp(token,STR_MODIFY)==0)
      events = events | MON_MODIFY;
    else if(strcmp(token,STR_DELETE)==0)
      events = events | MON_DELETE;
    else if(strcmp(token,STR_ATTRIB)==0)
      events = events | MON_ATTRIB;
    else if(strcmp(token,STR_MOVED_FROM)==0)
      events = events | MON_MOVED_FROM;
    else if(strcmp(token,STR_MOVED_TO)==0)
      events = events | MON_MOVED_TO;
    else if(strcmp(token,STR_ACCESS)==0)
      events = events | MON_ACCESS;
    token = strtok_r(NULL, STR_EVENT_SEPARATOR, &rest);
  }  
  free(tmpstr);
  tmpstr = NULL;
  return events;
}

char *int_events_to_str(int events) 
{
  int idx_arr = 0;  
  int num_items = 0;
  int lenstr;
  const int arr_events_size = 7;	
  const char *arr_events[] = { STR_CREATE, STR_MODIFY, STR_DELETE, STR_ATTRIB, STR_MOVED_FROM, STR_MOVED_FROM, STR_ACCESS};
  int int_events[] = { MON_CREATE, MON_MODIFY, MON_DELETE, MON_ATTRIB, MON_MOVED_FROM, MON_MOVED_TO, MON_ACCESS};
  char *arr_matches[7];
  lenstr = 0;
  if(events & MON_CREATE)
  {
    arr_matches[num_items] = g_strdup(STR_CREATE);          
    lenstr += strlen(arr_matches[num_items]);
    num_items++;
  }
  if(events & MON_MODIFY)
  {
    arr_matches[num_items] = g_strdup(STR_MODIFY);          
    lenstr += strlen(arr_matches[num_items]);
    num_items++;
  }
  if(events & MON_DELETE)
  {
    arr_matches[num_items] = g_strdup(STR_DELETE);          
    lenstr += strlen(arr_matches[num_items]);
    num_items++;    
  }
  if(events & MON_ATTRIB)
  {
    arr_matches[num_items] = g_strdup(STR_ATTRIB);          
    lenstr += strlen(arr_matches[num_items]);
    num_items++;    
  }
  if(events & MON_MOVED_FROM)
  {
    arr_matches[num_items] = g_strdup(STR_MOVED_FROM);          
    lenstr += strlen(arr_matches[num_items]);
    num_items++;    
  }
  if(events & MON_MOVED_TO)
  {
    arr_matches[num_items] = g_strdup(STR_MOVED_TO);          
    lenstr += strlen(arr_matches[num_items]);
    num_items++;    
  }
  if(events & MON_ACCESS)
  {
    arr_matches[num_items] = g_strdup(STR_ACCESS);          
    lenstr += strlen(arr_matches[num_items]);
    num_items++;    
  }      
  lenstr += num_items;
  char *retval = (char *)calloc(lenstr,sizeof(char));
  for(idx_arr = 0 ; idx_arr < num_items ; idx_arr++)
  {
    sprintf(&retval[strlen(retval)],"%s",arr_matches[idx_arr]);    
    free(arr_matches[idx_arr]);
    if(idx_arr < (num_items-1))
      sprintf(&retval[strlen(retval)],",");
  }  
  return retval;
}

void show_lua_error(lua_State *L, char *msg){
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));
}
void bail(lua_State *L, char *msg){
	show_lua_error(L,msg);
	exit(1);
}
