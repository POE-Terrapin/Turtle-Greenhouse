#ifndef __UTILS_H__
#define __UTILS_H__

template<typename T>
void serialWrite(T val){
  Serial.println(val);
}

template<typename H, typename... T>
void serialWrite(H head, T... tail){
  Serial.print(head);
  Serial.print(',');
  serialWrite(tail...);  
}

#endif

